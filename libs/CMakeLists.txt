# ====================================================================
# Dependencies of minet core
# ====================================================================

set(VENDOR_DIRS json mioc spdlog)

foreach(vendor ${VENDOR_DIRS})
    add_subdirectory(${vendor})
endforeach()

set(VENDORS nlohmann_json mioc spdlog)

set(MINET_VENDORS ${VENDORS} PARENT_SCOPE)
