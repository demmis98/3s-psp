BUILD:

$ cd build
$ make

TO-DO:

copy other structs


src/Game/color3rd.c:
-plContext commented out
-cseGetIdStoredBd commented
-cseSendBd2SpuWithId commented


modify src/Game/DC_Ghost.c:
-void njSetPaletteBankNumG(u32 globalIndex, s32 bank)
-void njSetPaletteMode(u32 mode)
-void njSetPaletteData(s32 offset, s32 count, void* data)
-s32 njReLoadTexturePartNumG(u32 gix, s8* srcAdrs, u32 ofs, u32 size)


src/Game/DEMO00.c:
-test flPs2State.DispWidth to SCREEN_WIDTH (and height equivalent)


EFFnn.c effect_nn_init(), test if just changing data types worked


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
