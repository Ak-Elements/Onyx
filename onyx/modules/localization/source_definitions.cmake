set(TARGET_PUBLIC_PATH ${CMAKE_CURRENT_SOURCE_DIR}/public/onyx/localization)
set(TARGET_PRIVATE_PATH ${CMAKE_CURRENT_SOURCE_DIR}/private/onyx/localization)

set(TARGET_PUBLIC_SOURCES
    ${TARGET_PUBLIC_PATH}/localizationbackend.h
    ${TARGET_PUBLIC_PATH}/localizationmodule.h
    ${TARGET_PUBLIC_PATH}/localizedstring.h
    ${TARGET_PUBLIC_PATH}/backends/gettextlocalizationbackend.h
)

set(TARGET_PRIVATE_SOURCES
    ${TARGET_PRIVATE_PATH}/localizationbackend.cpp
    ${TARGET_PRIVATE_PATH}/localizationmodule.cpp
    ${TARGET_PRIVATE_PATH}/localizedstring.cpp
    ${TARGET_PRIVATE_PATH}/backends/gettextlocalizationbackend.cpp
)
