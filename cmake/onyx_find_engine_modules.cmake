function(get_fully_qualified_name lines target_line output_var)
    set(namespace_stack "")
    set(brace_depth 0)
    set(line_index 0)

    foreach(line IN LISTS lines)
        if(line_index GREATER_EQUAL target_line)
            break()
        endif()

        # Detect namespaces, regex matches 'namespace' + whitespace + namespace name(s) + '{'
        # Supports nested namespace e.g. namespace A::B {
        string(REGEX MATCHALL
            "namespace[ \t]+([a-zA-Z_][a-zA-Z0-9_:]*)[ \t]*\\{?"
            ns_matches "${line}")

        if(ns_matches)
            # Extract namespace full name (like A::B)
            string(REGEX REPLACE "namespace[ \t]+([a-zA-Z_][a-zA-Z0-9_:]*)[ \t]*\\.*" "\\1" ns_name "${line}")

            # Push all namespace parts onto the stack
            string(STRIP "${ns_name}" ns_name)
            list(APPEND namespace_stack "${ns_name}")
        endif()

        # Count { and } in this line
        string(LENGTH "${line}" line_length)

        math(EXPR i 0)
        while(i LESS line_length)
            string(SUBSTRING "${line}" ${i} 1 ch)
            if(ch STREQUAL "{")
                math(EXPR brace_depth "${brace_depth} + 1")
            elseif(ch STREQUAL "}")
                math(EXPR brace_depth "${brace_depth} - 1")
                list(LENGTH namespace_stack ns_len)
                if(ns_len GREATER brace_depth)
                    list(REMOVE_AT namespace_stack -1)
                endif()
            endif()
            math(EXPR i "${i} + 1")
        endwhile()

        math(EXPR line_index "${line_index} + 1")
    endforeach()

    list(JOIN namespace_stack "::" ns_path)
    set(${output_var} "${ns_path}" PARENT_SCOPE)
endfunction()

function(onyx_find_engine_modules public_files base_directory out_module_classes out_module_include_paths)

    set(ENGINE_CLASSES "")
    set(ENGINE_INCLUDES "")

    foreach(filepath IN LISTS public_files)
        file(READ "${filepath}" file_content)

        string(REGEX MATCH ":[^\\n]*public[^\\n]*(Onyx::)?IEngineSystem" contains_module "${file_content}")
        if(NOT contains_module)
            continue()
        endif()

        string(REPLACE "\r\n" "\n" file_content "${file_content}")
        string(REPLACE "\r" "\n" file_content "${file_content}")
        string(REPLACE "\n" ";" lines "${file_content}")

        set(line_number 0)
        foreach(line IN LISTS lines)
            string(REGEX MATCHALL "class[ \t]+([A-Za-z_][A-Za-z0-9_]*)[ \t]*:[^{;]*public[^{};\/}]*(Onyx::)?IEngineSystem" class_matches "${line}")
            if(class_matches)
                # Extract class name
                string(REGEX REPLACE "class[ \t]+([a-zA-Z_][a-zA-Z0-9_]*)[ \t]*:.*" "\\1" class_name "${line}")
                string(STRIP "${class_name}" class_name)
                # Get fully qualified namespace at this line
                get_fully_qualified_name("${lines}" ${line_number} namespace_name)

                if(namespace_name STREQUAL "")
                    set(fq_name "${class_name}")
                else()
                    set(fq_name "${namespace_name}::${class_name}")
                endif()

                cmake_path(RELATIVE_PATH filepath BASE_DIRECTORY ${base_directory} OUTPUT_VARIABLE relativeInclude)
                list(APPEND ENGINE_CLASSES "${fq_name}")
                list(APPEND ENGINE_INCLUDES "${relativeInclude}")
            endif()

            math(EXPR line_number "${line_number} + 1")
        endforeach()
    endforeach()

    # Deduplicate & sort results
    list(REMOVE_DUPLICATES ENGINE_CLASSES)
    list(REMOVE_DUPLICATES ENGINE_INCLUDES)

    # Export these variables
    set(${out_module_classes} "${ENGINE_CLASSES}" PARENT_SCOPE)
    set(${out_module_include_paths} "${ENGINE_INCLUDES}" PARENT_SCOPE)
endfunction()