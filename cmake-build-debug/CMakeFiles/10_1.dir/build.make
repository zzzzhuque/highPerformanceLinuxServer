# CMAKE generated file: DO NOT EDIT!
# Generated by "Unix Makefiles" Generator, CMake Version 3.5

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
CMAKE_SOURCE_DIR = /home/lht/.zhuque/highPerformanceLinuxServer

# The top-level build directory on which CMake was run.
CMAKE_BINARY_DIR = /home/lht/.zhuque/highPerformanceLinuxServer/cmake-build-debug

# Include any dependencies generated for this target.
include CMakeFiles/10_1.dir/depend.make

# Include the progress variables for this target.
include CMakeFiles/10_1.dir/progress.make

# Include the compile flags for this target's objects.
include CMakeFiles/10_1.dir/flags.make

CMakeFiles/10_1.dir/chapter10/10_1.cpp.o: CMakeFiles/10_1.dir/flags.make
CMakeFiles/10_1.dir/chapter10/10_1.cpp.o: ../chapter10/10_1.cpp
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/home/lht/.zhuque/highPerformanceLinuxServer/cmake-build-debug/CMakeFiles --progress-num=$(CMAKE_PROGRESS_1) "Building CXX object CMakeFiles/10_1.dir/chapter10/10_1.cpp.o"
	/usr/bin/c++   $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -o CMakeFiles/10_1.dir/chapter10/10_1.cpp.o -c /home/lht/.zhuque/highPerformanceLinuxServer/chapter10/10_1.cpp

CMakeFiles/10_1.dir/chapter10/10_1.cpp.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/10_1.dir/chapter10/10_1.cpp.i"
	/usr/bin/c++  $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -E /home/lht/.zhuque/highPerformanceLinuxServer/chapter10/10_1.cpp > CMakeFiles/10_1.dir/chapter10/10_1.cpp.i

CMakeFiles/10_1.dir/chapter10/10_1.cpp.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/10_1.dir/chapter10/10_1.cpp.s"
	/usr/bin/c++  $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -S /home/lht/.zhuque/highPerformanceLinuxServer/chapter10/10_1.cpp -o CMakeFiles/10_1.dir/chapter10/10_1.cpp.s

CMakeFiles/10_1.dir/chapter10/10_1.cpp.o.requires:

.PHONY : CMakeFiles/10_1.dir/chapter10/10_1.cpp.o.requires

CMakeFiles/10_1.dir/chapter10/10_1.cpp.o.provides: CMakeFiles/10_1.dir/chapter10/10_1.cpp.o.requires
	$(MAKE) -f CMakeFiles/10_1.dir/build.make CMakeFiles/10_1.dir/chapter10/10_1.cpp.o.provides.build
.PHONY : CMakeFiles/10_1.dir/chapter10/10_1.cpp.o.provides

CMakeFiles/10_1.dir/chapter10/10_1.cpp.o.provides.build: CMakeFiles/10_1.dir/chapter10/10_1.cpp.o


# Object files for target 10_1
10_1_OBJECTS = \
"CMakeFiles/10_1.dir/chapter10/10_1.cpp.o"

# External object files for target 10_1
10_1_EXTERNAL_OBJECTS =

10_1: CMakeFiles/10_1.dir/chapter10/10_1.cpp.o
10_1: CMakeFiles/10_1.dir/build.make
10_1: CMakeFiles/10_1.dir/link.txt
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --bold --progress-dir=/home/lht/.zhuque/highPerformanceLinuxServer/cmake-build-debug/CMakeFiles --progress-num=$(CMAKE_PROGRESS_2) "Linking CXX executable 10_1"
	$(CMAKE_COMMAND) -E cmake_link_script CMakeFiles/10_1.dir/link.txt --verbose=$(VERBOSE)

# Rule to build all files generated by this target.
CMakeFiles/10_1.dir/build: 10_1

.PHONY : CMakeFiles/10_1.dir/build

CMakeFiles/10_1.dir/requires: CMakeFiles/10_1.dir/chapter10/10_1.cpp.o.requires

.PHONY : CMakeFiles/10_1.dir/requires

CMakeFiles/10_1.dir/clean:
	$(CMAKE_COMMAND) -P CMakeFiles/10_1.dir/cmake_clean.cmake
.PHONY : CMakeFiles/10_1.dir/clean

CMakeFiles/10_1.dir/depend:
	cd /home/lht/.zhuque/highPerformanceLinuxServer/cmake-build-debug && $(CMAKE_COMMAND) -E cmake_depends "Unix Makefiles" /home/lht/.zhuque/highPerformanceLinuxServer /home/lht/.zhuque/highPerformanceLinuxServer /home/lht/.zhuque/highPerformanceLinuxServer/cmake-build-debug /home/lht/.zhuque/highPerformanceLinuxServer/cmake-build-debug /home/lht/.zhuque/highPerformanceLinuxServer/cmake-build-debug/CMakeFiles/10_1.dir/DependInfo.cmake --color=$(COLOR)
.PHONY : CMakeFiles/10_1.dir/depend
