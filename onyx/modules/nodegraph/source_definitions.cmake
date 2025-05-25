set(TARGET_PUBLIC_PATH ${CMAKE_CURRENT_SOURCE_DIR}/public/onyx/nodegraph)
set(TARGET_PRIVATE_PATH ${CMAKE_CURRENT_SOURCE_DIR}/private/onyx/nodegraph)

set(TARGET_PUBLIC_SOURCES
    ${TARGET_PUBLIC_PATH}/executioncontext.h
    ${TARGET_PUBLIC_PATH}/graph.h
    ${TARGET_PUBLIC_PATH}/graphrunner.h
    ${TARGET_PUBLIC_PATH}/nodegraphfactory.h
    ${TARGET_PUBLIC_PATH}/nodegraphmodule.h
    ${TARGET_PUBLIC_PATH}/nodegraphserializer.h
    ${TARGET_PUBLIC_PATH}/nodes/fixedpinnode1in.h
    ${TARGET_PUBLIC_PATH}/nodes/fixedpinnode1in1out.h
    ${TARGET_PUBLIC_PATH}/nodes/fixedpinnode1in2out.h
    ${TARGET_PUBLIC_PATH}/nodes/fixedpinnode1in3out.h
    ${TARGET_PUBLIC_PATH}/nodes/fixedpinnode1in4out.h
    ${TARGET_PUBLIC_PATH}/nodes/fixedpinnode1out.h
    ${TARGET_PUBLIC_PATH}/nodes/fixedpinnode2in1out.h
    ${TARGET_PUBLIC_PATH}/nodes/fixedpinnode3in1out.h
    ${TARGET_PUBLIC_PATH}/nodes/fixedpinnode4in1out.h
    ${TARGET_PUBLIC_PATH}/nodes/fixedpinsnode.h
    ${TARGET_PUBLIC_PATH}/nodes/flexiblepinsnode.h
    ${TARGET_PUBLIC_PATH}/nodes/math/arithmeticnodes.h
    ${TARGET_PUBLIC_PATH}/nodes/math/geometricnodes.h
    ${TARGET_PUBLIC_PATH}/nodes/math/vectornodes_editor.h
    ${TARGET_PUBLIC_PATH}/nodes/math/vectornodes.h
    ${TARGET_PUBLIC_PATH}/nodes/node.h
    ${TARGET_PUBLIC_PATH}/pins/dynamicpin.h
    ${TARGET_PUBLIC_PATH}/pins/pin.h
    ${TARGET_PUBLIC_PATH}/pins/pin.h
    ${TARGET_PUBLIC_PATH}/pins/pinbase.h
    ${TARGET_PUBLIC_PATH}/pins/pinmeta.h
    ${TARGET_PUBLIC_PATH}/pins/pinmeta.hpp
)

set(TARGET_PRIVATE_SOURCES
    ${TARGET_PRIVATE_PATH}/executioncontext.cpp
    ${TARGET_PRIVATE_PATH}/graph.cpp
    ${TARGET_PRIVATE_PATH}/graphrunner.cpp
    ${TARGET_PRIVATE_PATH}/nodegraphfactory.cpp
    ${TARGET_PRIVATE_PATH}/nodegraphmodule.cpp
    ${TARGET_PRIVATE_PATH}/nodegraphserializer.cpp
    ${TARGET_PRIVATE_PATH}/nodes/node.cpp
    ${TARGET_PRIVATE_PATH}/pins/dynamicpin.cpp
)
