#include "psp/savesub.h"

#include <psputility.h>
#include <psputility_savedata.h>

#include "common/graphics.h"

#include <string.h>
#include <stdbool.h>


//for saving
#include "Game/WORK_SYS.h"
#include "Game/RANKING.h"
#include "Game/color3rd.h"

typedef struct {
    u32 version;
    u8 RTT_Enabled;
    u8 blit_filter;
    s16 render_mode;

    u8 Auto_Save;

    u8 SoundMode;
    u8 BGM_Level;
    u8 SE_Level;
    u8 BgmType;

    RANK_DATA Ranking[20];
    u8 Extra_Option;
    u8 PL_Color[2][20];
    _EXTRA_OPTION extra_option;

    u8 CRT_COLOR;
    _PAD_INFOR Pad_Infor[2];
} SaveData;

enum {
    SAVEMODE_L_MANUAL = 0
    , SAVEMODE_S_MANUAL
    , SAVEMODE_L_AUTO
    , SAVEMODE_S_AUTO
};

enum {
    SAVE_FILETYPE_DATA = 0
    , SAVE_FILETYPE_DIRECTIONS
    , SAVE_FILETYPE_REPLAY
};

extern const void* getIcon0(void);
extern const unsigned int getIcon0Len(void);

u16 fileType;
SceUtilitySavedataParam save_params;

volatile SaveData save_data;

void pspSaveLoad();
void saveScreenParms();
void saveSoundParms();
void saveGameProgress();
void saveColorCorrect();
void saveButtonMap();

void loadSoundParms();
void loadScreenParms();
void loadGameProgress();
void loadColorCorrect();
void loadButtonMap();

void SaveInit(s32 file_type, s32 save_mode) {
    s16 mode;

    switch(save_mode){
        case SAVEMODE_L_MANUAL:
            mode = PSP_UTILITY_SAVEDATA_AUTOLOAD;
            break;

        case SAVEMODE_S_MANUAL:
            mode = PSP_UTILITY_SAVEDATA_AUTOSAVE;
            break;

        case SAVEMODE_L_AUTO:
            mode = PSP_UTILITY_SAVEDATA_AUTOLOAD;
            break;

        case SAVEMODE_S_AUTO:
            mode = PSP_UTILITY_SAVEDATA_AUTOSAVE;
            break;
    }

    memset(&save_params, 0, sizeof(SceUtilitySavedataParam));
    save_params.base.size = sizeof(SceUtilitySavedataParam);
    save_params.base.language = PSP_SYSTEMPARAM_LANGUAGE_ENGLISH;
    save_params.base.buttonSwap = PSP_UTILITY_ACCEPT_CROSS;
    save_params.base.graphicsThread = 0x11;
    save_params.base.accessThread = 0x13;
    save_params.base.fontThread = 0x12;
    save_params.base.soundThread = 0x10;
    save_params.mode = mode;
    save_params.overwrite = 1;
    save_params.focus = PSP_UTILITY_SAVEDATA_FOCUS_LATEST; // Set initial focus to the newest file (for loading)

    strncpy(save_params.key, "demma", sizeof(save_params.key));
    strncpy(save_params.gameName, "SF33RD", sizeof(save_params.gameName));    // First part of the save name, game identifier name
    strncpy(save_params.saveName, "AUTO", sizeof(save_params.saveName));    // Second part of the save name, save identifier name
    strncpy(save_params.fileName, "SETTINGS.BIN", sizeof(save_params.fileName));    // name of the data file

    // Allocate buffers used to store various parts of the save data
    save_params.dataBuf = &save_data;
    save_params.dataBufSize = sizeof(SaveData);
    save_params.dataSize = sizeof(SaveData);

    // Set save data
    if (mode == PSP_UTILITY_SAVEDATA_AUTOSAVE) {
        strcpy(save_params.sfoParam.title, "Street Fighter 3 3rd Strike");
        strcpy(save_params.sfoParam.savedataTitle,"Save Data");
        strcpy(save_params.sfoParam.detail,"www.github.com/demmis98/3s-psp");
        save_params.sfoParam.parentalLevel = 1;

        // Set icon0
        save_params.icon0FileData.buf = (void*) getIcon0();
        save_params.icon0FileData.bufSize = getIcon0Len();
        save_params.icon0FileData.size = getIcon0Len();
        save_params.focus = PSP_UTILITY_SAVEDATA_FOCUS_FIRSTEMPTY; // If saving, set inital focus to the first empty slot
    }

    fileType = file_type;
}

s32 SaveMove() {

    if(save_params.mode != PSP_UTILITY_SAVEDATA_SAVE
        && save_params.mode != PSP_UTILITY_SAVEDATA_AUTOSAVE
        && save_params.mode != PSP_UTILITY_SAVEDATA_LISTSAVE
        && save_params.mode != PSP_UTILITY_SAVEDATA_LOAD
        && save_params.mode != PSP_UTILITY_SAVEDATA_AUTOLOAD
        && save_params.mode != PSP_UTILITY_SAVEDATA_LISTLOAD)
        return 0;

    if(save_params.mode == PSP_UTILITY_SAVEDATA_SAVE || save_params.mode == PSP_UTILITY_SAVEDATA_AUTOSAVE){
        save_data.Auto_Save = save_w[1].Auto_Save;
        Save_Game_Data();
        
        saveScreenParms();
        saveSoundParms();
        saveGameProgress();
        saveColorCorrect();
        saveButtonMap();
    }

    pspSaveLoad();

    if((save_params.mode == PSP_UTILITY_SAVEDATA_LOAD || save_params.mode == PSP_UTILITY_SAVEDATA_AUTOLOAD)
        && save_params.base.result == 0){

        switch(save_data.version){
        case 5:
            loadButtonMap();
        case 4:
            loadColorCorrect();
        case 3:
            loadGameProgress();
        case 2:
            loadSoundParms();
        case 1:
        case 0:
            loadScreenParms();
        }

        Copy_Save_w();
    }

    return 0;
}

