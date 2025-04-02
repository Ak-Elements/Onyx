set(TARGET_PUBLIC_PATH ${CMAKE_CURRENT_SOURCE_DIR}/public/onyx/assets)
set(TARGET_PRIVATE_PATH ${CMAKE_CURRENT_SOURCE_DIR}/private/onyx/assets)

set(TARGET_PUBLIC_SOURCES
	${TARGET_PUBLIC_PATH}/asset.h
	${TARGET_PUBLIC_PATH}/assethotreloadsystem.h
	${TARGET_PUBLIC_PATH}/assetloader.h
	${TARGET_PUBLIC_PATH}/assetloadrequest.h
	${TARGET_PUBLIC_PATH}/assetserializer.h
	${TARGET_PUBLIC_PATH}/assetsystem.h
)

set(TARGET_PRIVATE_SOURCES
	${TARGET_PRIVATE_PATH}/asset.cpp
	${TARGET_PRIVATE_PATH}/assethotreloadsystem.cpp
	${TARGET_PRIVATE_PATH}/assetloader.cpp
	${TARGET_PRIVATE_PATH}/assetloadrequest.cpp
	${TARGET_PRIVATE_PATH}/assetsystem.cpp
	
)
