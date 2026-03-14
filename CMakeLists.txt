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
)

target_include_directories(
    ${PROJECT_NAME} PRIVATE src
    ${PROJECT_NAME} PRIVATE include
    ${PROJECT_NAME} PRIVATE include/Compress/zlib
)

target_link_libraries(${PROJECT_NAME} PRIVATE
    pspdebug
    pspdisplay
    pspge
    pspgu
    pspctrl
    
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

    -fno-strict-aliasing

    ${DISABLED_WARNINGS}
)


# Create an EBOOT.PBP file
create_pbp_file(
    TARGET ${PROJECT_NAME}
    ICON_PATH NULL
    BACKGROUND_PATH NULL
    PREVIEW_PATH NULL
    TITLE ${PROJECT_NAME}
    VERSION 00.01
)

