# ====================================================================
# minet core main target
# ====================================================================

project(minet)

set(target_name ${PROJECT_NAME})

# --------------------------------------------------------------------
# Source files and build options
# --------------------------------------------------------------------

file(GLOB_RECURSE src_list CONFIGURE_DEPENDS "${PROJECT_SOURCE_DIR}/src/*.cpp")
add_library(${target_name} ${src_list})

# --------------------------------------------------------------------
# Target properties
# --------------------------------------------------------------------

target_include_directories(
    ${target_name}
    PUBLIC ${CMAKE_CURRENT_LIST_DIR}/include
    PRIVATE ${CMAKE_CURRENT_LIST_DIR}/src
)

if(MINET_ENABLE_ASSERTION)
    target_compile_definitions(${target_name} PUBLIC MINET_ENABLE_ASSERT)
else()
    message(WARNING "Assertion disabled for minet core")
endif()

if(MINET_MASTER_PROJECT)
    minet_enable_warnings(${target_name})
endif()

if(MINET_ASAN)
    target_compile_options(${target_name} PRIVATE -fsanitize=address,undefined -fno-omit-frame-pointer)
    target_link_options(${target_name} PUBLIC -fsanitize=address,undefined -static-libasan)
elseif(MINET_TSAN)
    target_compile_options(${target_name} PRIVATE -fsanitize=thread,undefined -fno-omit-frame-pointer)
    target_link_options(${target_name} PUBLIC -fsanitize=thread,undefined -static-libtsan)
endif()

# --------------------------------------------------------------------
# Emit library name
# --------------------------------------------------------------------
set(MINET_LIB ${target_name} PARENT_SCOPE)
