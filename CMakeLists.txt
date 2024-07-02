cmake_minimum_required(VERSION 3.27)
project(antlr4-testrig-runner)

set(antlr4_testrig_runner_VERSION 1.0.4)

include(CMakePackageConfigHelpers)
include(GNUInstallDirs)

# This prevents a spurious warning about an unused CMake variable
# when we are using CMAKE_TOOLCHAIN_FILE for vcpkg.
message(STATUS "Toolchain file: ${CMAKE_TOOLCHAIN_FILE}")

#set(DEBUG_SHARE "C:/source/vcpkg/packages/antlr4-testrig-runner_x64-mingw-static/debug/share/antlr4-testrig-runner")
set(DEBUG_SHARE "C:/source/vcpkg/packages/antlr4-testrig-runner_x64-mingw-static/debug/share")
if(NOT EXISTS "${DEBUG_SHARE}")
    message(WARNING "'${DEBUG_SHARE}' does not exist.")
else()
    message(WARNING "'${DEBUG_SHARE}' DOES exist.")
endif()

add_executable(antlr4-testrig-runner test_rig_main.cpp)

function(DLF_example_function foo bar)
    message("DLF_example_function: ${foo}, ${bar}")
endfunction()

message(STATUS "Install step 1")

# Step 1 of making a project installable.
# (See file "INSTALL_TARGET_HOW_TO.md".)
target_include_directories(antlr4-testrig-runner PUBLIC
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/${CMAKE_INSTALL_INCLUDEDIR}>
        $<INSTALL_INTERFACE:include>
)

message(STATUS "Install step 2")

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

message(STATUS "Install step 3")

# Step 3 of making a project installable.
install(
    TARGETS antlr4-testrig-runner
    EXPORT antlr4-testrig-runner-targets
    #RUNTIME DESTINATION bin
    #LIBRARY DESTINATION lib
    #ARCHIVE DESTINATION lib
)

message(STATUS "Install step 4")

# Step 4 of making a project installable.
# (See also "antlr4-testrig-runner-config.cmake.in".)
#include(CMakePackageConfigHelpers)

message(STATUS "Configure package file")

configure_package_config_file(
        cmake/antlr4-testrig-runner-config.cmake.in
    "${CMAKE_CURRENT_BINARY_DIR}/antlr4-testrig-runner-config.cmake"
    INSTALL_DESTINATION
        "${CMAKE_INSTALL_DATADIR}/antlr4-testrig-runner"
)

message(STATUS "Write version file")

write_basic_package_version_file(
        ${CMAKE_CURRENT_BINARY_DIR}/antlr4-testrig-runner-config-version.cmake
        VERSION ${antlr4_testrig_runner_VERSION}
        COMPATIBILITY SameMajorVersion)

message(STATUS "Install files to DATADIR")

# Also part of Step 4
install(
        FILES "${CMAKE_CURRENT_BINARY_DIR}/antlr4-testrig-runner-config.cmake"
        DESTINATION "${CMAKE_INSTALL_DATADIR}/antlr4-testrig-runner"
)

message(STATUS "Install step 5")

# Step 5 of making a project installable.
install(
        EXPORT antlr4-testrig-runner-targets
        DESTINATION ${CMAKE_INSTALL_DATADIR}/antlr4-testrig-runner
        NAMESPACE antlr4-testrig-runner::
        #FILE antlr4-testrig-runner-targets.cmake
        #COMPONENT development
)