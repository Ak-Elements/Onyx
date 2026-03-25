
set(onyx_TARGET_PUBLIC_DEPENDENCIES
	onyx-core
    onyx-input
)

set(onyx_TARGET_PRIVATE_DEPENDENCIES
    onyx-filesystem
)

if (UNIX)
    set( ONYX_USE_WAYLAND OFF CACHE STRING "Use Wayland" )
    set( ONYX_USE_X11 OFF CACHE STRING "Use X11" )

    if(NOT ONYX_USE_X11 )
        find_package(Wayland REQUIRED COMPONENTS Client Cursor)
        if (Wayland_FOUND)
            list(APPEND onyx_TARGET_PRIVATE_DEPENDENCIES ${Wayland_LIBRARIES} wayland-xdg)
            set(ONYX_USE_WAYLAND ON )
        else()
            set(ONYX_USE_WAYLAND OFF )
        endif()
        
    endif()

    if (NOT Wayland_FOUND)
        find_package(X11 REQUIRED COMPONENTS xcb xcb_xkb xkbcommon_X11)
        if (X11_xcb_FOUND)
            list(APPEND onyx_TARGET_PRIVATE_DEPENDENCIES X11::xcb X11::xcb_xkb X11::xkbcommon_X11)
        else()
            set(ONYX_USE_X11 false)
        endif()
    endif()

    find_package(X11 REQUIRED COMPONENTS xkbcommon)
    if (X11_xkbcommon_FOUND)
        list(APPEND onyx_TARGET_PRIVATE_DEPENDENCIES X11::xkbcommon)
    endif()
endif()

target_compile_definitions(onyx-platform PUBLIC 
    $<IF:$<BOOL:${ONYX_USE_WAYLAND}>,ONYX_USE_WAYLAND=1,ONYX_USE_WAYLAND=0>
    $<IF:$<BOOL:${ONYX_USE_X11}>,ONYX_USE_X11=1,ONYX_USE_X11=0>)