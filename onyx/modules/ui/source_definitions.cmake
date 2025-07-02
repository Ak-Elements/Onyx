set(TARGET_PUBLIC_PATH ${CMAKE_CURRENT_SOURCE_DIR}/public/onyx/ui)
set(TARGET_PRIVATE_PATH ${CMAKE_CURRENT_SOURCE_DIR}/private/onyx/ui)

set(TARGET_PUBLIC_SOURCES
	${TARGET_PUBLIC_PATH}/uimodule.h
    ${TARGET_PUBLIC_PATH}/ui_localization.h
	${TARGET_PUBLIC_PATH}/imguinotify.h
    ${TARGET_PUBLIC_PATH}/imguisystem.h
    ${TARGET_PUBLIC_PATH}/imguiwindow.h
    ${TARGET_PUBLIC_PATH}/notificationsystem.h
    ${TARGET_PUBLIC_PATH}/propertygrid.h
    ${TARGET_PUBLIC_PATH}/scopedcolor.h
    ${TARGET_PUBLIC_PATH}/scopeddisable.h
    ${TARGET_PUBLIC_PATH}/scopedid.h
    ${TARGET_PUBLIC_PATH}/scopedindent.h
    ${TARGET_PUBLIC_PATH}/scopedstyle.h
    ${TARGET_PUBLIC_PATH}/widgets.h
    ${TARGET_PUBLIC_PATH}/controls/assetselector.h
    ${TARGET_PUBLIC_PATH}/controls/button.h
    ${TARGET_PUBLIC_PATH}/controls/combobox.h
    ${TARGET_PUBLIC_PATH}/controls/dockspace.h
    ${TARGET_PUBLIC_PATH}/controls/vectorcontrol.h
    ${TARGET_PUBLIC_PATH}/layout/imguilayout.h
)

set(TARGET_PRIVATE_SOURCES
	${TARGET_PRIVATE_PATH}/uimodule.cpp
    ${TARGET_PRIVATE_PATH}/ui_localization.cpp
	${TARGET_PRIVATE_PATH}/imguisystem.cpp
    ${TARGET_PRIVATE_PATH}/imguiwindow.cpp
    ${TARGET_PRIVATE_PATH}/notificationsystem.cpp
    ${TARGET_PRIVATE_PATH}/propertygrid.cpp
    ${TARGET_PRIVATE_PATH}/scopedcolor.cpp
    ${TARGET_PRIVATE_PATH}/scopeddisable.cpp
    ${TARGET_PRIVATE_PATH}/scopedid.cpp
    ${TARGET_PRIVATE_PATH}/scopedindent.cpp
    ${TARGET_PRIVATE_PATH}/scopedstyle.cpp
    ${TARGET_PRIVATE_PATH}/widgets.cpp
    ${TARGET_PRIVATE_PATH}/controls/assetselector.cpp
    ${TARGET_PRIVATE_PATH}/controls/button.cpp
    ${TARGET_PRIVATE_PATH}/controls/combobox.cpp
    ${TARGET_PRIVATE_PATH}/controls/dockspace.cpp
    ${TARGET_PRIVATE_PATH}/controls/vectorcontrol.cpp
    ${TARGET_PRIVATE_PATH}/layout/imguilayout.cpp
)