/**
 * @file emlTSB.c
 * @brief TSB (Tone Sequence Block) sound event sequencer for the Capcom Sound Engine.
 *
 * Processes TSB sound event commands: key-on/off, stop, LFO modulation, and echo
 * (repeated playback with volume decay). Events can chain via link fields to create
 * multi-layered sound effects. Routes final playback through the emlShim abstraction.
 *
 * Part of the CapSndEng (Capcom Sound Engine) middleware layer.
 * Originally from the PS2 sound middleware.
 */

#include "sf33rd/AcrSDK/MiddleWare/PS2/CapSndEng/emlTSB.h"
#include "common.h"
#include "port/sound/emlShim.h"
#include "sf33rd/AcrSDK/MiddleWare/PS2/CapSndEng/emlMemMap.h"
#include "sf33rd/AcrSDK/MiddleWare/PS2/CapSndEng/emlRefPhd.h"

#include <memory.h>
#include <stdio.h>

#define ECHO_INDEX_MASK 0xF
#define TSB_LINK_TERMINATOR 0xFFFF
#define CSE_CMD_SND_START 0x10000000
#define CSE_CMD_SND_LFO 0x10000004

// Forward decls
static s32 mlTsbKeyOn(SoundEvent* pTSB, CSE_REQP* pReqp, u32 bank, u32 prog);
static void mlTsbSetToReqp(CSE_REQP* pReqp, SoundEvent* pTSB, u16 bank);
static s32 mlTsbInitEchoWork();
static s32 mlTsbMoveEchoWork();
static CSE_ECHOWORK* mlTsbPickupEchoWork(u32 index);
static s32 mlTsbCreateEcho(u32 bank, u32 code, s32* pRtpc);
static s32 mlTsbStopEcho(u32 bank, u32 code);
static s32 mlTsbStopEchoAll();
static s32 StartSound(CSE_PHDP* pPHDP, CSE_REQP* pREQP);
static s32 PlaySe(CSE_REQP* pReqp, u16 bank, u16 prog);

SoundEvent* gpTsb[TSB_MAX];          /**< Per-bank TSB data pointers */
CSE_ECHOWORK EchoWork[ECHOWORK_MAX]; /**< Echo work slots for repeated playback */

/** @brief Initialize the TSB sequencer and clear all echo work slots. */
s32 mlTsbInit() {
    mlTsbInitEchoWork();
    return 0;
}

/** @brief Execute one tick of the TSB server — processes active echo work slots. */
s32 mlTsbExecServer() {
    mlTsbMoveEchoWork();
    return 0;
}

/** @brief Stop all active TSB echo work slots. */
s32 mlTsbStopAll() {
    mlTsbStopEchoAll();
    return 0;
}

/** @brief Register a TSB data array for a given bank index. */
s32 mlTsbSetBankAddr(u32 bank, SoundEvent* addr) {
    if (bank >= TSB_MAX) {
        return -1;
    }

    gpTsb[bank] = addr;
    return 0;
}

/** @brief Get the SoundEvent entry for a given bank and event code. */
SoundEvent* mlTsbGetDataAdrs(u32 bank, u32 code) {
    if (bank >= TSB_MAX || gpTsb[bank] == NULL) {
        return NULL;
    }

    return &gpTsb[bank][code];
}

/** @brief Trigger a sound key-on event, applying flag logic before playback. */
s32 mlTsbKeyOn(SoundEvent* pTSB, CSE_REQP* pReqp, u32 bank, u32 prog) {
    if (pTSB->flags & 2) {
        pReqp->flags = pTSB->flags & ~8;
    } else {
        pReqp->flags = pTSB->flags | 8;
    }

    PlaySe(pReqp, bank, prog);
    return 0;
}

