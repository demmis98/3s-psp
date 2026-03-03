BUILD

$ cd build
$ make

TO-DO

modify structs:
-MTX
-TexturePoolUsed
-PatternState

copy other structs

include/Game/aboutspr.h:
-multitextures (idk what they do so they are commented)


src/Game/bg.c:
-ppg functions commented out


src/Game/color3rd.c:
-fl functions commented out
-plContext commented out
-cseGetIdStoredBd commented
-cseSendBd2SpuWithId commented


modify src/Game/DC_Ghost.c:
-void njSetPaletteBankNumG(u32 globalIndex, s32 bank)
-void njSetPaletteMode(u32 mode)
-void njSetPaletteData(s32 offset, s32 count, void* data)
-s32 njReLoadTexturePartNumG(u32 gix, s8* srcAdrs, u32 ofs, u32 size)


src/Game/DEMO00.c:
-ppg functions
-mmDebWriteTag
-test flPs2State.DispWidth to SCREEN_WIDTH (and height equivalent)


EFFnn.c effect_nn_init(), text if just changing data types worked


src/Game/GD3rd.c:
-ADXF variables
-ADXF read


src/Game/IOConv.c:
-flPADSetRepeatSw commented
-flpad_adr commented

src/Game/init3rd.c:
-CP3toPS2DrawOn()
-SaveMove()
-SaveInit(,)
-CP3toPS2DrawOff()


src/Game/main.c:
-size = flGetSpace(), i think it's to alocate ram
-mm, fl, ppg, zlib, seqs commented out
-mppMalloc commented out


-src/Game/menu.c:
-SaveMove()
-SaveInit(,)


src/Game/MTRANS.c:
-basically rewrite everything, specially anything involving MultiTexture
*it handles textures i think


src/Game/OPENING.c:
-ppg functions and variables

src/Game/op_sub.c:
-ppg functions and variables

src/Game/PulPul.c:
-i think it uses ps2 functions, on vibParamTrans

flpad i think is the gamepad, and a lot of calls to it were commented out


src/Game/RAMCNT.c:
-everything related to memory management


src/Game/sc_sub.c:
-ppg, ps2, fl commented out


src/Game/texcash.c:
-mts_ok commented out
-commented out basically everything


src/psp/MemMan.c:
-verify if it handles memory correctly


src/Game/Sound3rd.c:
-it's sound, it has to be ported with care, specificaly adxt stuff
-BGM_Server()


src/Game/EFFK2.c:
-fix sound_effect_request