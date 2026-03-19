/**
 * @file emlRefPhd.c
 * @brief PHD (Program Header Data) sound data parser for the Capcom Sound Engine.
 *
 * Validates and navigates PHD file chunks (Head, Prog, Smpl, Vagi) to extract
 * sound parameters needed for playback: volume, pan, pitch, ADSR envelope,
 * and VAG sample addresses. Each chunk is identified by a 4-byte magic tag.
 *
 * Part of the CapSndEng (Capcom Sound Engine) middleware layer.
 * Originally from the PS2 sound middleware.
 */

#include "sf33rd/AcrSDK/MiddleWare/PS2/CapSndEng/emlRefPhd.h"
#include "common.h"

#include <string.h>

#define PHD_PAN_CENTER 64
#define PHD_PAN_MIN (-64)
#define PHD_PAN_MAX 63
#define PHD_VOL_NORMALIZE 127
#define CENTS_PER_SEMITONE 100

/** @brief Validate a Head chunk by checking its "Head" magic tag. */
s32 IsSafeHeadChunk(_ps2_head_chunk* pHEAD) {
    if (strncmp((s8*)pHEAD, "Head", 4) == 0) {
        return 1;
    }

    return 0;
}

/** @brief Validate a Prog chunk by checking its "Prog" magic tag. */
s32 IsSafeProgChunk(_ps2_prog_chunk* pPROG) {
    if (strncmp((char*)pPROG, "Prog", 4) == 0) {
        return 1;
    }

    return 0;
}

/** @brief Validate a Smpl chunk by checking its "Smpl" magic tag. */
s32 IsSafeSmplChunk(_ps2_smpl_chunk* pSMPL) {
    if (strncmp((char*)pSMPL, "Smpl", 4) == 0) {
        return 1;
    }

    return 0;
}

/** @brief Validate a Vagi chunk by checking its "Vagi" magic tag. */
s32 IsSafeVagiChunk(_ps2_vagi_chunk* pVAGI) {
    if (strncmp((char*)pVAGI, "Vagi", 4) == 0) {
        return 1;
    }

    return 0;
}

/** @brief Get the number of key splits for a program by navigating Head → Prog chunks. */
s32 GetNumSplit(_ps2_head_chunk* pHEAD, u8 prog) {
    _ps2_prog_chunk* pPROG;
    _ps2_prog_param* pPPRM;
    u32 offset;

    if (IsSafeHeadChunk(pHEAD) != 1) {
        return -1;
    }

    pPROG = (_ps2_prog_chunk*)((uintptr_t)pHEAD + (u32)pHEAD->progChunkOffset);
    if (IsSafeProgChunk(pPROG) != 1) {
        return -2;
    }

    if (pPROG->maxProgNum < prog) {
        return -11;
    }

    offset = pPROG->progParamOffset[prog];
    if (offset == -1) {
        return -11;
    }

    pPPRM = (_ps2_prog_param*)((uintptr_t)pPROG + offset);
    return pPPRM->nSplit;
}

/** @brief Look up PHD parameter addresses for a specific program/note/split index. */
s32 GetPhdParam(CSE_PHDPADDR* pHDPA, _ps2_head_chunk* pHEAD, u8 prog, u8 note, u8 index) {
    _ps2_prog_chunk* pPROG;
    _ps2_smpl_chunk* pSMPL;
    _ps2_vagi_chunk* pVAGI;
    _ps2_split_block* pSBLK;
    _ps2_prog_param* pPPRM;
    _ps2_smpl_param* pSPRM;
    _ps2_vagi_param* pVPRM;

    pPROG = (_ps2_prog_chunk*)((uintptr_t)&pHEAD->tag + (u32)pHEAD->progChunkOffset);
    pSMPL = (_ps2_smpl_chunk*)((uintptr_t)&pHEAD->tag + (u32)pHEAD->smplChunkOffset);
    pVAGI = (_ps2_vagi_chunk*)((uintptr_t)&pHEAD->tag + (u32)pHEAD->vagiChunkOffset);

    if (IsSafeHeadChunk(pHEAD) != 1) {
        return -1;
    }

    if (IsSafeProgChunk(pPROG) != 1) {
        return -2;
    }

    if (IsSafeSmplChunk(pSMPL) != 1) {
        return -3;
    }

    if (IsSafeVagiChunk(pVAGI) != 1) {
        return -4;
    }

    pPPRM = (_ps2_prog_param*)((uintptr_t)pPROG + pPROG->progParamOffset[prog]);
    pSBLK = &pPPRM->splitBlock[index];
    pSPRM = &pSMPL->smplParam[pSBLK->sampleIndex];
    pVPRM = &pVAGI->vagiParam[pSPRM->vagiIndex];

    if (!(pSBLK->lowKey > note) && !(note > pSBLK->highKey)) {
        pHDPA->pPprm = pPPRM;
        pHDPA->pSblk = pSBLK;
        pHDPA->pSprm = pSPRM;
        pHDPA->pVprm = pVPRM;
        return 1;
    }

    return -1;
}

/** @brief Calculate final SPU playback parameters by combining program, split, and sample layers. */
s32 CalcPhdParam(CSE_PHDP* pPHDP, CSE_PHDPADDR* pHDPA, u8 note, u32 SpuTopAddr) {
    s16 pan;

    pPHDP->vol = pHDPA->pPprm->vol;
    pPHDP->vol = ((pPHDP->vol * pHDPA->pSblk->vol) / PHD_VOL_NORMALIZE);
    pPHDP->vol = ((pPHDP->vol * pHDPA->pSprm->vol) / PHD_VOL_NORMALIZE);
    pan = pHDPA->pPprm->pan - PHD_PAN_CENTER;

    if (pan < PHD_PAN_MIN) {
        pan = PHD_PAN_MIN;
    } else if (pan > PHD_PAN_MAX) {
        pan = PHD_PAN_MAX;
    }

    pan += (pHDPA->pSblk->pan - PHD_PAN_CENTER);
    if (pan < PHD_PAN_MIN) {
        pan = PHD_PAN_MIN;
    } else if (pan > PHD_PAN_MAX) {
        pan = PHD_PAN_MAX;
    }

    pan += (pHDPA->pSprm->pan - PHD_PAN_CENTER);
    if (pan < PHD_PAN_MIN) {
        pan = PHD_PAN_MIN;
    } else if (pan > PHD_PAN_MAX) {
        pan = PHD_PAN_MAX;
    }

    pPHDP->pan = (pan + PHD_PAN_CENTER);
    pPHDP->pitch = pHDPA->pPprm->fine + pHDPA->pPprm->trans * CENTS_PER_SEMITONE;
    pPHDP->pitch += pHDPA->pSblk->fine + pHDPA->pSblk->trans * CENTS_PER_SEMITONE;
    pPHDP->pitch += pHDPA->pSprm->fine + pHDPA->pSprm->trans * CENTS_PER_SEMITONE;
    pPHDP->pitch += (note - pHDPA->pSprm->base) * CENTS_PER_SEMITONE;
    pPHDP->bendLow = pHDPA->pSblk->bendLow;
    pPHDP->bendHigh = pHDPA->pSblk->bendHigh;
    pPHDP->adsr1 = pHDPA->pSprm->ADSR1;
    pPHDP->adsr2 = pHDPA->pSprm->ADSR2;
    pPHDP->s_addr = SpuTopAddr + pHDPA->pVprm->vagOffset;
    pPHDP->freq = pHDPA->pVprm->sampleRate;
    return 0;
}
