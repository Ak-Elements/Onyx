set(onyx_TARGET_PUBLIC_SOURCES
    platformcontext.h
    platformsystem.h
    window.h
    windowsettings.h
    linux/wayland/waylandplatformcontext.h
    linux/wayland/waylandwindow.h
    linux/wayland/waylandinput.h
    linux/wayland/waylandpointer.h
    linux/wayland/waylandkeyboard.h
    windows/windowsplatformcontext.h
    windows/windowswindow.h

)

set(onyx_TARGET_PRIVATE_SOURCES
    platformsystem.cpp
    linux/wayland/waylandplatformcontext.cpp
    linux/wayland/waylandwindow.cpp
    linux/wayland/waylandinput.cpp
    linux/wayland/waylandpointer.cpp
    linux/wayland/waylandkeyboard.cpp
    windows/windowsplatformcontext.cpp
    windows/windowswindow.cpp
)