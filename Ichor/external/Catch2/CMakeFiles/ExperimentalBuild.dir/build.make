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
CMAKE_SOURCE_DIR = /home/bjorn/Thesis/git_repo/Ichor

# The top-level build directory on which CMake was run.
CMAKE_BINARY_DIR = /home/bjorn/Thesis/git_repo/Ichor

# Utility rule file for ExperimentalBuild.

# Include the progress variables for this target.
include external/Catch2/CMakeFiles/ExperimentalBuild.dir/progress.make

external/Catch2/CMakeFiles/ExperimentalBuild:
	cd /home/bjorn/Thesis/git_repo/Ichor/external/Catch2 && /usr/bin/ctest -D ExperimentalBuild

ExperimentalBuild: external/Catch2/CMakeFiles/ExperimentalBuild
ExperimentalBuild: external/Catch2/CMakeFiles/ExperimentalBuild.dir/build.make

.PHONY : ExperimentalBuild

# Rule to build all files generated by this target.
external/Catch2/CMakeFiles/ExperimentalBuild.dir/build: ExperimentalBuild

.PHONY : external/Catch2/CMakeFiles/ExperimentalBuild.dir/build

external/Catch2/CMakeFiles/ExperimentalBuild.dir/clean:
	cd /home/bjorn/Thesis/git_repo/Ichor/external/Catch2 && $(CMAKE_COMMAND) -P CMakeFiles/ExperimentalBuild.dir/cmake_clean.cmake
.PHONY : external/Catch2/CMakeFiles/ExperimentalBuild.dir/clean

external/Catch2/CMakeFiles/ExperimentalBuild.dir/depend:
	cd /home/bjorn/Thesis/git_repo/Ichor && $(CMAKE_COMMAND) -E cmake_depends "Unix Makefiles" /home/bjorn/Thesis/git_repo/Ichor /home/bjorn/Thesis/git_repo/Ichor/external/Catch2 /home/bjorn/Thesis/git_repo/Ichor /home/bjorn/Thesis/git_repo/Ichor/external/Catch2 /home/bjorn/Thesis/git_repo/Ichor/external/Catch2/CMakeFiles/ExperimentalBuild.dir/DependInfo.cmake --color=$(COLOR)
.PHONY : external/Catch2/CMakeFiles/ExperimentalBuild.dir/depend

