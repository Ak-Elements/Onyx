set(onyx_TARGET_PUBLIC_DEPENDENCIES
    onyx-core
    onyx-assets
    onyx-filesystem
    onyx-entity
    onyx-graphics
    onyx-input
    onyx-inputactions
    onyx-localization
    onyx-nodegraph
    onyx-ui
    onyx-volume 
)

#remove onyx modules that should not be there (e.g. onyx-volume)
set(onyx_TARGET_PRIVATE_DEPENDENCIES
    onyx-profiler
)
