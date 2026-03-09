BUILD:

$ cd build
$ make

TO-DO:

copy other structs


src/Game/color3rd.c:
-plContext commented out
-cseGetIdStoredBd commented
-cseSendBd2SpuWithId commented


src/Game/DEMO00.c:
-test flPs2State.DispWidth to SCREEN_WIDTH (and height equivalent)


EFFnn.c effect_nn_init(), test if just changing data types worked


src/Game/GD3rd.c:
-ADXF variables
-ADXF read

src/Game/init3rd.c:
-CP3toPS2DrawOn()
-CP3toPS2DrawOff()


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

src/psp/MemMan.c:
-verify if it handles memory correctly


src/Game/Sound3rd.c:
-it's sound, it has to be ported with care, specificaly adxt stuff
-BGM_Server()
