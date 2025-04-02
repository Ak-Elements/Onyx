set(TARGET_PUBLIC_PATH ${CMAKE_CURRENT_SOURCE_DIR}/public)
set(TARGET_PRIVATE_PATH ${CMAKE_CURRENT_SOURCE_DIR}/private)

set(${CURRENT_TARGET}_pch ${TARGET_PUBLIC_PATH}/onyx/filesystem/onyx_filesystem_pch.h)

set(TARGET_PUBLIC_SOURCES
	${TARGET_PUBLIC_PATH}/onyx/filesystem/filedialog.h
	${TARGET_PUBLIC_PATH}/onyx/filesystem/filestream.h
	${TARGET_PUBLIC_PATH}/onyx/filesystem/filewatcher.h
	${TARGET_PUBLIC_PATH}/onyx/filesystem/imagefile.h
	${TARGET_PUBLIC_PATH}/onyx/filesystem/onyx_filesystem_pch.h
	${TARGET_PUBLIC_PATH}/onyx/filesystem/onyxfile.h
	${TARGET_PUBLIC_PATH}/onyx/filesystem/path.h
	
)

set(TARGET_PRIVATE_SOURCES
	${TARGET_PRIVATE_PATH}/onyx/filesystem/filedialog.cpp
	${TARGET_PRIVATE_PATH}/onyx/filesystem/filestream.cpp
	${TARGET_PRIVATE_PATH}/onyx/filesystem/filewatcher.cpp
	${TARGET_PRIVATE_PATH}/onyx/filesystem/imagefile.cpp
	${TARGET_PRIVATE_PATH}/onyx/filesystem/onyx_filesystem.cpp
	${TARGET_PRIVATE_PATH}/onyx/filesystem/onyxfile.cpp
	${TARGET_PRIVATE_PATH}/onyx/filesystem/path.cpp
)