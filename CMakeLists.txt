cmake_minimum_required(VERSION 3.11)

project(3rd-strike)

set(CMAKE_C_STANDARD 11)
set(CMAKE_C_STANDARD_REQUIRED ON)

file(GLOB pspC src/psp/*.c)
file(GLOB commonC src/common/*.c)
file(GLOB gameC src/Game/*.c)
file(GLOB debugC src/Game/debug/*.c)
file(GLOB message3rdC src/Game/Message3rd/C_USA/*.c)
file(GLOB bin2objC src/bin2obj/*.c)
file(GLOB acrCommonC src/AcrSDK/common/*.c)
file(GLOB compressC src/Compress/*.c)
file(GLOB Lz77C src/Compress/Lz77/*.c)
file(GLOB zlibC src/Compress/zlib/*.c)

file(GLOB portC src/port/*.c)
file(GLOB portSoundC src/port/sound/*.c)
file(GLOB gameSoundC src/Game/sound/*.c)
file(GLOB acrSndC src/sf33rd/AcrSDK/MiddleWare/PS2/CapSndEng/*.c)
file(GLOB cseDataC src/sf33rd/Source/PS2/cseDataFiles/*.c)

add_executable(${PROJECT_NAME}
	${pspC}
	${commonC}
    ${gameC}
    ${debugC}
    ${message3rdC}
    ${bin2objC}
    ${acrCommonC}
    ${compressC}
    ${Lz77C}
    ${zlibC}
    ${portC}
    ${portSoundC}
    ${gameSoundC}
    ${acrSndC}
    ${cseDataC}
)

set_source_files_properties(src/psp/icon0.c PROPERTIES COMPILE_FLAGS "-G0")

target_include_directories(
    ${PROJECT_NAME} PRIVATE src
    ${PROJECT_NAME} PRIVATE include
    ${PROJECT_NAME} PRIVATE include/Compress/zlib
    ${PROJECT_NAME} PRIVATE include/port/sound
    ${PROJECT_NAME} PRIVATE include/Game/sound
    ${PROJECT_NAME} PRIVATE include/sf33rd/AcrSDK/MiddleWare/PS2/CapSndEng
    ${PROJECT_NAME} PRIVATE include/sf33rd/Source/PS2/cseDataFiles
)

target_link_libraries(${PROJECT_NAME} PRIVATE
    pspdebug
    pspdisplay
    pspge
    pspgu
    pspctrl
    psppower
    
    pspaudio
    pspaudiolib
    pspatrac3
)

# from 3sx CMakeLists
set(DISABLED_WARNINGS
    -Wno-unused-but-set-variable
    -Wno-pointer-sign
    -Wno-parentheses
    -Wno-incompatible-pointer-types
    -Wno-discarded-qualifiers
    -Wno-format-overflow
    -Wno-comment
    -Wno-maybe-uninitialized
    -Wno-array-bounds
    -Wno-aggressive-loop-optimizations
    -Wno-stringop-overflow
    -Wno-restrict
    -Wno-stringop-truncation
    -Wno-unused-variable
    -Wno-unused-const-variable
    -Wno-strict-prototypes
    -Wno-old-style-definition
    -Wno-implicit-function-declaration
    -Wno-int-conversion
)

target_compile_options(${PROJECT_NAME}  PRIVATE
    -Wall
    -O3 -ffast-math

    -fno-strict-aliasing

    ${DISABLED_WARNINGS}
)


# Create an EBOOT.PBP file
create_pbp_file(
    TARGET ${PROJECT_NAME}
    ICON_PATH ${CMAKE_SOURCE_DIR}/ICON0.PNG
    BACKGROUND_PATH ${CMAKE_SOURCE_DIR}/PIC1.PNG
    MUSIC_PATH ${CMAKE_SOURCE_DIR}/57.at3
    PREVIEW_PATH NULL
    TITLE ${PROJECT_NAME}
    VERSION 01.01
)

