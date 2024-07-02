# How to add Install target to CMake project

# Steps in Brief

1. Change `target_include_directories` to use generator expressions.
2. List the header files in a variable and pass that to `install(FILES ...)`.
3. Add `install(TARGETS ...)` for each target with exports and install destination.
4. Generate config files and pass that to `install(FILES ...)`.
5. Use `install(EXPORT ...)` to install exports created during step 3.

# Detailed Steps

## 1. Generator Expressions for Include Directories

### Use

```
target_include_directories(foo PUBLIC
		$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/${CMAKE_INSTALL_INCLUDEDIR}>
		$<INSTALL_INTERFACE:include>
)
```

### Note

Skipping this step may result in the CMake error, `Target "name" INTERFACE_INCLUDE_DIRECTORIES property contains path: "path" which is prefixed in the source directory.`  If you've already taken care of the root CMakeLists.txt, it may indicate that you need to repeat steps 1-5 for the CMakeLists.txt in an include'd subdirectory.

### Explanation

The include path may (and probably, should) differ between the build target and the install target.  By default the include directory property of the library would be exported exactly as it was for the build of the library, which is not the correct path for the user of the library.  Conversely it would not be correct to use the install directory for include during build, as we have to build the library first before we can install it.

We therefore use generator expressions to conditionally change the value of  the target include directories depending on whether it is a build target or an install target. Typically the project may already have a line like, `target_include_directories(foo PUBLIC ./include)`; that should be replaced with the format to use, above.  Or, add it if the project didn't already specify one.

## 2. Install the Header Files

### Use

```
set(FOO_HDRS
	include/foo/foo.h
)

add_library(foo STATIC
	${FOO_SRCS}
	${FOO_HDRS}
)

install(FILES ${FOO_HDRS} DESTINATION include/foo)
```

### Note

The "include/foo/..." directories named in `set(FOO_HDRS ...)` is the path within the project source code.  (This path should be edited to match however your source code is organized.)  The "include/foo" after the `DESTINATION` in the install command is the path within the prefix (installation) directory and may be different.

TODO: how does CMake handle these paths; does it concatenate the paths?  Do we need to take steps to avoid installing "include/foo/foo.h" into "include/foo/include/foo/foo.h"?  If `target_include_directories` is properly set can `FOO_HDRS` just list file names?

### Explanation

The project may or may not explicitly list the header files in a project sources variable or passed directly to `add_library`.  If the project doesn't already list headers in a separate variable (or if it mixes them together with .c/.cpp files in the same list), pull the header files out into a separate `FOO_HDRS` variable.

Then pass the `${FOO_HDRS}` variable to an `install(FILES ...)` command (see use, above).  The destination should normally be of the form `include/foo` so that it goes to the include directory of the install prefix and further subdivided by library name so as not to conflict with other libraries' headers.

(It is not strictly necessary to pass `${FOO_HDRS}` to `add_library(...)` although it may cause CMake to generate nicer project files in some circumstances.  It is also not strictly necessary to put the non-header source files into a `FOO_SRCS` variable; you list the sources directly to add_library or add_executable - as CLion is wont to do.  We just need the headers in a variable to pass to both `add_library` and to `install`.)

## 3. Install Targets

### Use
```
install(TARGETS foo bar baz
    EXPORT foo-targets
    RUNTIME DESTINATION bin
    LIBRARY DESTINATION lib
    ARCHIVE DESTINATION lib)
```

### Explanation

This step accomplishes three things.  It lists the targets which are to be installed, it creates an export, and sets the destination(s) to install the various targets.  More on "export" in steps 4 and 5.

(Do not be confused by the naming convention: "foo-targets" is not a target, it is an export, named "foo-targets" because it will look for the targets passed to this install command.)

## 4. Generate Package Config Files

### Use

In CMakeLists.txt:

```
include(CMakePackageConfigHelpers)
configure_package_config_file(
    cmake/foo-config.cmake.in
    "${CMAKE_CURRENT_BINARY_DIR}/foo-config.cmake"
    INSTALL_DESTINATION "${CMAKE_INSTALL_DATADIR}/foo/cmake"
)

install(
    FILES "${CMAKE_CURRENT_BINARY_DIR}/foo-config.cmake"
    DESTINATION "${CMAKE_INSTALL_DATADIR}/foo/cmake"
)
```

In new file, "cmake/foo-config.cmake.in":

```
@PACKAGE_INIT@

include(CMakeFindDependencyMacro)
find_dependency(list each dependency)

include("${CMAKE_CURRENT_LIST_DIR}/foo-targets.cmake")

check_required_components(foo)
```

### Explanation

We have to include a CMake feature in order to get the method for generating a package configuration file.  We also need to manually create the ".cmake.in" file to pass to this method.  Both of those things are demonstrated in "use", above.  Carefully replace each instance of "foo" with your library target name, and replace the arguments to "find_dependency" in the .cmake.in file with your actual dependencies.

This step will create a package config file (looked for when the user of library foo runs find_package).  The package config file also includes the export file (see steps 3 and 5).

## 5. Install Exports

### Use
```
install(
		EXPORT foo-targets
		DESTINATION lib/cmake/foo
		NAMESPACE foo::
		FILE foo-targets.cmake
		COMPONENT development
)
```

### Explanation

This step is what actually causes the foo-targets.cmake file for the foo-targets export to be installed.  (That file is referenced in an include in the package configuration file generated in step 4.)  It would seem to also be the command which results in putting namespaces on find_package-imported targets like, "foo::foo" and "foo::bar".

# References

https://www.f-ax.de/dev/2020/10/07/cmake-config-package.html

https://www.foonathan.net/2016/03/cmake-install/

TODO: I'm not sure if I'm correctly following the advice given here or not:
https://cmake.org/pipermail/cmake/2019-September/070014.html

## Changes:

+ Use ".../${CMAKE_INSTALL_INCLUDEDIR}" instead of ".../include"
+ Step 4 configure_package_config_file and "install(FILES" could come before step 3.
+ TODO: remove "/cmake" from end of install destination(s)
+ TODO: remove RUNTIME/LIBRARY/ARCHIVE from step 3
+ TODO: change destination in step 5 to use CMAKE_INSTALL_DATADIR
+ TODO: don't bother with cmake/ folder for config.in file
+ TODO: move include to step 0, include(CMakePackageConfigHelpers) and include(GNUInstallDirs)
