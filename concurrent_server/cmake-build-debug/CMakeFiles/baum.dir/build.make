# CMAKE generated file: DO NOT EDIT!
# Generated by "Unix Makefiles" Generator, CMake Version 3.24

# Delete rule output on recipe failure.
.DELETE_ON_ERROR:

#=============================================================================
# Special targets provided by cmake.

# Disable implicit rules so canonical targets will work.
.SUFFIXES:

# Disable VCS-based implicit rules.
% : %,v

# Disable VCS-based implicit rules.
% : RCS/%

# Disable VCS-based implicit rules.
% : RCS/%,v

# Disable VCS-based implicit rules.
% : SCCS/s.%

# Disable VCS-based implicit rules.
% : s.%

.SUFFIXES: .hpux_make_needs_suffix_list

# Command-line flag to silence nested $(MAKE).
$(VERBOSE)MAKESILENT = -s

#Suppress display of executed commands.
$(VERBOSE).SILENT:

# A target that is always out of date.
cmake_force:
.PHONY : cmake_force

#=============================================================================
# Set environment variables for the build.

# The shell in which to execute make rules.
SHELL = /bin/sh

# The CMake executable.
CMAKE_COMMAND = /home/pi/CLion-2022.3/clion-2022.3/bin/cmake/linux/bin/cmake

# The command to remove a file.
RM = /home/pi/CLion-2022.3/clion-2022.3/bin/cmake/linux/bin/cmake -E rm -f

# Escaping for special characters.
EQUALS = =

# The top-level source directory on which CMake was run.
CMAKE_SOURCE_DIR = /home/pi/CLionProjects/baum

# The top-level build directory on which CMake was run.
CMAKE_BINARY_DIR = /home/pi/CLionProjects/baum/cmake-build-debug

# Include any dependencies generated for this target.
include CMakeFiles/baum.dir/depend.make
# Include any dependencies generated by the compiler for this target.
include CMakeFiles/baum.dir/compiler_depend.make

# Include the progress variables for this target.
include CMakeFiles/baum.dir/progress.make

# Include the compile flags for this target's objects.
include CMakeFiles/baum.dir/flags.make

CMakeFiles/baum.dir/main.cpp.o: CMakeFiles/baum.dir/flags.make
CMakeFiles/baum.dir/main.cpp.o: /home/pi/CLionProjects/baum/main.cpp
CMakeFiles/baum.dir/main.cpp.o: CMakeFiles/baum.dir/compiler_depend.ts
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/home/pi/CLionProjects/baum/cmake-build-debug/CMakeFiles --progress-num=$(CMAKE_PROGRESS_1) "Building CXX object CMakeFiles/baum.dir/main.cpp.o"
	/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -MD -MT CMakeFiles/baum.dir/main.cpp.o -MF CMakeFiles/baum.dir/main.cpp.o.d -o CMakeFiles/baum.dir/main.cpp.o -c /home/pi/CLionProjects/baum/main.cpp

CMakeFiles/baum.dir/main.cpp.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/baum.dir/main.cpp.i"
	/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -E /home/pi/CLionProjects/baum/main.cpp > CMakeFiles/baum.dir/main.cpp.i

CMakeFiles/baum.dir/main.cpp.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/baum.dir/main.cpp.s"
	/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -S /home/pi/CLionProjects/baum/main.cpp -o CMakeFiles/baum.dir/main.cpp.s

# Object files for target baum
baum_OBJECTS = \
"CMakeFiles/baum.dir/main.cpp.o"

# External object files for target baum
baum_EXTERNAL_OBJECTS =

baum: CMakeFiles/baum.dir/main.cpp.o
baum: CMakeFiles/baum.dir/build.make
baum: libnet.a
baum: libServer.a
baum: libconcurrency_utils.a
baum: libutils.a
baum: CMakeFiles/baum.dir/link.txt
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --bold --progress-dir=/home/pi/CLionProjects/baum/cmake-build-debug/CMakeFiles --progress-num=$(CMAKE_PROGRESS_2) "Linking CXX executable baum"
	$(CMAKE_COMMAND) -E cmake_link_script CMakeFiles/baum.dir/link.txt --verbose=$(VERBOSE)

# Rule to build all files generated by this target.
CMakeFiles/baum.dir/build: baum
.PHONY : CMakeFiles/baum.dir/build

CMakeFiles/baum.dir/clean:
	$(CMAKE_COMMAND) -P CMakeFiles/baum.dir/cmake_clean.cmake
.PHONY : CMakeFiles/baum.dir/clean

CMakeFiles/baum.dir/depend:
	cd /home/pi/CLionProjects/baum/cmake-build-debug && $(CMAKE_COMMAND) -E cmake_depends "Unix Makefiles" /home/pi/CLionProjects/baum /home/pi/CLionProjects/baum /home/pi/CLionProjects/baum/cmake-build-debug /home/pi/CLionProjects/baum/cmake-build-debug /home/pi/CLionProjects/baum/cmake-build-debug/CMakeFiles/baum.dir/DependInfo.cmake --color=$(COLOR)
.PHONY : CMakeFiles/baum.dir/depend

