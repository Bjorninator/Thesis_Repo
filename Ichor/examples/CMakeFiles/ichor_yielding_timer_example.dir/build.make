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


# Produce verbose output by default.
VERBOSE = 1

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
CMAKE_SOURCE_DIR = /home/bjorn/Thesis/Ichor

# The top-level build directory on which CMake was run.
CMAKE_BINARY_DIR = /home/bjorn/Thesis/Ichor

# Include any dependencies generated for this target.
include examples/CMakeFiles/ichor_yielding_timer_example.dir/depend.make

# Include the progress variables for this target.
include examples/CMakeFiles/ichor_yielding_timer_example.dir/progress.make

# Include the compile flags for this target's objects.
include examples/CMakeFiles/ichor_yielding_timer_example.dir/flags.make

examples/CMakeFiles/ichor_yielding_timer_example.dir/yielding_timer_example/main.cpp.o: examples/CMakeFiles/ichor_yielding_timer_example.dir/flags.make
examples/CMakeFiles/ichor_yielding_timer_example.dir/yielding_timer_example/main.cpp.o: examples/yielding_timer_example/main.cpp
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/home/bjorn/Thesis/Ichor/CMakeFiles --progress-num=$(CMAKE_PROGRESS_1) "Building CXX object examples/CMakeFiles/ichor_yielding_timer_example.dir/yielding_timer_example/main.cpp.o"
	cd /home/bjorn/Thesis/Ichor/examples && /usr/bin/g++-10  $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -o CMakeFiles/ichor_yielding_timer_example.dir/yielding_timer_example/main.cpp.o -c /home/bjorn/Thesis/Ichor/examples/yielding_timer_example/main.cpp

examples/CMakeFiles/ichor_yielding_timer_example.dir/yielding_timer_example/main.cpp.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/ichor_yielding_timer_example.dir/yielding_timer_example/main.cpp.i"
	cd /home/bjorn/Thesis/Ichor/examples && /usr/bin/g++-10 $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -E /home/bjorn/Thesis/Ichor/examples/yielding_timer_example/main.cpp > CMakeFiles/ichor_yielding_timer_example.dir/yielding_timer_example/main.cpp.i

examples/CMakeFiles/ichor_yielding_timer_example.dir/yielding_timer_example/main.cpp.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/ichor_yielding_timer_example.dir/yielding_timer_example/main.cpp.s"
	cd /home/bjorn/Thesis/Ichor/examples && /usr/bin/g++-10 $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -S /home/bjorn/Thesis/Ichor/examples/yielding_timer_example/main.cpp -o CMakeFiles/ichor_yielding_timer_example.dir/yielding_timer_example/main.cpp.s

# Object files for target ichor_yielding_timer_example
ichor_yielding_timer_example_OBJECTS = \
"CMakeFiles/ichor_yielding_timer_example.dir/yielding_timer_example/main.cpp.o"

# External object files for target ichor_yielding_timer_example
ichor_yielding_timer_example_EXTERNAL_OBJECTS =

bin/ichor_yielding_timer_example: examples/CMakeFiles/ichor_yielding_timer_example.dir/yielding_timer_example/main.cpp.o
bin/ichor_yielding_timer_example: examples/CMakeFiles/ichor_yielding_timer_example.dir/build.make
bin/ichor_yielding_timer_example: bin/libichor.a
bin/ichor_yielding_timer_example: examples/CMakeFiles/ichor_yielding_timer_example.dir/link.txt
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --bold --progress-dir=/home/bjorn/Thesis/Ichor/CMakeFiles --progress-num=$(CMAKE_PROGRESS_2) "Linking CXX executable ../bin/ichor_yielding_timer_example"
	cd /home/bjorn/Thesis/Ichor/examples && $(CMAKE_COMMAND) -E cmake_link_script CMakeFiles/ichor_yielding_timer_example.dir/link.txt --verbose=$(VERBOSE)

# Rule to build all files generated by this target.
examples/CMakeFiles/ichor_yielding_timer_example.dir/build: bin/ichor_yielding_timer_example

.PHONY : examples/CMakeFiles/ichor_yielding_timer_example.dir/build

examples/CMakeFiles/ichor_yielding_timer_example.dir/clean:
	cd /home/bjorn/Thesis/Ichor/examples && $(CMAKE_COMMAND) -P CMakeFiles/ichor_yielding_timer_example.dir/cmake_clean.cmake
.PHONY : examples/CMakeFiles/ichor_yielding_timer_example.dir/clean

examples/CMakeFiles/ichor_yielding_timer_example.dir/depend:
	cd /home/bjorn/Thesis/Ichor && $(CMAKE_COMMAND) -E cmake_depends "Unix Makefiles" /home/bjorn/Thesis/Ichor /home/bjorn/Thesis/Ichor/examples /home/bjorn/Thesis/Ichor /home/bjorn/Thesis/Ichor/examples /home/bjorn/Thesis/Ichor/examples/CMakeFiles/ichor_yielding_timer_example.dir/DependInfo.cmake --color=$(COLOR)
.PHONY : examples/CMakeFiles/ichor_yielding_timer_example.dir/depend

