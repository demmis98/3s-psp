/**
 * @file emlMemMap.c
 * @brief Memory map management for the Capcom Sound Engine.
 *
 * Wraps the SPU bank address system (eflSpuMap) and adds a per-bank PHD
 * (program header data) address table. Used to associate EE-side PHD data
 * pointers with SPU memory banks.
 *
 * Part of the CapSndEng (Capcom Sound Engine) middleware layer.
 * Originally from the PS2 sound middleware.
 */

#include "sf33rd/AcrSDK/MiddleWare/PS2/CapSndEng/emlMemMap.h"
#include "common.h"
#include "sf33rd/AcrSDK/MiddleWare/PS2/CapSndEng/eflSpuMap.h"

// sbss

void* PhdAddr[PHDADDR_MAX];

/**
 * @brief Initialize the memory map system.
 *
 * Initializes the underlying SPU memory map and clears all PHD address slots.
 *
 * @param pSpuMemMap Pointer to the SPUMAP data block
 * @return 0 on success, negative on SPU map init failure
 */
s32 mlMemMapInit(void* pSpuMemMap) {
    s32 result;
    s32 i;

    result = flSpuMapInit((PSPUMAP*)pSpuMemMap);
    if (result < 0) {
        return result;
    }

    for (i = 0; i < PHDADDR_MAX; i++) {
        PhdAddr[i] = NULL;
    }

    return 0;
}

/**
 * @brief Get the SPU RAM address for a given bank.
 *
 * @param bank Bank index
 * @return SPU address of the bank
 */
u32 mlMemMapGetBankAddr(u32 bank) {
    return flSpuMapGetBankAddr(bank);
}

/**
 * @brief Store a PHD data pointer for a given bank.
 *
 * @param bank Bank index (0 to PHDADDR_MAX-1)
 * @param addr Pointer to the PHD data in EE memory
 * @return 0 on success, -1 if bank index is out of range
 */
s32 mlMemMapSetPhdAddr(u32 bank, void* addr) {
    if (bank >= PHDADDR_MAX) {
        return -1;
    }

    PhdAddr[bank] = addr;
    return 0;
}

/**
 * @brief Retrieve the PHD data pointer for a given bank.
 *
 * @param bank Bank index (0 to PHDADDR_MAX-1)
 * @return Pointer to PHD data, or NULL if bank index is out of range
 */
void* mlMemMapGetPhdAddr(u32 bank) {
    if (bank >= PHDADDR_MAX) {
        return NULL;
    }

    return PhdAddr[bank];
}
