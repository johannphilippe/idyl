# =============================================================================
# modules/CMakeLists.txt — idyl module build configuration
#
# Defines an INTERFACE library that provides:
#   • Include paths for module headers and the module framework
#   • Dependency on the core value types (core.hpp, span.hpp)
#
# To create a new module:
#   1. Add a header file in this directory (e.g. my_module.hpp)
#   2. Subclass idyl::module::base_module
#   3. Override name() and functions()
#   4. Register in idyl.cpp via registry.add(std::make_unique<my_module>())
#
# Modules are header-only — no .cpp files needed.
# =============================================================================

add_library(idyl_modules INTERFACE)

target_include_directories(idyl_modules INTERFACE
    ${CMAKE_CURRENT_SOURCE_DIR}/..     # access to include/, core/, utilities/
)
