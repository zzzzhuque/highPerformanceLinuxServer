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
include CMakeFiles/client.dir/depend.make

# Include the progress variables for this target.
include CMakeFiles/client.dir/progress.make

# Include the compile flags for this target's objects.
include CMakeFiles/client.dir/flags.make

CMakeFiles/client.dir/chapter15_processpool/client.cpp.o: CMakeFiles/client.dir/flags.make
CMakeFiles/client.dir/chapter15_processpool/client.cpp.o: ../chapter15_processpool/client.cpp
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/home/lht/.zhuque/highPerformanceLinuxServer/cmake-build-debug/CMakeFiles --progress-num=$(CMAKE_PROGRESS_1) "Building CXX object CMakeFiles/client.dir/chapter15_processpool/client.cpp.o"
	/usr/bin/c++   $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -o CMakeFiles/client.dir/chapter15_processpool/client.cpp.o -c /home/lht/.zhuque/highPerformanceLinuxServer/chapter15_processpool/client.cpp

CMakeFiles/client.dir/chapter15_processpool/client.cpp.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/client.dir/chapter15_processpool/client.cpp.i"
	/usr/bin/c++  $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -E /home/lht/.zhuque/highPerformanceLinuxServer/chapter15_processpool/client.cpp > CMakeFiles/client.dir/chapter15_processpool/client.cpp.i

CMakeFiles/client.dir/chapter15_processpool/client.cpp.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/client.dir/chapter15_processpool/client.cpp.s"
	/usr/bin/c++  $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -S /home/lht/.zhuque/highPerformanceLinuxServer/chapter15_processpool/client.cpp -o CMakeFiles/client.dir/chapter15_processpool/client.cpp.s

CMakeFiles/client.dir/chapter15_processpool/client.cpp.o.requires:

.PHONY : CMakeFiles/client.dir/chapter15_processpool/client.cpp.o.requires

CMakeFiles/client.dir/chapter15_processpool/client.cpp.o.provides: CMakeFiles/client.dir/chapter15_processpool/client.cpp.o.requires
	$(MAKE) -f CMakeFiles/client.dir/build.make CMakeFiles/client.dir/chapter15_processpool/client.cpp.o.provides.build
.PHONY : CMakeFiles/client.dir/chapter15_processpool/client.cpp.o.provides

CMakeFiles/client.dir/chapter15_processpool/client.cpp.o.provides.build: CMakeFiles/client.dir/chapter15_processpool/client.cpp.o


# Object files for target client
client_OBJECTS = \
"CMakeFiles/client.dir/chapter15_processpool/client.cpp.o"

# External object files for target client
client_EXTERNAL_OBJECTS =

client: CMakeFiles/client.dir/chapter15_processpool/client.cpp.o
client: CMakeFiles/client.dir/build.make
client: CMakeFiles/client.dir/link.txt
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --bold --progress-dir=/home/lht/.zhuque/highPerformanceLinuxServer/cmake-build-debug/CMakeFiles --progress-num=$(CMAKE_PROGRESS_2) "Linking CXX executable client"
	$(CMAKE_COMMAND) -E cmake_link_script CMakeFiles/client.dir/link.txt --verbose=$(VERBOSE)

# Rule to build all files generated by this target.
CMakeFiles/client.dir/build: client

.PHONY : CMakeFiles/client.dir/build

CMakeFiles/client.dir/requires: CMakeFiles/client.dir/chapter15_processpool/client.cpp.o.requires

.PHONY : CMakeFiles/client.dir/requires

CMakeFiles/client.dir/clean:
	$(CMAKE_COMMAND) -P CMakeFiles/client.dir/cmake_clean.cmake
.PHONY : CMakeFiles/client.dir/clean

CMakeFiles/client.dir/depend:
	cd /home/lht/.zhuque/highPerformanceLinuxServer/cmake-build-debug && $(CMAKE_COMMAND) -E cmake_depends "Unix Makefiles" /home/lht/.zhuque/highPerformanceLinuxServer /home/lht/.zhuque/highPerformanceLinuxServer /home/lht/.zhuque/highPerformanceLinuxServer/cmake-build-debug /home/lht/.zhuque/highPerformanceLinuxServer/cmake-build-debug /home/lht/.zhuque/highPerformanceLinuxServer/cmake-build-debug/CMakeFiles/client.dir/DependInfo.cmake --color=$(COLOR)
.PHONY : CMakeFiles/client.dir/depend

