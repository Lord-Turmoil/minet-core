# ====================================================================
# Dependencies of minet core
# ====================================================================

set(vendors json mioc spdlog)
foreach(vendor ${vendors})
    add_subdirectory(${vendor})
endforeach()

# json has different target name
set(vendors nlohmann_json mioc spdlog)
foreach(vendor ${vendors})
    minet_disable_warnings(${vendor})
endforeach()

# emit vendors
set(MINET_VENDORS ${vendors} PARENT_SCOPE)
