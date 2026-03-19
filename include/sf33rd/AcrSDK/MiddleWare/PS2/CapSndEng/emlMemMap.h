/**
 * @file emlMemMap.h
 * @brief Memory map management API — PHD address storage and SPU bank forwarding.
 *
 * Part of the CapSndEng (Capcom Sound Engine) middleware layer.
 */

#ifndef EML_MEM_MAP_H
#define EML_MEM_MAP_H

#include "types.h"

#define PHDADDR_MAX 16

extern void* PhdAddr[PHDADDR_MAX];

s32 mlMemMapInit(void* pSpuMemMap);
u32 mlMemMapGetBankAddr(u32 bank);
s32 mlMemMapSetPhdAddr(u32 bank, void* addr);
void* mlMemMapGetPhdAddr(u32 bank);

#endif
