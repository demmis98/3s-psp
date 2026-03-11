BUILD:

$ cd build
$ make

TO-DO:

src/Game/GD3rd.c:
-forced Check_LDREQ_Clear() return 1

src/Game/sel_pl.c:
-commented anything involving Check_PL_Load() and others
-commented if (Check_Exit_Check() == 0 && Debug_w[24] == 0xFF) {

src/Game/Next_CPU.c:
-comented out Check_PL_Load() on Wait_Load_Complete functions

src/Game/DC_Ghost.c:
-//sceGuDrawArray(GU_TRIANGLES, GU_COLOR_8888 | GU_VERTEX_32BITF | GU_TRANSFORM_2D, 6, 0, vertices);


src/Game/color3rd.c:
-plContext commented out
-cseGetIdStoredBd commented
-cseSendBd2SpuWithId commented

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


src/Game/Sound3rd.c:
-it's sound, it has to be ported with care, specificaly adxt stuff
-BGM_Server()
