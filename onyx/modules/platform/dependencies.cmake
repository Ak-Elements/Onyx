
set(onyx_TARGET_PUBLIC_DEPENDENCIES
	onyx-core
    onyx-input
)

set(onyx_TARGET_PRIVATE_DEPENDENCIES
    onyx-filesystem
)

if (UNIX)
    find_package(Wayland REQUIRED COMPONENTS Client Cursor)
    find_package(X11 REQUIRED COMPONENTS xkbcommon)
    if (Wayland_FOUND)
        list(APPEND onyx_TARGET_PRIVATE_DEPENDENCIES ${Wayland_LIBRARIES} wayland-xdg)
    endif()

    if (X11_xkbcommon_FOUND)
        message("Found")
        list(APPEND onyx_TARGET_PRIVATE_DEPENDENCIES X11::xkbcommon)
    endif()
endif()

target_compile_definitions(onyx-platform PUBLIC $<IF:$<BOOL:${Wayland_FOUND}>,ONYX_USE_WAYLAND=1,ONYX_USE_WAYLAND=0>)
