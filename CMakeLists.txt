cmake_minimum_required(VERSION 3.11)

project(tank-fun)

file(GLOB incC inc/*.c)
file(GLOB gameCpp game/*.cpp)
file(GLOB entityCpp entity/*.cpp)

add_executable(${PROJECT_NAME}
	main.cpp
	${incC}
	${gameCpp}
	${entityCpp}
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
