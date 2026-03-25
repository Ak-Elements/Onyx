
set(onyx_TARGET_PUBLIC_DEPENDENCIES
	onyx-core
    onyx-input
)

set(onyx_TARGET_PRIVATE_DEPENDENCIES
    onyx-filesystem
)

if (UNIX)
    option(ONYX_USE_X11 "Use X11 instead of Wayland (default: OFF, falls back to Wayland if X11 not found)" OFF)

    if (ONYX_USE_X11)
        find_package(X11 COMPONENTS xcb xcb_xkb xkbcommon_X11)
        if (X11_xcb_FOUND)
            list(APPEND onyx_TARGET_PRIVATE_DEPENDENCIES X11::xcb X11::xcb_xkb X11::xkbcommon_X11)
            set(ONYX_USE_WAYLAND OFF CACHE BOOL "Use Wayland" FORCE)
        else()
            message(WARNING "X11 requested but xcb components not found, falling back to Wayland")
            set(ONYX_USE_X11 OFF CACHE BOOL "Use X11 instead of Wayland" FORCE)
        endif()
    endif()

    if (NOT ONYX_USE_X11)
        find_package(Wayland REQUIRED COMPONENTS Client Cursor)
        if (Wayland_FOUND)
            list(APPEND onyx_TARGET_PRIVATE_DEPENDENCIES ${Wayland_LIBRARIES} wayland-xdg)
            set(ONYX_USE_WAYLAND ON CACHE BOOL "Use Wayland" FORCE)
        else()
            message(FATAL_ERROR "Neither X11 (xcb) nor Wayland found — cannot build on this system")
        endif()
    endif()
endif()

target_compile_definitions(onyx-platform PUBLIC 
    $<IF:$<BOOL:${ONYX_USE_WAYLAND}>,ONYX_USE_WAYLAND=1,ONYX_USE_WAYLAND=0>
    $<IF:$<BOOL:${ONYX_USE_X11}>,ONYX_USE_X11=1,ONYX_USE_X11=0>)