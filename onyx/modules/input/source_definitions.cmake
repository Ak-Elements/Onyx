set(TARGET_PUBLIC_PATH ${CMAKE_CURRENT_SOURCE_DIR}/public/onyx/input)
set(TARGET_PRIVATE_PATH ${CMAKE_CURRENT_SOURCE_DIR}/private/onyx/input)

set(TARGET_PUBLIC_SOURCES
	${TARGET_PUBLIC_PATH}/inputmodule.h
	${TARGET_PUBLIC_PATH}/inputbindingsregistry.h
	${TARGET_PUBLIC_PATH}/inputbindingsregistry.h
	${TARGET_PUBLIC_PATH}/inputsystem.h
	${TARGET_PUBLIC_PATH}/inputaction.h
	${TARGET_PUBLIC_PATH}/inputactionsasset.h
	${TARGET_PUBLIC_PATH}/inputactionsmap.h
	${TARGET_PUBLIC_PATH}/inputactionsystem.h
	${TARGET_PUBLIC_PATH}/inputactionsserializer.h
	${TARGET_PUBLIC_PATH}/inputbinding.h
	${TARGET_PUBLIC_PATH}/inputtypes.h
	${TARGET_PUBLIC_PATH}/gamepad.h
	${TARGET_PUBLIC_PATH}/keycodes.h
	${TARGET_PUBLIC_PATH}/mouse.h
	${TARGET_PUBLIC_PATH}/platform/windows/windows_gamecontroller.h
	${TARGET_PUBLIC_PATH}/platform/windows/windows_keycodes.h
)

set(TARGET_PRIVATE_SOURCES
	${TARGET_PRIVATE_PATH}/inputmodule.cpp
	${TARGET_PRIVATE_PATH}/inputbindingsregistry.cpp
	${TARGET_PRIVATE_PATH}/inputaction.cpp
	${TARGET_PRIVATE_PATH}/inputactionsasset.cpp
	${TARGET_PRIVATE_PATH}/inputsystem.cpp
	${TARGET_PRIVATE_PATH}/inputactionsmap.cpp
	${TARGET_PRIVATE_PATH}/inputactionsystem.cpp
	${TARGET_PRIVATE_PATH}/inputactionsserializer.cpp
	${TARGET_PRIVATE_PATH}/inputbinding.cpp
	${TARGET_PRIVATE_PATH}/platform/windows/windows_gamecontroller.cpp
)