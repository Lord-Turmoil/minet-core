# Extract version
function(minet_extract_version)
    file(READ "${CMAKE_CURRENT_LIST_DIR}/minet/include/minet/version.h" file_contents)

    string(REGEX MATCH "MINET_VERSION_MAJOR ([0-9]+)" _ "${file_contents}")
    if(NOT CMAKE_MATCH_COUNT EQUAL 1)
        message(FATAL_ERROR "Could not extract major version number")
    endif()
    set(MAJOR_VERSION ${CMAKE_MATCH_1})

    string(REGEX MATCH "MINET_VERSION_MINOR ([0-9]+)" _ "${file_contents}")
    if(NOT CMAKE_MATCH_COUNT EQUAL 1)
        message(FATAL_ERROR "Could not extract minor version number")
    endif()
    set(MINOR_VERSION ${CMAKE_MATCH_1})
    
    string(REGEX MATCH "MINET_VERSION_PATCH ([0-9]+)" _ "${file_contents}")
    if(NOT CMAKE_MATCH_COUNT EQUAL 1)
        message(FATAL_ERROR "Could not extract patch version number")
    endif()
    set(PATCH_VERSION ${CMAKE_MATCH_1})

    set(MINET_VERSION_MAJOR ${MAJOR_VERSION} PARENT_SCOPE)
    set(MINET_VERSION_MINOR ${MINOR_VERSION} PARENT_SCOPE)
    set(MINET_VERSION_PATCH ${PATCH_VERSION} PARENT_SCOPE)
    set(MINET_VERSION "${MAJOR_VERSION}.${MINOR_VERSION}.${PATCH_VERSION}" PARENT_SCOPE)
endfunction()

# Turn on warnings on the given target
function(minet_enable_warnings target_name)
    target_compile_options(${target_name} PRIVATE -Wall -Wextra -Werror)
endfunction()

# Turn off warnings on the given target 
function(minet_disable_warnings target_name)
    get_target_property(type ${target_name} TYPE)
    if (NOT ${type} STREQUAL "INTERFACE_LIBRARY")
        target_compile_options(${target_name} PRIVATE -w)
    endif()
endfunction()