void SaveMsg(u16 x, u16 y){
    if(save_params.mode == PSP_UTILITY_SAVEDATA_SAVE || save_params.mode == PSP_UTILITY_SAVEDATA_AUTOSAVE){
        SSPutStr(x, y, 6, "SAVING...");
    }
    else if(save_params.mode == PSP_UTILITY_SAVEDATA_LOAD || save_params.mode == PSP_UTILITY_SAVEDATA_AUTOLOAD){
        SSPutStr(x, y, 0, "LOADING...");
    }
}

void pspSaveLoad(){
    int mode;

    // DEMMA change this to avoid incompatibilities
    save_data.version = 5;

    if (sceUtilitySavedataInitStart(&save_params) < 0)
        return;
        //return 0;

    do {
        mode = sceUtilitySavedataGetStatus();

        switch(mode) {
            case PSP_UTILITY_DIALOG_VISIBLE:
                sceUtilitySavedataUpdate(1);
                break;

            case PSP_UTILITY_DIALOG_QUIT:
                sceUtilitySavedataShutdownStart();
                break;

            case PSP_UTILITY_DIALOG_NONE:
                return;
        }

        sceDisplayWaitVblankStart();

    } while (mode != PSP_UTILITY_DIALOG_FINISHED);
    //return (dialog.base.result == PSP_UTILITY_COMMON_RESULT_OK);
    return;
}

void saveScreenParms() {
    save_data.RTT_Enabled = RTT_Enabled;
    save_data.blit_filter = blit_filter;
    save_data.render_mode = render_mode;
}

void saveSoundParms() {
    save_data.SoundMode = sys_w.sound_mode;
    save_data.BGM_Level = save_w[1].BGM_Level;
    save_data.SE_Level = save_w[1].SE_Level;
    save_data.BgmType = sys_w.bgm_type;
}

void saveGameProgress() {
    int ix;
    for (ix = 0; ix < 20; ix++) {
        save_data.Ranking[ix] = Ranking_Data[ix];
    }

    for (ix = 0; ix < 20; ix++) {
        save_data.PL_Color[0][ix] = save_w[1].PL_Color[0][ix];
    }

    for (ix = 0; ix < 20; ix++) {
        save_data.PL_Color[1][ix] = save_w[1].PL_Color[1][ix];
    }

    save_data.Extra_Option = save_w[1].Extra_Option;
    save_data.extra_option = save_w[1].extra_option;
}

void saveColorCorrect() {
    save_data.CRT_COLOR = CRT_COLOR;
}

void saveButtonMap() {
    save_data.Pad_Infor[0] = save_w[1].Pad_Infor[0];
    save_data.Pad_Infor[1] = save_w[1].Pad_Infor[1];
}

void loadScreenParms() {
    RTT_Enabled = save_data.RTT_Enabled;
    blit_filter = save_data.blit_filter;
    render_mode = save_data.render_mode;
}

void loadSoundParms() {
    sys_w.sound_mode = save_data.SoundMode;
    save_w[1].BGM_Level = save_data.BGM_Level;
    save_w[1].SE_Level = save_data.SE_Level;
    sys_w.bgm_type = save_data.BgmType;
}


void loadGameProgress() {
    int ix, iy;
    for (ix = 0; ix < 20; ix++) {
        save_w[1].Ranking[ix] = save_data.Ranking[ix];
    }

    for(iy = 0; iy < 6; iy++){
        for (ix = 0; ix < 20; ix++) {
            save_w[iy].PL_Color[0][ix] = save_data.PL_Color[0][ix];
        }

        for (ix = 0; ix < 20; ix++) {
            save_w[iy].PL_Color[1][ix] = save_data.PL_Color[1][ix];
        }
    }

    save_w[1].Extra_Option = save_data.Extra_Option;
    save_w[1].extra_option = save_data.extra_option;
}

void loadColorCorrect() {
    CRT_COLOR = save_data.CRT_COLOR;
}

void loadButtonMap() {
    for(int i = 0; i < 6; i++){
        save_w[i].Pad_Infor[0] = save_data.Pad_Infor[0];
        save_w[i].Pad_Infor[1] = save_data.Pad_Infor[1];
    }
}

extern const u8 Player_Name_Pos_TBL[21][2];
void displayGameProgress(){
    int k;
    u8 *col = save_data.PL_Color[0];

    SSPutStr(1, 5, 0, "PROGRESS");
    for(k = 0; k < 20; k++){
        scfont_sqput(2, k + 7, *col ? 1 : 26, 1, Player_Name_Pos_TBL[k][0], Player_Name_Pos_TBL[k][1], 5, 1, 2);
        col++;
    }
}