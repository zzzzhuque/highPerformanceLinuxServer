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
include CMakeFiles/syslog.dir/depend.make

# Include the progress variables for this target.
include CMakeFiles/syslog.dir/progress.make

# Include the compile flags for this target's objects.
include CMakeFiles/syslog.dir/flags.make

CMakeFiles/syslog.dir/chapter07/syslog.cpp.o: CMakeFiles/syslog.dir/flags.make
CMakeFiles/syslog.dir/chapter07/syslog.cpp.o: ../chapter07/syslog.cpp
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/home/lht/.zhuque/highPerformanceLinuxServer/cmake-build-debug/CMakeFiles --progress-num=$(CMAKE_PROGRESS_1) "Building CXX object CMakeFiles/syslog.dir/chapter07/syslog.cpp.o"
	/usr/bin/c++   $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -o CMakeFiles/syslog.dir/chapter07/syslog.cpp.o -c /home/lht/.zhuque/highPerformanceLinuxServer/chapter07/syslog.cpp

CMakeFiles/syslog.dir/chapter07/syslog.cpp.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/syslog.dir/chapter07/syslog.cpp.i"
	/usr/bin/c++  $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -E /home/lht/.zhuque/highPerformanceLinuxServer/chapter07/syslog.cpp > CMakeFiles/syslog.dir/chapter07/syslog.cpp.i

CMakeFiles/syslog.dir/chapter07/syslog.cpp.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/syslog.dir/chapter07/syslog.cpp.s"
	/usr/bin/c++  $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -S /home/lht/.zhuque/highPerformanceLinuxServer/chapter07/syslog.cpp -o CMakeFiles/syslog.dir/chapter07/syslog.cpp.s

CMakeFiles/syslog.dir/chapter07/syslog.cpp.o.requires:

.PHONY : CMakeFiles/syslog.dir/chapter07/syslog.cpp.o.requires

CMakeFiles/syslog.dir/chapter07/syslog.cpp.o.provides: CMakeFiles/syslog.dir/chapter07/syslog.cpp.o.requires
	$(MAKE) -f CMakeFiles/syslog.dir/build.make CMakeFiles/syslog.dir/chapter07/syslog.cpp.o.provides.build
.PHONY : CMakeFiles/syslog.dir/chapter07/syslog.cpp.o.provides

CMakeFiles/syslog.dir/chapter07/syslog.cpp.o.provides.build: CMakeFiles/syslog.dir/chapter07/syslog.cpp.o


# Object files for target syslog
syslog_OBJECTS = \
"CMakeFiles/syslog.dir/chapter07/syslog.cpp.o"

# External object files for target syslog
syslog_EXTERNAL_OBJECTS =

syslog: CMakeFiles/syslog.dir/chapter07/syslog.cpp.o
syslog: CMakeFiles/syslog.dir/build.make
syslog: CMakeFiles/syslog.dir/link.txt
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --bold --progress-dir=/home/lht/.zhuque/highPerformanceLinuxServer/cmake-build-debug/CMakeFiles --progress-num=$(CMAKE_PROGRESS_2) "Linking CXX executable syslog"
	$(CMAKE_COMMAND) -E cmake_link_script CMakeFiles/syslog.dir/link.txt --verbose=$(VERBOSE)

# Rule to build all files generated by this target.
CMakeFiles/syslog.dir/build: syslog

.PHONY : CMakeFiles/syslog.dir/build

CMakeFiles/syslog.dir/requires: CMakeFiles/syslog.dir/chapter07/syslog.cpp.o.requires

.PHONY : CMakeFiles/syslog.dir/requires

CMakeFiles/syslog.dir/clean:
	$(CMAKE_COMMAND) -P CMakeFiles/syslog.dir/cmake_clean.cmake
.PHONY : CMakeFiles/syslog.dir/clean

CMakeFiles/syslog.dir/depend:
	cd /home/lht/.zhuque/highPerformanceLinuxServer/cmake-build-debug && $(CMAKE_COMMAND) -E cmake_depends "Unix Makefiles" /home/lht/.zhuque/highPerformanceLinuxServer /home/lht/.zhuque/highPerformanceLinuxServer /home/lht/.zhuque/highPerformanceLinuxServer/cmake-build-debug /home/lht/.zhuque/highPerformanceLinuxServer/cmake-build-debug /home/lht/.zhuque/highPerformanceLinuxServer/cmake-build-debug/CMakeFiles/syslog.dir/DependInfo.cmake --color=$(COLOR)
.PHONY : CMakeFiles/syslog.dir/depend

