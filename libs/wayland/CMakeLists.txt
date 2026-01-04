find_package(Wayland REQUIRED COMPONENTS Client Cursor)
find_package(WaylandProtocols REQUIRED)
find_package(WaylandScanner REQUIRED)

if (Wayland_FOUND)

    add_library(wayland-xdg STATIC ${wayland-xdg-sources})
    
    ecm_add_wayland_client_protocol(
        wayland-xdg
        PROTOCOL  ${WaylandProtocols_DATADIR}/stable/xdg-shell/xdg-shell.xml
        BASENAME xdg-shell
        PRIVATE_CODE
    )
    ecm_add_wayland_client_protocol(
        wayland-xdg
        PROTOCOL  ${WaylandProtocols_DATADIR}/unstable/xdg-decoration/xdg-decoration-unstable-v1.xml 
        BASENAME xdg-decoration
        PRIVATE_CODE
    )

    set_target_properties (wayland-xdg PROPERTIES FOLDER extern)

    target_include_directories(wayland-xdg PUBLIC
        $<BUILD_INTERFACE:${CMAKE_CURRENT_BINARY_DIR}>
        $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>
    )

    if (ONYX_ENABLE_INSTALL)
        include(GNUInstallDirs)
        install(
            TARGETS
                wayland-xdg
            EXPORT wayland-xdg-target
            INCLUDES DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}
            RUNTIME
                DESTINATION ${CMAKE_INSTALL_BINDIR}
                COMPONENT ${PROJECT_NAME}_RunTime
            LIBRARY
                DESTINATION ${CMAKE_INSTALL_LIBDIR}
                COMPONENT ${PROJECT_NAME}_RunTime
                NAMELINK_COMPONENT ${PROJECT_NAME}_Development
            ARCHIVE
                DESTINATION ${CMAKE_INSTALL_LIBDIR}
                COMPONENT ${PROJECT_NAME}_Development
        )

        install(EXPORT wayland-xdg-target
            FILE wayland-xdg-target.cmake
            NAMESPACE wayland::
            DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/${PROJECT_NAME}
            COMPONENT wayland_Development
        )

        # Export targets to build tree root
        export(TARGETS
            wayland-xdg
            NAMESPACE wayland::
            FILE ${CMAKE_BINARY_DIR}/wayland-xdg-target.cmake
        )
    endif()
endif()