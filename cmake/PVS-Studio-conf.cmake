if (ENABLE_PVS_STUDIO)
    message("- PVS Studio enabled in CMakeLists.txt")
    set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
    include(cmake/extra/PVS-Studio.cmake)
    foreach(TARGET ${ALL_TARGETS})
        pvs_studio_add_target(TARGET ${TARGET}.analyze ALL
                OUTPUT FORMAT errorfile
                ANALYZE ${TARGET}
                LOG target.err)
    endforeach()
endif ()
