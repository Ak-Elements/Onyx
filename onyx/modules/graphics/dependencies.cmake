set(CMAKE_FOLDER_PREV, ${CMAKE_FOLDER})	

set(onyx_TARGET_PUBLIC_DEPENDENCIES
    onyx-assets
    onyx-nodegraph
    onyx-rhi
)

set(onyx_TARGET_PRIVATE_DEPENDENCIES
    onyx-core
    onyx-filesystem
    onyx-profiler
)

set(CMAKE_FOLDER ${CMAKE_FOLDER_PREV})