#ifndef SOUND_IDS_H
#define SOUND_IDS_H

#include "types.h"

typedef enum {
    SND_NONE = 0,
    SND_MENU_CURSOR = 96,
    SND_MENU_SELECT = 98,
    SND_DIR_CURSOR = 343,

    // BGM IDs based on extraction/reference
    SND_BGM_VS_WAIT = 51,
    SND_BGM_CHARACTER_SELECT = 57,

} SoundRequest;

typedef struct {
    SoundRequest logical_id;
    u16 engine_code;
    s16 ptix;
    s16 bank;
    s16 port;
} SoundLookupEntry;

const SoundLookupEntry* Get_Sound_Lookup(SoundRequest id);

#endif // SOUND_IDS_H
