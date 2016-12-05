
#
##  .o88b. db      d888888b d88888b d8b   db d888888b
## d8P  Y8 88        `88'   88'     888o  88 `~~88~~'
## 8P      88         88    88ooooo 88V8o 88    88
## 8b      88         88    88~~~~~ 88 V8o88    88
## Y8b  d8 88booo.   .88.   88.     88  V888    88
##  `Y88P' Y88888P Y888888P Y88888P VP   V8P    YP
#

find_package(CURL)
find_package(OpenGL)
find_package(OpenAL)

file(GLOB QCMMON_SRCS qcommon/*.[ch])
file(GLOB CLIENT_SRCS client/*.[ch])
file(GLOB SERVER_SRCS server/*.[ch])
file(GLOB BOTLIB_SRCS botlib/*.[ch])
file(GLOB RENDERER_SRCS renderer/*.[ch])
file(GLOB SPLINES_SRCS splines/*.[ch])
file(GLOB PLATFORM_SRCS splines/*.[ch])

add_executable( tremulous

    ${QCMMON_SRCS}
    ${CLIENT_SRCS}
    ${SERVER_SRCS}
    ${BOTLIB_SRCS}
    ${RENDERER_SRCS}
    ${SPLINES_SRCS}
    ${PLATFORM_SRCS}
    #    #
    #    cl_cgame.c
    #    cl_cin.c
    #    cl_console.c
    #    cl_input.c
    #    cl_keys.c
    #    cl_main.c
    #    cl_net_chan.c
    #    cl_parse.c
    #    cl_scrn.c
    #    cl_ui.c
    #    client.h
    #    keys.h
    #    snd_adpcm.c
    #    snd_dma.c
    #    snd_local.h
    #    snd_mem.c
    #    snd_mix.c
    #    snd_public.h
    #    snd_wavelet.c
    #    #
    #    ${QCMMON_SRCS}
    #    #
    #    ${PARENT_DIR}/server/sv_ccmds.c
    #    ${PARENT_DIR}/server/sv_client.c
    #    ${PARENT_DIR}/server/sv_game.c
    #    ${PARENT_DIR}/server/sv_init.c
    #    ${PARENT_DIR}/server/sv_main.c
    #    ${PARENT_DIR}/server/sv_net_chan.c
    #    ${PARENT_DIR}/server/sv_snapshot.c
    #    ${PARENT_DIR}/server/sv_world.c
    #    #
    #    ${PARENT_DIR}/sys/con_log.c
    #    ${PARENT_DIR}/sys/con_tty.c
    #    ${PARENT_DIR}/sys/sys_main.c
    #    ${PARENT_DIR}/sys/sys_unix.c
    #    ${APPLE_SOURCES}
    )

if(APPLE)
 # FIXME Prefixed with "lua" to prevent cmake from doing "-l-framework Cocoa"
 set(FRAMEWORKS "-framework Cocoa -framework Security -framework OpenAL -framework IOKit")
else(APPLE)
 if(UNIX)
  set(SYSLIBS dl rt)
 endif(UNIX)
endif(APPLE)

if(NOT USE_RENDERER_DLOPEN)
  if(USE_OPENGL1)
  set(RENDERER_LIBRARY renderergl1)
  endif(USE_OPENGL1)
  set(RENDERER_LIBRARY renderergl2)
endif(NOT USE_RENDERER_DLOPEN)

target_link_libraries(
    tremulous
    #
    lua
    nettle
    zlib
    ogg
    opus
    opusfile
    #
    script_api
    restclient
    #
    ${FRAMEWORKS}
    ${CURL_LIBRARIES}
    ${SDL2_LIBRARIES}
    ${OPENGL_LIBRARIES}
    ${OPENAL_LIBRARY}
    ${SYSLIBS}
    )

include_directories(
    ${PARENT_DIR}
    ${PARENT_DIR}/zlib
    ${PARENT_DIR}/restclient
    ${PARENT_DIR}/rapidjson
    ${PARENT_DIR}/SDL2/include
    ${PARENT_DIR}/jpeg-8c
    ${PARENT_DIR}/libogg-1.3.1/include
    ${PARENT_DIR}/lua-5.3.3/include
    ${PARENT_DIR}/sol
    ${PARENT_DIR}/nettle-3.3
    ${PARENT_DIR}/opus-1.1/include
    ${PARENT_DIR}/opusfile-0.5/include
    ${SDL2_INCLUDE_DIRS}
    ${OPENAL_INCLUDE_DIR}
    )

# TODO: Turn this into a macro 
if (USE_INTERNAL_SDL2)
add_custom_command(
    TARGET tremulous
    POST_BUILD COMMAND ${CMAKE_COMMAND}
    ARGS -E copy ${CMAKE_SOURCE_DIR}/src/SDL2/libs/Darwin/libSDL2-2.0.0.dylib ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/libSDL2-2.0.0.dylib
    )
endif(USE_INTERNAL_SDL2)

if (USE_RENDERER_DLOPEN)
    add_custom_command(
        TARGET tremulous
        POST_BUILD COMMAND ${CMAKE_COMMAND}
        ARGS -E copy ${CMAKE_BINARY_DIR}/src/renderergl1/librenderergl1${CMAKE_SHARED_LIBRARY_SUFFIX} ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/renderer_opengl1${CMAKE_SHARED_LIBRARY_SUFFIX}
        )

    add_custom_command(
        TARGET tremulous
        POST_BUILD COMMAND ${CMAKE_COMMAND}
        ARGS -E copy ${CMAKE_BINARY_DIR}/src/renderergl2/librenderergl2${CMAKE_SHARED_LIBRARY_SUFFIX} ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/renderer_opengl2${CMAKE_SHARED_LIBRARY_SUFFIX}
        )
endif(USE_RENDERER_DLOPEN)

add_dependencies(
    tremulous
    ui
    renderergl1
    renderergl2
    )
