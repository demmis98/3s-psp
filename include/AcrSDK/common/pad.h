#ifndef COMMON_PAD_H
#define COMMON_PAD_H

#include "structs.h"
#include "types.h"

typedef enum SWKey {
    SWK_UP = 1 << 0,
    SWK_DOWN = 1 << 1,
    SWK_LEFT = 1 << 2,
    SWK_RIGHT = 1 << 3,
    SWK_WEST = 1 << 4,
    SWK_NORTH = 1 << 5,
    SWK_RIGHT_SHOULDER = 1 << 6,
    SWK_LEFT_SHOULDER = 1 << 7,
    SWK_SOUTH = 1 << 8,
    SWK_EAST = 1 << 9,
    SWK_RIGHT_TRIGGER = 1 << 10,
    SWK_LEFT_TRIGGER = 1 << 11,
    SWK_LEFT_STICK = 1 << 12,
    SWK_RIGHT_STICK = 1 << 13,
    SWK_START = 1 << 14,
    SWK_BACK = 1 << 15,
} SWKey;

#define SWK_DIRECTIONS (SWK_UP | SWK_DOWN | SWK_LEFT | SWK_RIGHT)
#define SWK_ATTACKS                                                                                                    \
    (SWK_WEST | SWK_NORTH | SWK_RIGHT_SHOULDER | SWK_LEFT_SHOULDER | SWK_SOUTH | SWK_EAST | SWK_RIGHT_TRIGGER |        \
     SWK_LEFT_TRIGGER)

#endif
