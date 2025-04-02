set(TARGET_PUBLIC_PATH ${CMAKE_CURRENT_SOURCE_DIR}/public/onyx/nodegraph)
set(TARGET_PRIVATE_PATH ${CMAKE_CURRENT_SOURCE_DIR}/private/onyx/nodegraph)

set(TARGET_PUBLIC_SOURCES
	${TARGET_PUBLIC_PATH}/nodegraphmodule.h
	${TARGET_PUBLIC_PATH}/executioncontext.h
	${TARGET_PUBLIC_PATH}/graph.h
	${TARGET_PUBLIC_PATH}/graphrunner.h
	${TARGET_PUBLIC_PATH}/node.h
	${TARGET_PUBLIC_PATH}/nodefactory.h
	${TARGET_PUBLIC_PATH}/pin.h
	${TARGET_PUBLIC_PATH}/shadernodefactory.h
	${TARGET_PUBLIC_PATH}/nodegraphserializer.h
	${TARGET_PUBLIC_PATH}/nodes/math/arithmeticnodes.h
	${TARGET_PUBLIC_PATH}/nodes/math/geometricnodes.h
	${TARGET_PUBLIC_PATH}/nodes/math/vectornodes.h
	${TARGET_PUBLIC_PATH}/nodes/math/vectornodes_editor.h
)

set(TARGET_PRIVATE_SOURCES
	${TARGET_PRIVATE_PATH}/nodegraphmodule.cpp
	${TARGET_PRIVATE_PATH}/executioncontext.cpp
	${TARGET_PRIVATE_PATH}/graph.cpp
	${TARGET_PRIVATE_PATH}/graphrunner.cpp
	${TARGET_PRIVATE_PATH}/node.cpp
	${TARGET_PRIVATE_PATH}/nodefactory.cpp
	${TARGET_PRIVATE_PATH}/nodegraphserializer.cpp
)
