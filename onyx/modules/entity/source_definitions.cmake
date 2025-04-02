set(TARGET_PUBLIC_PATH ${CMAKE_CURRENT_SOURCE_DIR}/public/onyx/entity)
set(TARGET_PRIVATE_PATH ${CMAKE_CURRENT_SOURCE_DIR}/private/onyx/entity)

set(TARGET_PUBLIC_SOURCES
	${TARGET_PUBLIC_PATH}/entity.h
	${TARGET_PUBLIC_PATH}/entity.hpp
	${TARGET_PUBLIC_PATH}/entityregistry.h
	${TARGET_PUBLIC_PATH}/typeinfo.h
	${TARGET_PUBLIC_PATH}/prefab.h
	${TARGET_PUBLIC_PATH}/prefabregistry.h
	${TARGET_PUBLIC_PATH}/system.h
)

set(TARGET_PRIVATE_SOURCES
	${TARGET_PRIVATE_PATH}/prefabregistry.cpp
	${TARGET_PRIVATE_PATH}/system.cpp
)