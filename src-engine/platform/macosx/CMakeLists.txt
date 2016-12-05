add_executable( owe
            CGMouseDeltaFix.h
            CGMouseDeltaFix.m
            CGPrivateAPI.h
            Q3Controller.h
            Q3Controller.m
            macosx_display.h
            macosx_display.m
            macosx_glimp.h
            macosx_glimp.m
            macosx_glsmp_mutex.m
            macosx_glsmp_null.m
            macosx_glsmp_ports.m
            macosx_input.m
            macosx_local.h
            macosx_qgl.h
            macosx_sndcore.m
            macosx_snddma.m
            macosx_sys.m
            macosx_timers.h
            macosx_timers.m
            )

include_directories(
    ../client/
    )
