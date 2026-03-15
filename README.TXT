BUILD:

$ cd build
$ make

TO-DO:

src/Game/GD3rd.c:
-forced Check_LDREQ_Clear() return 1

src/Game/sel_pl.c:
-commented anything involving Check_PL_Load() and others
-commented if (Check_Exit_Check() == 0 && Debug_w[24] == 0xFF)

src/Game/Next_CPU.c:
-comented out Check_PL_Load() on Wait_Load_Complete functions

src/Game/DC_Ghost.c:
-//sceGuDrawArray(GU_TRIANGLES, GU_COLOR_8888 | GU_VERTEX_32BITF | GU_TRANSFORM_2D, 6, 0, vertices);

src/Game/bg.c
-Bg_Texture_Load_EX() if fail Search_ramcnt_type(0x12) DEBUG_BG_DRAW_OFF and return

src/Game/MTRANS.c
-mlt_obj_trans_cp3_ext() return if fail

src/Game/Game.c
-Game2_3 commented out Game2_1 and forced G_no[2] = 1;
-Game2_1 forces Allow_a_battle_f = 1;

src/Game/Manage.c
-Game_Manage_2_3 commented out conditions

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
