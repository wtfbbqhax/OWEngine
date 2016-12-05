cmake_minimum_required(VERSION 3.6)

project(Tremulous C CXX)

set(CMAKE_CXX_FLAGS -std=c++1y)
set(CMAKE_CXX_FLAGS "-x objective-c++")


set(TOP_SRC "../..")

add_executable( owe
            CGMouseDeltaFix.h
            CGMouseDeltaFix.m
            CGPrivateAPI.h
            Q3Controller.h
            Q3Controller.mm
            macosx_display.h
            macosx_display.mm
            macosx_glimp.h
            macosx_glimp.mm
            macosx_glsmp_mutex.mm
            macosx_glsmp_null.mm
            macosx_glsmp_ports.mm
            macosx_input.mm
            macosx_local.h
            macosx_qgl.h
            macosx_sndcore.mm
            macosx_snddma.mm
            macosx_sys.mm
            macosx_timers.h
            macosx_timers.mm
            ${TOP_SRC}/client/client.h
            ${TOP_SRC}/qcommon/qcommon.h
            ${TOP_SRC}/qcommon/q_shared.h
            )

include_directories(
    ${TOP_SRC}/client
    )
