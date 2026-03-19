/**
 * @file eflSpuMap.c
 * @brief SPU memory bank address mapping for the Capcom Sound Engine.
 *
 * Manages the SPU (Sound Processing Unit) memory layout by parsing a SPUMAP
 * data structure and computing per-bank addresses within SPU RAM. Supports
 * multiple pages of bank configurations.
 *
 * Part of the CapSndEng (Capcom Sound Engine) middleware layer.
 * Originally from the PS2 sound middleware.
 */

#include "sf33rd/AcrSDK/MiddleWare/PS2/CapSndEng/eflSpuMap.h"
#include "common.h"

#include <stdio.h>
#include <string.h>

#define SPU_TOP_ADDR 0x5020
#define SPU_RAM_LIMIT 0x1FFFFF

// bss

CURRMAP CurrMap;

// sbss

PSPUMAP* pSpuMap;
u32 CurrPage;
u32 SpuTopAddr;

/**
 * @brief Initialize the SPU memory map from a SPUMAP data block.
 *
 * Validates the "SPUMAPDT" magic tag, resets all bank addresses to the
 * SPU top address (0x5020), and applies the first page configuration.
 *
 * @param pMap Pointer to the SPUMAP data structure
 * @return 0 on success, -1 if the magic tag is invalid
 */
s32 flSpuMapInit(PSPUMAP* pMap) {
    u32 i;

    if (strncmp((char*)pMap, "SPUMAPDT", 8) != 0) {
        return -1;
    }

    pSpuMap = pMap;
    CurrPage = 0;
    SpuTopAddr = SPU_TOP_ADDR;

    for (i = 0; i < SPUBANK_MAX; i++) {
        CurrMap.BankAddr[i] = SPU_TOP_ADDR;
        CurrMap.BankSize[i] = 0;
    }

    flSpuMapChgPage(CurrPage);

    return 0;
}

/**
 * @brief Switch to a different SPU memory map page.
 *
 * Recalculates all bank addresses based on the selected page's bank sizes.
 * Banks are laid out sequentially starting from SpuTopAddr. Returns an error
 * if any bank would exceed the 2 MB SPU RAM limit (0x1FFFFF).
 *
 * @param page Page index to switch to
 * @return 0 on success, -1 on invalid page or address overflow
 */
s32 flSpuMapChgPage(u32 page) {
    s32 i;
    u32 addr;
    PSPUMAP_PAGE* pPage;

    if ((pSpuMap->Head.NumPages - 1) > page) {
        return -1;
    }

    CurrPage = page;
    pPage = &pSpuMap->Page[CurrPage];
    addr = SpuTopAddr;

    for (i = 0; i < SPUBANK_MAX; i++) {
        CurrMap.BankAddr[i] = addr;
        CurrMap.BankSize[i] = pPage->BankSize[i];

        if (CurrMap.BankSize[i] + CurrMap.BankAddr[i] > SPU_RAM_LIMIT) {
            printf("[EE]");
            printf("(ERR)");
            // "Address has changed\n"
            printf("アドレスが変\n");
            return -1;
        }

        addr += pPage->BankSize[i];
    }

    return 0;
}

/**
 * @brief Get the SPU RAM start address for a given bank.
 *
 * @param bank Bank index (0 to SPUBANK_MAX-1)
 * @return SPU address of the bank, or 0 if the bank index is out of range
 */
u32 flSpuMapGetBankAddr(u32 bank) {
    if (bank >= SPUBANK_MAX) {
        return 0;
    }

    return CurrMap.BankAddr[bank];
}