/** @brief Process a TSB sound request, dispatching by command type and following links. */
s32 mlTsbRequest(u16 bank, u16 code, s32* aRtpc) {
    CSE_REQP reqp = {};
    SoundEvent* pTSB;


    while (1) {
        pTSB = mlTsbGetDataAdrs(bank, code);
        if (pTSB == NULL) {
            return -1;
        }

        mlTsbSetToReqp(&reqp, pTSB, bank);
        reqp.note += aRtpc[1];
        reqp.id1 += aRtpc[2];
        reqp.id2 += aRtpc[3];
        reqp.prio += aRtpc[4];
        reqp.vol += aRtpc[5];
        reqp.pan += aRtpc[6];
        reqp.pitch += aRtpc[7];
        reqp.kofftime += aRtpc[8];
        reqp.limit += aRtpc[9];

        switch (pTSB->cmd) {
        case 0:
        case 4:
        case 8:
        case 9:
        case 10:
        case 11:
        case 12:
        default:
            // Do nothing
            break;

        case 6:
            mlTsbCreateEcho(bank, code, aRtpc);
            break;

        case 7:
            mlTsbStopEcho(bank, code);
            break;

        case 1:

            mlTsbKeyOn(pTSB, &reqp, bank, pTSB->prog + aRtpc[0]);
            break;

        case 2:
            emlShimSeKeyOff(&reqp);
            break;

        case 3:
            emlShimSeStop(&reqp);
            break;

        case 5:
            CSE_SYS_PARAM_LFO lfo_param;
            lfo_param.cmd = CSE_CMD_SND_LFO;
            lfo_param.reqp = reqp;
            lfo_param.pmd_speed = pTSB->param0;
            lfo_param.pmd_depth = pTSB->param1;
            lfo_param.amd_speed = pTSB->param2;
            lfo_param.amd_depth = pTSB->param3;
            emlShimSeSetLfo(&lfo_param);
            break;
        }

        if (((pTSB->link) != TSB_LINK_TERMINATOR) && (pTSB->link != code)) {
            code = (u16)pTSB->link;
        } else {
            break;
        }
    }
    return 0;
}

/** @brief Copy SoundEvent fields into a CSE_REQP request parameter block. */
void mlTsbSetToReqp(CSE_REQP* pReqp, SoundEvent* pTSB, u16 bank) {
    pReqp->flags = pTSB->flags;
    pReqp->bank = bank;
    pReqp->note = pTSB->note;
    pReqp->vol = pTSB->vol;
    pReqp->pan = pTSB->pan;
    pReqp->pitch = pTSB->pitch;
    pReqp->prio = pTSB->prio;
    pReqp->id1 = pTSB->id1;
    pReqp->id2 = pTSB->id2;
    pReqp->kofftime = pTSB->kofftime;
    pReqp->attr = pTSB->attr;
    pReqp->limit = pTSB->limit;
}

/** @brief Clear all echo work slots to zero. */
s32 mlTsbInitEchoWork() {
    u32 i;

    for (i = 0; i < ECHOWORK_MAX; i++) {
        memset(&EchoWork[i], 0, sizeof(CSE_ECHOWORK));
    }

    return 0;
}

/** @brief Tick all active echo work slots — decrement interval, replay with volume decay. */
s32 mlTsbMoveEchoWork() {
    u32 i;
    CSE_ECHOWORK* pEchoWork;
    SoundEvent* pTSB;
    CSE_REQP reqp = {};

    for (i = 0; i < ECHOWORK_MAX; i++) {
        pEchoWork = &EchoWork[i];

        if (pEchoWork->BeFlag == 1) {
            pEchoWork->CurrInterval--;

            if (pEchoWork->CurrInterval == 0) {
                pTSB = mlTsbGetDataAdrs(pEchoWork->Bank, pEchoWork->Code);
                if (pTSB == NULL) {
                    pEchoWork->BeFlag = 0;
                    continue;
                }

                mlTsbSetToReqp(&reqp, pTSB, pEchoWork->Bank);
                reqp.note += pEchoWork->Rtpc[1];
                reqp.id1 += pEchoWork->Rtpc[2];
                reqp.id2 += pEchoWork->Rtpc[3];
                reqp.prio += pEchoWork->Rtpc[4];
                reqp.vol += pEchoWork->Rtpc[5];
                reqp.pan += pEchoWork->Rtpc[6];
                reqp.pitch += pEchoWork->Rtpc[7];
                reqp.kofftime += pEchoWork->Rtpc[8];
                reqp.limit += pEchoWork->Rtpc[9];
                mlTsbKeyOn(pTSB, &reqp, pEchoWork->Bank, (pTSB->prog + pEchoWork->Rtpc[0]));
                pEchoWork->CurrTimes--;

                if (pEchoWork->CurrTimes == 0) {
                    pEchoWork->BeFlag = 0;
                } else {
                    pEchoWork->CurrInterval = pEchoWork->Interval;

                    if (pEchoWork->CurrTimes == (pEchoWork->Times - 1)) {
                        pEchoWork->Rtpc[5] -= pEchoWork->VolDec1st;
                    } else {
                        pEchoWork->Rtpc[5] -= pEchoWork->VolDec;
                    }

                    if (pEchoWork->Rtpc[5] < -127) {
                        pEchoWork->BeFlag = 0;
                    }
                }
            }
        }
    }

    return 0;
}

