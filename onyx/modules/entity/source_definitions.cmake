set(TARGET_PUBLIC_PATH ${CMAKE_CURRENT_SOURCE_DIR}/public/onyx/entity)
set(TARGET_PRIVATE_PATH ${CMAKE_CURRENT_SOURCE_DIR}/private/onyx/entity)

set(TARGET_PUBLIC_SOURCES
	${TARGET_PUBLIC_PATH}/entity.h
	${TARGET_PUBLIC_PATH}/entity.hpp
	${TARGET_PUBLIC_PATH}/entitycomponentsystem.h
	${TARGET_PUBLIC_PATH}/entityregistry.h
	${TARGET_PUBLIC_PATH}/prefab.h
	${TARGET_PUBLIC_PATH}/prefabregistry.h
)

set(TARGET_PRIVATE_SOURCES
	${TARGET_PRIVATE_PATH}/entitycomponentsystem.cpp
    ${TARGET_PRIVATE_PATH}/entityregistry.cpp
	${TARGET_PRIVATE_PATH}/prefabregistry.cpp
)