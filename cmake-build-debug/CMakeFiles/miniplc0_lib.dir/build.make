# CMAKE generated file: DO NOT EDIT!
# Generated by "MinGW Makefiles" Generator, CMake Version 3.15

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

SHELL = cmd.exe

# The CMake executable.
CMAKE_COMMAND = "G:\Clion\CLion 2019.2.5\bin\cmake\win\bin\cmake.exe"

# The command to remove a file.
RM = "G:\Clion\CLion 2019.2.5\bin\cmake\win\bin\cmake.exe" -E remove -f

# Escaping for special characters.
EQUALS = =

# The top-level source directory on which CMake was run.
CMAKE_SOURCE_DIR = C:\Users\Cheng\Desktop\c0

# The top-level build directory on which CMake was run.
CMAKE_BINARY_DIR = C:\Users\Cheng\Desktop\c0\cmake-build-debug

# Include any dependencies generated for this target.
include CMakeFiles/miniplc0_lib.dir/depend.make

# Include the progress variables for this target.
include CMakeFiles/miniplc0_lib.dir/progress.make

# Include the compile flags for this target's objects.
include CMakeFiles/miniplc0_lib.dir/flags.make

CMakeFiles/miniplc0_lib.dir/tokenizer/tokenizer.cpp.obj: CMakeFiles/miniplc0_lib.dir/flags.make
CMakeFiles/miniplc0_lib.dir/tokenizer/tokenizer.cpp.obj: CMakeFiles/miniplc0_lib.dir/includes_CXX.rsp
CMakeFiles/miniplc0_lib.dir/tokenizer/tokenizer.cpp.obj: ../tokenizer/tokenizer.cpp
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=C:\Users\Cheng\Desktop\c0\cmake-build-debug\CMakeFiles --progress-num=$(CMAKE_PROGRESS_1) "Building CXX object CMakeFiles/miniplc0_lib.dir/tokenizer/tokenizer.cpp.obj"
	G:\Clion\MinGW\mingw64\bin\g++.exe  $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -o CMakeFiles\miniplc0_lib.dir\tokenizer\tokenizer.cpp.obj -c C:\Users\Cheng\Desktop\c0\tokenizer\tokenizer.cpp

CMakeFiles/miniplc0_lib.dir/tokenizer/tokenizer.cpp.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/miniplc0_lib.dir/tokenizer/tokenizer.cpp.i"
	G:\Clion\MinGW\mingw64\bin\g++.exe $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -E C:\Users\Cheng\Desktop\c0\tokenizer\tokenizer.cpp > CMakeFiles\miniplc0_lib.dir\tokenizer\tokenizer.cpp.i

CMakeFiles/miniplc0_lib.dir/tokenizer/tokenizer.cpp.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/miniplc0_lib.dir/tokenizer/tokenizer.cpp.s"
	G:\Clion\MinGW\mingw64\bin\g++.exe $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -S C:\Users\Cheng\Desktop\c0\tokenizer\tokenizer.cpp -o CMakeFiles\miniplc0_lib.dir\tokenizer\tokenizer.cpp.s

CMakeFiles/miniplc0_lib.dir/analyser/analyser.cpp.obj: CMakeFiles/miniplc0_lib.dir/flags.make
CMakeFiles/miniplc0_lib.dir/analyser/analyser.cpp.obj: CMakeFiles/miniplc0_lib.dir/includes_CXX.rsp
CMakeFiles/miniplc0_lib.dir/analyser/analyser.cpp.obj: ../analyser/analyser.cpp
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=C:\Users\Cheng\Desktop\c0\cmake-build-debug\CMakeFiles --progress-num=$(CMAKE_PROGRESS_2) "Building CXX object CMakeFiles/miniplc0_lib.dir/analyser/analyser.cpp.obj"
	G:\Clion\MinGW\mingw64\bin\g++.exe  $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -o CMakeFiles\miniplc0_lib.dir\analyser\analyser.cpp.obj -c C:\Users\Cheng\Desktop\c0\analyser\analyser.cpp

CMakeFiles/miniplc0_lib.dir/analyser/analyser.cpp.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/miniplc0_lib.dir/analyser/analyser.cpp.i"
	G:\Clion\MinGW\mingw64\bin\g++.exe $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -E C:\Users\Cheng\Desktop\c0\analyser\analyser.cpp > CMakeFiles\miniplc0_lib.dir\analyser\analyser.cpp.i

CMakeFiles/miniplc0_lib.dir/analyser/analyser.cpp.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/miniplc0_lib.dir/analyser/analyser.cpp.s"
	G:\Clion\MinGW\mingw64\bin\g++.exe $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -S C:\Users\Cheng\Desktop\c0\analyser\analyser.cpp -o CMakeFiles\miniplc0_lib.dir\analyser\analyser.cpp.s

# Object files for target miniplc0_lib
miniplc0_lib_OBJECTS = \
"CMakeFiles/miniplc0_lib.dir/tokenizer/tokenizer.cpp.obj" \
"CMakeFiles/miniplc0_lib.dir/analyser/analyser.cpp.obj"

# External object files for target miniplc0_lib
miniplc0_lib_EXTERNAL_OBJECTS =

libminiplc0_lib.a: CMakeFiles/miniplc0_lib.dir/tokenizer/tokenizer.cpp.obj
libminiplc0_lib.a: CMakeFiles/miniplc0_lib.dir/analyser/analyser.cpp.obj
libminiplc0_lib.a: CMakeFiles/miniplc0_lib.dir/build.make
libminiplc0_lib.a: CMakeFiles/miniplc0_lib.dir/link.txt
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --bold --progress-dir=C:\Users\Cheng\Desktop\c0\cmake-build-debug\CMakeFiles --progress-num=$(CMAKE_PROGRESS_3) "Linking CXX static library libminiplc0_lib.a"
	$(CMAKE_COMMAND) -P CMakeFiles\miniplc0_lib.dir\cmake_clean_target.cmake
	$(CMAKE_COMMAND) -E cmake_link_script CMakeFiles\miniplc0_lib.dir\link.txt --verbose=$(VERBOSE)

# Rule to build all files generated by this target.
CMakeFiles/miniplc0_lib.dir/build: libminiplc0_lib.a

.PHONY : CMakeFiles/miniplc0_lib.dir/build

CMakeFiles/miniplc0_lib.dir/clean:
	$(CMAKE_COMMAND) -P CMakeFiles\miniplc0_lib.dir\cmake_clean.cmake
.PHONY : CMakeFiles/miniplc0_lib.dir/clean

CMakeFiles/miniplc0_lib.dir/depend:
	$(CMAKE_COMMAND) -E cmake_depends "MinGW Makefiles" C:\Users\Cheng\Desktop\c0 C:\Users\Cheng\Desktop\c0 C:\Users\Cheng\Desktop\c0\cmake-build-debug C:\Users\Cheng\Desktop\c0\cmake-build-debug C:\Users\Cheng\Desktop\c0\cmake-build-debug\CMakeFiles\miniplc0_lib.dir\DependInfo.cmake --color=$(COLOR)
.PHONY : CMakeFiles/miniplc0_lib.dir/depend

