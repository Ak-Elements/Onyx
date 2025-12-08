set(onyx_TARGET_PUBLIC_DEPENDENCIES
    onyx-core
    onyx-assets
    onyx-filesystem
    onyx-graphics
)
# maybe move the window out of onyx-graphics to avoid pulling the whole graphics module just for input handling 

set(onyx_TARGET_PRIVATE_DEPENDENCIES
    onyx-profiler
)