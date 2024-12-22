# ====================================================================
# minet core main target
# ====================================================================

project(minet)

set(TARGET_NAME ${PROJECT_NAME})

# --------------------------------------------------------------------
# Source files and build options
# --------------------------------------------------------------------

file(GLOB_RECURSE SRC_LIST CONFIGURE_DEPENDS "${PROJECT_SOURCE_DIR}/src/*.cpp")
add_library(${TARGET_NAME} ${SRC_LIST})

# --------------------------------------------------------------------
# Target properties
# --------------------------------------------------------------------

if(MINET_ENABLE_ASSERTION)
    target_compile_definitions(${TARGET_NAME} PUBLIC MINET_ENABLE_ASSERT)
else()
    message(WARNING "Assertion disabled for minet core")
endif()

if(MINET_MASTER_PROJECT)
    minet_enable_warnings(${TARGET_NAME})
endif()

target_include_directories(
    ${TARGET_NAME}
    PUBLIC ${CMAKE_CURRENT_LIST_DIR}/include
    PRIVATE ${CMAKE_CURRENT_LIST_DIR}/src
)

# --------------------------------------------------------------------
# Emit library name
# --------------------------------------------------------------------
set(MINET_LIB ${TARGET_NAME} PARENT_SCOPE)
