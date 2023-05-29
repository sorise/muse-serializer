# CMAKE generated file: DO NOT EDIT!
# Generated by "Unix Makefiles" Generator, CMake Version 3.25

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
CMAKE_COMMAND = /home/remix/progress/clion-2022.3.3/bin/cmake/linux/x64/bin/cmake

# The command to remove a file.
RM = /home/remix/progress/clion-2022.3.3/bin/cmake/linux/x64/bin/cmake -E rm -f

# Escaping for special characters.
EQUALS = =

# The top-level source directory on which CMake was run.
CMAKE_SOURCE_DIR = /home/remix/coding/cpp/serializer

# The top-level build directory on which CMake was run.
CMAKE_BINARY_DIR = /home/remix/coding/cpp/serializer/build

# Include any dependencies generated for this target.
include src/CMakeFiles/serializer.dir/depend.make
# Include any dependencies generated by the compiler for this target.
include src/CMakeFiles/serializer.dir/compiler_depend.make

# Include the progress variables for this target.
include src/CMakeFiles/serializer.dir/progress.make

# Include the compile flags for this target's objects.
include src/CMakeFiles/serializer.dir/flags.make

src/CMakeFiles/serializer.dir/serializer/binarySerializer.cpp.o: src/CMakeFiles/serializer.dir/flags.make
src/CMakeFiles/serializer.dir/serializer/binarySerializer.cpp.o: /home/remix/coding/cpp/serializer/src/serializer/binarySerializer.cpp
src/CMakeFiles/serializer.dir/serializer/binarySerializer.cpp.o: src/CMakeFiles/serializer.dir/compiler_depend.ts
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/home/remix/coding/cpp/serializer/build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_1) "Building CXX object src/CMakeFiles/serializer.dir/serializer/binarySerializer.cpp.o"
	cd /home/remix/coding/cpp/serializer/build/src && /bin/g++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -MD -MT src/CMakeFiles/serializer.dir/serializer/binarySerializer.cpp.o -MF CMakeFiles/serializer.dir/serializer/binarySerializer.cpp.o.d -o CMakeFiles/serializer.dir/serializer/binarySerializer.cpp.o -c /home/remix/coding/cpp/serializer/src/serializer/binarySerializer.cpp

src/CMakeFiles/serializer.dir/serializer/binarySerializer.cpp.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/serializer.dir/serializer/binarySerializer.cpp.i"
	cd /home/remix/coding/cpp/serializer/build/src && /bin/g++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -E /home/remix/coding/cpp/serializer/src/serializer/binarySerializer.cpp > CMakeFiles/serializer.dir/serializer/binarySerializer.cpp.i

src/CMakeFiles/serializer.dir/serializer/binarySerializer.cpp.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/serializer.dir/serializer/binarySerializer.cpp.s"
	cd /home/remix/coding/cpp/serializer/build/src && /bin/g++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -S /home/remix/coding/cpp/serializer/src/serializer/binarySerializer.cpp -o CMakeFiles/serializer.dir/serializer/binarySerializer.cpp.s

src/CMakeFiles/serializer.dir/serializer/util.cpp.o: src/CMakeFiles/serializer.dir/flags.make
src/CMakeFiles/serializer.dir/serializer/util.cpp.o: /home/remix/coding/cpp/serializer/src/serializer/util.cpp
src/CMakeFiles/serializer.dir/serializer/util.cpp.o: src/CMakeFiles/serializer.dir/compiler_depend.ts
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/home/remix/coding/cpp/serializer/build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_2) "Building CXX object src/CMakeFiles/serializer.dir/serializer/util.cpp.o"
	cd /home/remix/coding/cpp/serializer/build/src && /bin/g++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -MD -MT src/CMakeFiles/serializer.dir/serializer/util.cpp.o -MF CMakeFiles/serializer.dir/serializer/util.cpp.o.d -o CMakeFiles/serializer.dir/serializer/util.cpp.o -c /home/remix/coding/cpp/serializer/src/serializer/util.cpp

src/CMakeFiles/serializer.dir/serializer/util.cpp.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/serializer.dir/serializer/util.cpp.i"
	cd /home/remix/coding/cpp/serializer/build/src && /bin/g++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -E /home/remix/coding/cpp/serializer/src/serializer/util.cpp > CMakeFiles/serializer.dir/serializer/util.cpp.i

src/CMakeFiles/serializer.dir/serializer/util.cpp.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/serializer.dir/serializer/util.cpp.s"
	cd /home/remix/coding/cpp/serializer/build/src && /bin/g++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -S /home/remix/coding/cpp/serializer/src/serializer/util.cpp -o CMakeFiles/serializer.dir/serializer/util.cpp.s

# Object files for target serializer
serializer_OBJECTS = \
"CMakeFiles/serializer.dir/serializer/binarySerializer.cpp.o" \
"CMakeFiles/serializer.dir/serializer/util.cpp.o"

# External object files for target serializer
serializer_EXTERNAL_OBJECTS =

src/libserializer.so: src/CMakeFiles/serializer.dir/serializer/binarySerializer.cpp.o
src/libserializer.so: src/CMakeFiles/serializer.dir/serializer/util.cpp.o
src/libserializer.so: src/CMakeFiles/serializer.dir/build.make
src/libserializer.so: src/CMakeFiles/serializer.dir/link.txt
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --bold --progress-dir=/home/remix/coding/cpp/serializer/build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_3) "Linking CXX shared library libserializer.so"
	cd /home/remix/coding/cpp/serializer/build/src && $(CMAKE_COMMAND) -E cmake_link_script CMakeFiles/serializer.dir/link.txt --verbose=$(VERBOSE)

# Rule to build all files generated by this target.
src/CMakeFiles/serializer.dir/build: src/libserializer.so
.PHONY : src/CMakeFiles/serializer.dir/build

src/CMakeFiles/serializer.dir/clean:
	cd /home/remix/coding/cpp/serializer/build/src && $(CMAKE_COMMAND) -P CMakeFiles/serializer.dir/cmake_clean.cmake
.PHONY : src/CMakeFiles/serializer.dir/clean

src/CMakeFiles/serializer.dir/depend:
	cd /home/remix/coding/cpp/serializer/build && $(CMAKE_COMMAND) -E cmake_depends "Unix Makefiles" /home/remix/coding/cpp/serializer /home/remix/coding/cpp/serializer/src /home/remix/coding/cpp/serializer/build /home/remix/coding/cpp/serializer/build/src /home/remix/coding/cpp/serializer/build/src/CMakeFiles/serializer.dir/DependInfo.cmake --color=$(COLOR)
.PHONY : src/CMakeFiles/serializer.dir/depend
