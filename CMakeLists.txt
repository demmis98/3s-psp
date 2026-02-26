cmake_minimum_required(VERSION 3.11)

project(3rd-strike)

file(GLOB pspC src/psp/*.c)
file(GLOB commonC src/inc/*.c)
file(GLOB gameC src/Game/*.c)
file(GLOB bin2obj src/bin2obj/.*c)

add_executable(${PROJECT_NAME}
	${pspC}
	${common}
    ${gameC}
    ${bin2obj}
)

target_include_directories(
    ${PROJECT_NAME} PRIVATE src
    ${PROJECT_NAME} PRIVATE include
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

# Create an EBOOT.PBP file
create_pbp_file(
    TARGET ${PROJECT_NAME}
    ICON_PATH NULL
    BACKGROUND_PATH NULL
    PREVIEW_PATH NULL
    TITLE ${PROJECT_NAME}
    VERSION 00.01
)
