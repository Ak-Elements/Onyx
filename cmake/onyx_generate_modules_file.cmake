function(onyx_generate_modules_file project_name)

    get_property(onyx_all_module_classes GLOBAL PROPERTY onyx_module_classes)
    get_property(onyx_all_module_include_paths GLOBAL PROPERTY onyx_module_include_paths)
    get_property(project_all_module_classes GLOBAL PROPERTY project_engine_module_classes)
    get_property(project_all_module_include_paths GLOBAL PROPERTY project_engine_module_include_paths)

    # Build include calls
    set(INCLUDE_ONYX_MODULES "")
    foreach(include_path IN LISTS onyx_all_module_include_paths)
        string(APPEND INCLUDE_ONYX_MODULES
    "#include <${include_path}>\n")
    endforeach()

    # Build include calls
    set(INCLUDE_PROJECT_MODULES "")
    foreach(include_path IN LISTS project_all_module_include_paths)
        string(APPEND INCLUDE_PROJECT_MODULES
    "#include <${include_path}>\n")
    endforeach()

    # Build register calls
    set(REGISTER_ONYX_MODULES "")
    foreach(module_class_name IN LISTS onyx_all_module_classes)
        string(APPEND REGISTER_ONYX_MODULES
    "        Onyx::Application::EngineModuleFactory::RegisterSystem<${module_class_name}>();\n")
    endforeach()

    # Build register calls
    set(REGISTER_PROJECT_MODULES "")
    foreach(module_class_name IN LISTS project_all_module_classes)
        string(APPEND REGISTER_PROJECT_MODULES
    "        Onyx::Application::EngineModuleFactory::RegisterSystem<${module_class_name}>();\n")
    endforeach()

    set(engine_modules_file_path "${CMAKE_CURRENT_BINARY_DIR}/${project_name}/generated/enginemodules.gen.cpp")
    configure_file(
    "${onyx_SOURCE_DIR}/cmake/onyx_engine_modules.cpp.in"
    "${engine_modules_file_path}"
    @ONLY
    )

    target_sources(${project_name} PRIVATE ${engine_modules_file_path})
endfunction()