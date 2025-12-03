if(NOT IS_DIRECTORY ${PROJECT_SOURCE_DIR})
    message(FATAL_ERROR "Please build using the outermost CMakeLists.txt file.")
endif()

option(ONYX_PROFILER_ENABLED "Enable profiler" OFF)

list(APPEND ONYX_PUBLIC_DEFINES
    $<IF:$<BOOL:${ONYX_PROFILER_ENABLED}>,ONYX_PROFILER_ENABLED=1,ONYX_PROFILER_ENABLED=0>
)
set(ONYX_PUBLIC_DEFINES ${ONYX_PUBLIC_DEFINES} PARENT_SCOPE)

onyx_add_target(onyx-profiler
    FOLDER ${ONYX_TARGETS_FOLDER}
    NAMESPACE "Onyx::Profiler"
)