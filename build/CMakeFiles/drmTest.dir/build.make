# CMAKE generated file: DO NOT EDIT!
# Generated by "Unix Makefiles" Generator, CMake Version 3.16

# Delete rule output on recipe failure.
.DELETE_ON_ERROR:


#=============================================================================
# Special targets provided by cmake.

# Disable implicit rules so canonical targets will work.
.SUFFIXES:


# Remove some rules from gmake that .SUFFIXES does not remove.
SUFFIXES =

.SUFFIXES: .hpux_make_needs_suffix_list


# Suppress display of executed commands.
$(VERBOSE).SILENT:


# A target that is always out of date.
cmake_force:

.PHONY : cmake_force

#=============================================================================
# Set environment variables for the build.

# The shell in which to execute make rules.
SHELL = /bin/sh

# The CMake executable.
CMAKE_COMMAND = /usr/bin/cmake

# The command to remove a file.
RM = /usr/bin/cmake -E remove -f

# Escaping for special characters.
EQUALS = =

# The top-level source directory on which CMake was run.
CMAKE_SOURCE_DIR = /home/firefly/mpp/live_client_mpp

# The top-level build directory on which CMake was run.
CMAKE_BINARY_DIR = /home/firefly/mpp/live_client_mpp/build

# Include any dependencies generated for this target.
include CMakeFiles/drmTest.dir/depend.make

# Include the progress variables for this target.
include CMakeFiles/drmTest.dir/progress.make

# Include the compile flags for this target's objects.
include CMakeFiles/drmTest.dir/flags.make

CMakeFiles/drmTest.dir/src/drm_test.cpp.o: CMakeFiles/drmTest.dir/flags.make
CMakeFiles/drmTest.dir/src/drm_test.cpp.o: ../src/drm_test.cpp
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/home/firefly/mpp/live_client_mpp/build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_1) "Building CXX object CMakeFiles/drmTest.dir/src/drm_test.cpp.o"
	g++  $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -o CMakeFiles/drmTest.dir/src/drm_test.cpp.o -c /home/firefly/mpp/live_client_mpp/src/drm_test.cpp

CMakeFiles/drmTest.dir/src/drm_test.cpp.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/drmTest.dir/src/drm_test.cpp.i"
	g++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -E /home/firefly/mpp/live_client_mpp/src/drm_test.cpp > CMakeFiles/drmTest.dir/src/drm_test.cpp.i

CMakeFiles/drmTest.dir/src/drm_test.cpp.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/drmTest.dir/src/drm_test.cpp.s"
	g++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -S /home/firefly/mpp/live_client_mpp/src/drm_test.cpp -o CMakeFiles/drmTest.dir/src/drm_test.cpp.s

# Object files for target drmTest
drmTest_OBJECTS = \
"CMakeFiles/drmTest.dir/src/drm_test.cpp.o"

# External object files for target drmTest
drmTest_EXTERNAL_OBJECTS =

drmTest: CMakeFiles/drmTest.dir/src/drm_test.cpp.o
drmTest: CMakeFiles/drmTest.dir/build.make
drmTest: CMakeFiles/drmTest.dir/link.txt
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --bold --progress-dir=/home/firefly/mpp/live_client_mpp/build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_2) "Linking CXX executable drmTest"
	$(CMAKE_COMMAND) -E cmake_link_script CMakeFiles/drmTest.dir/link.txt --verbose=$(VERBOSE)

# Rule to build all files generated by this target.
CMakeFiles/drmTest.dir/build: drmTest

.PHONY : CMakeFiles/drmTest.dir/build

CMakeFiles/drmTest.dir/clean:
	$(CMAKE_COMMAND) -P CMakeFiles/drmTest.dir/cmake_clean.cmake
.PHONY : CMakeFiles/drmTest.dir/clean

CMakeFiles/drmTest.dir/depend:
	cd /home/firefly/mpp/live_client_mpp/build && $(CMAKE_COMMAND) -E cmake_depends "Unix Makefiles" /home/firefly/mpp/live_client_mpp /home/firefly/mpp/live_client_mpp /home/firefly/mpp/live_client_mpp/build /home/firefly/mpp/live_client_mpp/build /home/firefly/mpp/live_client_mpp/build/CMakeFiles/drmTest.dir/DependInfo.cmake --color=$(COLOR)
.PHONY : CMakeFiles/drmTest.dir/depend

