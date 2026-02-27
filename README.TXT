BUILD

$ cd build
$ make

TO-DO

modify structs:
-MTX
-TexturePoolUsed
-PatternState

copy other structs

modify src/Game/DC_Ghost.c:
-void njSetPaletteBankNumG(u32 globalIndex, s32 bank)
-void njSetPaletteMode(u32 mode)
-void njSetPaletteData(s32 offset, s32 count, void* data)
-s32 njReLoadTexturePartNumG(u32 gix, s8* srcAdrs, u32 ofs, u32 size)

continue fixing every file after DC_Ghost

include/Game/aboutspr.h:
-multitextures (idk what they do so they are commented)

src/Game/DEMO00.c:
-ppg functions
-mmDebWriteTag
-test flPs2State.DispWidth to SCREEN_WIDTH (and height equivalent)

src/Game/EFF02.c:
-fix sound_effect_request