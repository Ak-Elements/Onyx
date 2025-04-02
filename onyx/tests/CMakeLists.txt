if(NOT IS_DIRECTORY ${PROJECT_SOURCE_DIR})
    message(FATAL_ERROR "Please build using the outermost CMakeLists.txt file.")
endif()

set(CURRENT_TARGET onyx-test)
add_executable(${CURRENT_TARGET}
    ${CMAKE_CURRENT_LIST_DIR}/test.cpp
	${CMAKE_CURRENT_LIST_DIR}/randomnumbertuplegenerator.h
	${CMAKE_CURRENT_LIST_DIR}/test_directedacyclicgraph.cpp
	${CMAKE_CURRENT_LIST_DIR}/test_morton32.cpp
	${CMAKE_CURRENT_LIST_DIR}/test_morton64.cpp
	${CMAKE_CURRENT_LIST_DIR}/test_tree.cpp
	${CMAKE_CURRENT_LIST_DIR}/test_asynctask.cpp
	${CMAKE_CURRENT_LIST_DIR}/test_threading.cpp
	${CMAKE_CURRENT_LIST_DIR}/test_reference.cpp
	${CMAKE_CURRENT_LIST_DIR}/core/geometry/test_vector2.cpp
	${CMAKE_CURRENT_LIST_DIR}/core/geometry/test_vector3.cpp
	${CMAKE_CURRENT_LIST_DIR}/core/geometry/test_vector4.cpp
	
)

# set cxx standard and settings
set_target_properties(${CURRENT_TARGET} PROPERTIES
    CXX_STANDARD 20
    CXX_STANDARD_REQUIRED YES
    CXX_EXTENSIONS NO
	CXX_VISIBILITY_PRESET hidden
	VISIBILITY_INLINES_HIDDEN YES
	SOVERSION ${MAJOR_VERSION}
	VERSION ${LIB_VERSION}
)

# add compiler defines
target_compile_definitions(${CURRENT_TARGET} PUBLIC
	$<$<CONFIG:DEBUG>:IS_DEBUG>
	$<$<CONFIG:RELEASE>:IS_RELEASE>)

target_compile_definitions(${CURRENT_TARGET} PRIVATE
     $<$<CXX_COMPILER_ID:Clang>:IS_CLANG>
	 $<$<CXX_COMPILER_ID:AppleClang>:_IS_APPLE>
	 $<$<CXX_COMPILER_ID:GNU>:IS_GCC>
     $<$<CXX_COMPILER_ID:MSVC>:WIN32_LEAN_AND_MEAN NOMINMAX IS_VISUAL_STUDIO IS_WINDOWS>
)

# enable compiler options
target_compile_options(${CURRENT_TARGET} PRIVATE ${ONYX_COMPILE_OPTIONS})
#### Link Options ####
target_link_options(${CURRENT_TARGET} PRIVATE ${ONYX_LINK_OPTIONS})

#### Includes ####
target_include_directories(${CURRENT_TARGET} PRIVATE
        $<BUILD_INTERFACE: ${CMAKE_CURRENT_LIST_DIR}/>
        $<INSTALL_INTERFACE:source
)

set(CMAKE_FOLDER_PREV, ${CMAKE_FOLDER})
set(CMAKE_FOLDER extern/Catch2)
FetchContent_Declare(Catch2
	GIT_REPOSITORY https://github.com/catchorg/Catch2.git
	GIT_TAG v3.5.4
)

FetchContent_MakeAvailable(Catch2)

# reset cmake folder
set(CMAKE_FOLDER ${CMAKE_FOLDER_PREV})

list(APPEND CMAKE_MODULE_PATH ${catch2_SOURCE_DIR}/extras)

target_link_libraries(${CURRENT_TARGET}
	onyx-core
	onyx-volume
	Catch2::Catch2WithMain)

include(CTest)
include(Catch)
catch_discover_tests(${CURRENT_TARGET})

include(GNUInstallDirs)
install(TARGETS ${CURRENT_TARGET}
 RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
 COMPONENT ${CURRENT_TARGET}_RunTime
 LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
 COMPONENT ${CURRENT_TARGET}_RunTime
 NAMELINK_COMPONENT ${CURRENT_TARGET}_Development
 ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
 COMPONENT ${CURRENT_TARGET}_Development
)