/** @brief Allocate and reset an echo work slot at the given index. */
CSE_ECHOWORK* mlTsbPickupEchoWork(u32 index) {
    if (index >= ECHOWORK_MAX) {
        return NULL;
    }

    memset(&EchoWork[index], 0, sizeof(CSE_ECHOWORK));
    EchoWork[index].BeFlag = 1;
    return &EchoWork[index];
}

/** @brief Create a new echo effect from a TSB event's parameters. */
s32 mlTsbCreateEcho(u32 bank, u32 code, s32* pRtpc) {
    CSE_ECHOWORK* pEchoWork;
    SoundEvent* pTsb;

    pTsb = mlTsbGetDataAdrs(bank, code);
    if (pTsb == NULL) {
        return -1;
    }

    pEchoWork = mlTsbPickupEchoWork(pTsb->id1 & ECHO_INDEX_MASK);

    if (pEchoWork == NULL) {
        return -1;
    }

    pEchoWork->Bank = bank;
    pEchoWork->Code = code;
    pEchoWork->Interval = pTsb->param0;
    pEchoWork->VolDec1st = pTsb->param1;
    pEchoWork->VolDec = pTsb->param2;
    pEchoWork->Times = pTsb->param3;
    pEchoWork->CurrInterval = 1;
    pEchoWork->CurrTimes = pEchoWork->Times;
    memcpy(pEchoWork->Rtpc, pRtpc, sizeof(pEchoWork->Rtpc));
    return 0;
}

/** @brief Stop a specific echo effect by looking up its work slot from the TSB event. */
s32 mlTsbStopEcho(u32 bank, u32 code) {
    SoundEvent* pTsb = mlTsbGetDataAdrs(bank, code);
    if (pTsb == NULL) {
        return -1;
    }

    u32 echo_idx = pTsb->id1 & ECHO_INDEX_MASK;
    if (echo_idx < ECHOWORK_MAX) {
        EchoWork[echo_idx].BeFlag = 0;
    }
    return 0;
}

/** @brief Stop all active echo effects by clearing every work slot's flag. */
s32 mlTsbStopEchoAll() {
    u32 i;

    for (i = 0; i < ECHOWORK_MAX; i++) {
        EchoWork[i].BeFlag = 0;
    }

    return 0;
}

/** @brief Build a SNDSTART system param and route it through the shim layer. */
static s32 StartSound(CSE_PHDP* pPHDP, CSE_REQP* pREQP) {
    CSE_SYS_PARAM_SNDSTART param;

    param.cmd = CSE_CMD_SND_START;
    param.phdp = *pPHDP;
    param.reqp = *pREQP;

    emlShimStartSound(&param);

    return 0;
}

/** @brief Play a sound effect by resolving PHD splits and starting each matching voice. */
static s32 PlaySe(CSE_REQP* pReqp, u16 bank, u16 prog) {
    _ps2_head_chunk* pHEAD;
    CSE_PHDPADDR PhdPAddr = {};
    CSE_PHDP phdp;
    s32 NumSplit;
    s32 i;
    s32 result;

    pHEAD = mlMemMapGetPhdAddr(bank);
    NumSplit = GetNumSplit(pHEAD, prog);


    for (i = 0; i < NumSplit; i++) {
        result = GetPhdParam(&PhdPAddr, pHEAD, prog, pReqp->note, i);

        if (result >= 0) {
            CalcPhdParam(&phdp, &PhdPAddr, pReqp->note, mlMemMapGetBankAddr(bank));
            StartSound(&phdp, pReqp);
        }
    }

    return 0;
}
