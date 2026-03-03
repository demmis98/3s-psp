#include "malloc.h"

#include "common.h"
#include "fl.h"
#include "psp/MemMan.h"

u32 mmInitialNumber;

void mmSystemInitialize() {
    mmInitialNumber = 0;
}

void mmHeapInitialize(_MEMMAN_OBJ* mmobj, u8* adrs, s32 size, s32 unit, s8* format) {
    mmobj->oriHead = adrs;
    mmobj->oriSize = size;
    mmobj->ownUnit = unit;
    mmobj->ownNumber = mmInitialNumber++;
    mmobj->memHead = (u8*)mmRoundUp(mmobj->ownUnit, (uintptr_t)adrs);
    mmobj->memSize = mmRoundOff(mmobj->ownUnit, (uintptr_t)adrs + size) - (uintptr_t)mmobj->memHead;
    mmobj->remainder = mmobj->memSize - (mmobj->ownUnit * 2);
    mmobj->remainderMin = mmobj->remainder;
    mmobj->cell_1st = (struct _MEMMAN_CELL*)mmobj->memHead;
    mmobj->cell_fin = (struct _MEMMAN_CELL*)((uintptr_t)&mmobj->memHead[mmobj->memSize] - mmobj->ownUnit);
    mmobj->cell_1st->prev = NULL;
    mmobj->cell_1st->next = mmobj->cell_fin;
    mmobj->cell_1st->size = mmobj->ownUnit;
    mmobj->cell_fin->prev = mmobj->cell_1st;
    mmobj->cell_fin->next = NULL;
    mmobj->cell_fin->size = mmobj->ownUnit;
}

uintptr_t mmRoundUp(s32 unit, uintptr_t num) {
    return ~(unit - 1) & (num + unit - 1);
}

uintptr_t mmRoundOff(s32 unit, uintptr_t num) {
    return num & ~(unit - 1);
}

void mmDebWriteTag(s8* tag) {
    // Do nothing
    flLogOut("mmDebWriteTag");
    flLogOut(tag);
}

ssize_t mmGetRemainder(_MEMMAN_OBJ* mmobj) {
    return mmobj->remainder;
}

ssize_t mmGetRemainderMin(_MEMMAN_OBJ* mmobj) {
    return mmobj->remainderMin;
}

u8* mmAlloc(_MEMMAN_OBJ* mmobj, ssize_t size, s32 flag) {
    struct _MEMMAN_CELL* cell = mmAllocSub(mmobj, size, flag);
    if (!cell)
        return NULL;

    // Return pointer **after header**
    return (u8*)cell + mmobj->ownUnit;
}

struct _MEMMAN_CELL* mmAllocSub(_MEMMAN_OBJ* mmobj, ssize_t size, s32 flag) {
    if (size <= 0)
        return NULL;

    // compute total size including header + alignment
    size_t totalSize = ALIGN16(size + mmobj->ownUnit);

    // Use memalign to satisfy 16-byte alignment (important on PSP for GU/VFPU)
    void* ptr = memalign(16, totalSize);
    if (!ptr)
        return NULL;

    // store size in a fake "header" for bookkeeping
    struct _MEMMAN_CELL* cell = (struct _MEMMAN_CELL*)ptr;
    cell->size = (s32)totalSize;

    return cell;
}

void mmFree(_MEMMAN_OBJ* mmobj, u8* adrs) {
    struct _MEMMAN_CELL* cell;

    if (adrs != NULL) {
        cell = (struct _MEMMAN_CELL*)((intptr_t)adrs - mmobj->ownUnit);
        mmobj->remainder += cell->size;
        cell->prev->next = cell->next;
        cell->next->prev = cell->prev;
    } else {
        return;
    }
}
