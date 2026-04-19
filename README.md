# Street Fighter III: 3rd Strike - PSP Port

A PSP port of Street Fighter III: 3rd Strike.
Based on the PS2 decompilation  https://github.com/crowded-street/3s-decomp
And the PC port https://github.com/crowded-street/3sx

## --How to use--

You need `SF33RD.AFS` from the PS2 disc/iso. Place it in a `resources/` folder next to `EBOOT.PBP`:

```
ms0:/PSP/GAME/3rd-strike/
    EBOOT.PBP
    resources/
        SF33RD.AFS
```


## --Controls--

- Start+Select: Soft reset (returns to title screen)
- Press Start at boot: Disable backgrounds (debug)
- Press Select at boot: play as Player 2

## --Settings--

### Screen Settings
#### Screen mode
    -"Stretch" for full screen
    -"Square" for pseudo 4:3
    -"Native" for 384x224 resolution
    -"Vertical" for native width, expanded height
    -"Extended" for experimental rendering
#### Filter
    -"Nearest" for nearest scaling
    -"Bilinear" for #define PSP_FATar scaling
#### Scaling mode
    -"Fast" for scaling each vertex
    -"Smooth" for scaling the whole canvas
#### Color correction
    -"On" to emulate a CRT's colors
    -"OFF" for original colors

### Sound Settings
- Choose between stereo or mono
- Change the volume for the music and the sound effects
- Choose either "Arranged" soundtrack or "Original"


## --Tecnical Features--

### Rendering
- Native CPS3 resolution (384x224) centered on PSP screen (480x272)
- Per-vertex offset scaling with scissor clipping for clean pixel rendering
- No sub-pixel seams or texture atlas bleeding
- Shadow transparency via GU_TFX_MODULATE

### Audio - Complete Pipeline
- **BGM**: CRI ADX ADPCM decoder with 48kHz to 44.1kHz resampling
- **SFX**: PS2 SPU2 emulator (48 voices, VAG/ADPCM, ADSR envelopes)
- **Character voices**: Per-character BD sound banks loaded from AFS
- **VS screen jingle**: In-memory ADX playback (ADX_StartMem)
- **Seamless BGM**: Gapless segment transitions with preload system
- **Capcom Sound Engine (CSE)**: Full PHD/TSB bank system for SE dispatch

### I/O
- AFS archive reader with background I/O thread
- Async file reads for asset loading (LDREQ queue)
- Sync reads for audio (gapless transitions)
- Single AFS file handle design

### Performance
- 333MHz CPU clock on "Smooth" rendering, 266MHz on "Fast" rendering
- SPU emulator: MAX_ACTIVE_VOICES=10 cap, linear interpolation, unrolled ADPCM decode
- -O3 -ffast-math compiler optimization
- Graceful error recovery (no freeze traps)

### Save System
- Screen settings and sound setting can be saved using the PSP's native api, and it also saves the arcade progress (rankings and arcade completion) which unlocks extra colors (holding start when selecting a character)

## --Building--

Requires [pspdev](https://github.com/pspdev/pspdev) toolchain (tested via WSL Debian).

```bash
mkdir build_wsl && cd build_wsl
psp-cmake ..
make -j4
```

## Known Issues

- Seamless BGM segment transitions have a minor audible click on some tracks
- White noise burst possible on SPU voices with invalid start addresses (mitigated with address validation)

## TODO / Stubs

### Audio Polish
- ADX loading is fully synchronous — could benefit from async with double-buffered player state
- Seamless preload system could use a dedicated I/O fd to avoid LDREQ conflicts

### Rendering
- MTRANS error traps converted to early returns — some texture groups may not load in time on first frame
- `ppgPurgeTextureFromVRAM` / `ppgPurgePaletteFromVRAM` — PS2 VRAM ops disabled, PSP equivalent not needed

### General
- `fatal_error()`, `not_implemented()`, `debug_print()` — empty logging functions
- `tarPADDestroy()` — PAD cleanup stub
- `flMemset()` / `flMemcpy()` — manual loops, should use stdlib

### Optimize
- MTRANS.c mlt_obj functions

## --Credits--
- demmis98
- gibletto
- Kinu Nishimura (banner artwork)


## --Thanks--
thanks to everyone following this proyect
i honestly never thought it could run this well on real hardware, and even have sound
thanks to everyone who downloaded it and tried it on their psp, it makes me so happy to see people enjoying this port
this code has taken a new life beyond what i had envisioned, and i'm happy for that
if you have any comment, suggestion or anything let us know
-demma (demmis98)
