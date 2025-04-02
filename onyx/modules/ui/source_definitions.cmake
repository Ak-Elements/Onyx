set(TARGET_PUBLIC_PATH ${CMAKE_CURRENT_SOURCE_DIR}/public/onyx/ui)
set(TARGET_PRIVATE_PATH ${CMAKE_CURRENT_SOURCE_DIR}/private/onyx/ui)

set(TARGET_PUBLIC_SOURCES
	${TARGET_PUBLIC_PATH}/uimodule.h
	${TARGET_PUBLIC_PATH}/imguinotify.h
    ${TARGET_PUBLIC_PATH}/imguistyle.h
    ${TARGET_PUBLIC_PATH}/imguisystem.h
    ${TARGET_PUBLIC_PATH}/imguiwindow.h
    ${TARGET_PUBLIC_PATH}/notificationsystem.h
    ${TARGET_PUBLIC_PATH}/propertygrid.h
    ${TARGET_PUBLIC_PATH}/widgets.h
    ${TARGET_PUBLIC_PATH}/controls/dockspace.h
    ${TARGET_PUBLIC_PATH}/controls/vectorcontrol.h
)

set(TARGET_PRIVATE_SOURCES
	${TARGET_PRIVATE_PATH}/uimodule.cpp
	${TARGET_PRIVATE_PATH}/imguisystem.cpp
    ${TARGET_PRIVATE_PATH}/imguiwindow.cpp
    ${TARGET_PRIVATE_PATH}/notificationsystem.cpp
    ${TARGET_PRIVATE_PATH}/propertygrid.cpp
    ${TARGET_PRIVATE_PATH}/widgets.cpp
    ${TARGET_PRIVATE_PATH}/controls/dockspace.cpp
    ${TARGET_PRIVATE_PATH}/controls/vectorcontrol.cpp
)