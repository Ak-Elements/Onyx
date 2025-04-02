set(TARGET_PUBLIC_PATH ${CMAKE_CURRENT_SOURCE_DIR}/public/onyx/application)
set(TARGET_PRIVATE_PATH ${CMAKE_CURRENT_SOURCE_DIR}/private/onyx/application)

set(TARGET_PUBLIC_SOURCES
	${TARGET_PUBLIC_PATH}/application.h
	${TARGET_PUBLIC_PATH}/debug/gui/statusbaroverlay.h
	${TARGET_PUBLIC_PATH}/debug/gui/keyboardoverlay.h
	${TARGET_PUBLIC_PATH}/debug/gui/notificationloggersink.h
	${TARGET_PUBLIC_PATH}/debug/gui/fpsstatusbaritem.h
	${TARGET_PUBLIC_PATH}/graphics/meshsourceasset.h
	${TARGET_PUBLIC_PATH}/taskgraph/taskgraph.h
	${TARGET_PUBLIC_PATH}/taskgraph/taskgraphtask.h
)

set(TARGET_PRIVATE_SOURCES
	${TARGET_PRIVATE_PATH}/application.cpp
	${TARGET_PRIVATE_PATH}/debug/gui/statusbaroverlay.cpp
	${TARGET_PRIVATE_PATH}/debug/gui/keyboardoverlay.cpp
	${TARGET_PRIVATE_PATH}/debug/gui/notificationloggersink.cpp
	${TARGET_PRIVATE_PATH}/debug/gui/fpsstatusbaritem.cpp
	${TARGET_PRIVATE_PATH}/graphics/meshsourceasset.cpp
	${TARGET_PRIVATE_PATH}/platform/windows/main.cpp
	${TARGET_PRIVATE_PATH}/taskgraph/taskgraph.cpp
	${TARGET_PRIVATE_PATH}/taskgraph/taskgraphtask.cpp
)