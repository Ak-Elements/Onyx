set(TARGET_PUBLIC_PATH ${CMAKE_CURRENT_SOURCE_DIR}/public/onyx/gamecore)
set(TARGET_PRIVATE_PATH ${CMAKE_CURRENT_SOURCE_DIR}/private/onyx/gamecore)

set(TARGET_PUBLIC_SOURCES
	${TARGET_PUBLIC_PATH}/gamecore.h
	${TARGET_PUBLIC_PATH}/components/cameracomponent.h
	${TARGET_PUBLIC_PATH}/components/idcomponent.h
	${TARGET_PUBLIC_PATH}/components/namecomponent.h
	${TARGET_PUBLIC_PATH}/components/transformcomponent.h
	${TARGET_PUBLIC_PATH}/components/transientcomponent.h
	${TARGET_PUBLIC_PATH}/components/graphics/lightcomponents.h
	${TARGET_PUBLIC_PATH}/components/graphics/materialcomponent.h
	${TARGET_PUBLIC_PATH}/components/graphics/textcomponent.h
	${TARGET_PUBLIC_PATH}/rendertasks/depthprepassrendertask.h
	${TARGET_PUBLIC_PATH}/rendertasks/staticmeshrendertask.h
	${TARGET_PUBLIC_PATH}/rendertasks/textrendertask.h
	${TARGET_PUBLIC_PATH}/scene/sceneframedata.h
	${TARGET_PUBLIC_PATH}/scene/scene.h
	${TARGET_PUBLIC_PATH}/scene/scenesector.h
	${TARGET_PUBLIC_PATH}/scene/scenesectorstreamer.h
	${TARGET_PUBLIC_PATH}/serialize/sceneserializer.h
	${TARGET_PUBLIC_PATH}/serialize/sdffontserializer.h
    ${TARGET_PUBLIC_PATH}/systems/lightingsystem.h
)

set(TARGET_PRIVATE_SOURCES
	${TARGET_PRIVATE_PATH}/gamecore.cpp
	${TARGET_PRIVATE_PATH}/components/cameracomponent.cpp
	${TARGET_PRIVATE_PATH}/components/freecameracomponent.cpp
	${TARGET_PRIVATE_PATH}/components/idcomponent.cpp
	${TARGET_PRIVATE_PATH}/components/namecomponent.cpp
	${TARGET_PRIVATE_PATH}/components/transformcomponent.cpp
	${TARGET_PRIVATE_PATH}/components/graphics/lightcomponents.cpp
	${TARGET_PRIVATE_PATH}/components/graphics/materialcomponent.cpp
	${TARGET_PRIVATE_PATH}/components/graphics/textcomponent.cpp
	${TARGET_PRIVATE_PATH}/rendertasks/depthprepassrendertask.cpp
	${TARGET_PRIVATE_PATH}/rendertasks/staticmeshrendertask.cpp
	${TARGET_PRIVATE_PATH}/rendertasks/textrendertask.cpp
	${TARGET_PRIVATE_PATH}/scene/scene.cpp
	${TARGET_PRIVATE_PATH}/scene/scenesectorstreamer.cpp
	${TARGET_PRIVATE_PATH}/serialize/sceneserializer.cpp
	${TARGET_PRIVATE_PATH}/serialize/sdffontserializer.cpp
    ${TARGET_PRIVATE_PATH}/systems/lightingsystem.cpp
)