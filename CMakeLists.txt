cmake_minimum_required(VERSION 3.27)
project(antlr4-testrig-runner)

# This prevents a spurious warning about an unused CMake variable
# when we are using CMAKE_TOOLCHAIN_FILE for vcpkg.
message(STATUS "Toolchain file: ${CMAKE_TOOLCHAIN_FILE}")

add_executable(antlr4-testrig-runner test_rig_main.cpp)

function(DLF_example_function foo bar)
    message("DLF_example_function: ${foo}, ${bar}")
endfunction()

# Step 1 of making a project installable.
# (See file "INSTALL_TARGET_HOW_TO.md".)
target_include_directories(antlr4-testrig-runner PUBLIC
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
        $<INSTALL_INTERFACE:include>
)

# Step 2 of making a project installable.
# (Not applicable for this project.)
#[[
set(FOO_HDRS
        include/foo/foo.h
)

add_library(foo STATIC
        ${FOO_SRCS}
        ${FOO_HDRS}
)

install(FILES ${FOO_HDRS} DESTINATION include/foo)
]]

# Step 3 of making a project installable.
install(TARGETS antlr4-testrig-runner
        EXPORT antlr4-testrig-runner-targets
        RUNTIME DESTINATION bin
        LIBRARY DESTINATION lib
        ARCHIVE DESTINATION lib)

# Step 4 of making a project installable.
# (See also "cmake/antlr4-testrig-runner-config.cmake.in".)
include(CMakePackageConfigHelpers)
configure_package_config_file(
        cmake/antlr4-testrig-runner-config.cmake.in
        "${CMAKE_CURRENT_BINARY_DIR}/antlr4-testrig-runner-config.cmake"
        INSTALL_DESTINATION "${CMAKE_INSTALL_DATADIR}/antlr4-testrig-runner/cmake"
)

# Also part of Step 4
install(
        FILES "${CMAKE_CURRENT_BINARY_DIR}/antlr4-testrig-runner-config.cmake"
        DESTINATION "${CMAKE_INSTALL_DATADIR}/antlr4-testrig-runner/cmake"
)

# Step 5 of making a project installable.
install(
        EXPORT antlr4-testrig-runner-targets
        DESTINATION lib/cmake/antlr4-testrig-runner
        NAMESPACE antlr4-testrig-runner::
        FILE antlr4-testrig-runner-targets.cmake
        COMPONENT development
)