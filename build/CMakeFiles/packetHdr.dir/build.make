# CMAKE generated file: DO NOT EDIT!
# Generated by "Unix Makefiles" Generator, CMake Version 3.12

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
CMAKE_COMMAND = /usr/local/bin/cmake

# The command to remove a file.
RM = /usr/local/bin/cmake -E remove -f

# Escaping for special characters.
EQUALS = =

# The top-level source directory on which CMake was run.
CMAKE_SOURCE_DIR = /root/mcoco/packetHdr

# The top-level build directory on which CMake was run.
CMAKE_BINARY_DIR = /root/mcoco/packetHdr/build

# Include any dependencies generated for this target.
include CMakeFiles/packetHdr.dir/depend.make

# Include the progress variables for this target.
include CMakeFiles/packetHdr.dir/progress.make

# Include the compile flags for this target's objects.
include CMakeFiles/packetHdr.dir/flags.make

CMakeFiles/packetHdr.dir/src/packetHdr.cpp.o: CMakeFiles/packetHdr.dir/flags.make
CMakeFiles/packetHdr.dir/src/packetHdr.cpp.o: ../src/packetHdr.cpp
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/root/mcoco/packetHdr/build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_1) "Building CXX object CMakeFiles/packetHdr.dir/src/packetHdr.cpp.o"
	/usr/bin/c++  $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -o CMakeFiles/packetHdr.dir/src/packetHdr.cpp.o -c /root/mcoco/packetHdr/src/packetHdr.cpp

CMakeFiles/packetHdr.dir/src/packetHdr.cpp.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/packetHdr.dir/src/packetHdr.cpp.i"
	/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -E /root/mcoco/packetHdr/src/packetHdr.cpp > CMakeFiles/packetHdr.dir/src/packetHdr.cpp.i

CMakeFiles/packetHdr.dir/src/packetHdr.cpp.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/packetHdr.dir/src/packetHdr.cpp.s"
	/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -S /root/mcoco/packetHdr/src/packetHdr.cpp -o CMakeFiles/packetHdr.dir/src/packetHdr.cpp.s

CMakeFiles/packetHdr.dir/src/pktH264.cpp.o: CMakeFiles/packetHdr.dir/flags.make
CMakeFiles/packetHdr.dir/src/pktH264.cpp.o: ../src/pktH264.cpp
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/root/mcoco/packetHdr/build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_2) "Building CXX object CMakeFiles/packetHdr.dir/src/pktH264.cpp.o"
	/usr/bin/c++  $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -o CMakeFiles/packetHdr.dir/src/pktH264.cpp.o -c /root/mcoco/packetHdr/src/pktH264.cpp

CMakeFiles/packetHdr.dir/src/pktH264.cpp.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/packetHdr.dir/src/pktH264.cpp.i"
	/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -E /root/mcoco/packetHdr/src/pktH264.cpp > CMakeFiles/packetHdr.dir/src/pktH264.cpp.i

CMakeFiles/packetHdr.dir/src/pktH264.cpp.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/packetHdr.dir/src/pktH264.cpp.s"
	/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -S /root/mcoco/packetHdr/src/pktH264.cpp -o CMakeFiles/packetHdr.dir/src/pktH264.cpp.s

CMakeFiles/packetHdr.dir/src/pktH265.cpp.o: CMakeFiles/packetHdr.dir/flags.make
CMakeFiles/packetHdr.dir/src/pktH265.cpp.o: ../src/pktH265.cpp
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/root/mcoco/packetHdr/build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_3) "Building CXX object CMakeFiles/packetHdr.dir/src/pktH265.cpp.o"
	/usr/bin/c++  $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -o CMakeFiles/packetHdr.dir/src/pktH265.cpp.o -c /root/mcoco/packetHdr/src/pktH265.cpp

CMakeFiles/packetHdr.dir/src/pktH265.cpp.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/packetHdr.dir/src/pktH265.cpp.i"
	/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -E /root/mcoco/packetHdr/src/pktH265.cpp > CMakeFiles/packetHdr.dir/src/pktH265.cpp.i

CMakeFiles/packetHdr.dir/src/pktH265.cpp.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/packetHdr.dir/src/pktH265.cpp.s"
	/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -S /root/mcoco/packetHdr/src/pktH265.cpp -o CMakeFiles/packetHdr.dir/src/pktH265.cpp.s

# Object files for target packetHdr
packetHdr_OBJECTS = \
"CMakeFiles/packetHdr.dir/src/packetHdr.cpp.o" \
"CMakeFiles/packetHdr.dir/src/pktH264.cpp.o" \
"CMakeFiles/packetHdr.dir/src/pktH265.cpp.o"

# External object files for target packetHdr
packetHdr_EXTERNAL_OBJECTS =

packetHdr: CMakeFiles/packetHdr.dir/src/packetHdr.cpp.o
packetHdr: CMakeFiles/packetHdr.dir/src/pktH264.cpp.o
packetHdr: CMakeFiles/packetHdr.dir/src/pktH265.cpp.o
packetHdr: CMakeFiles/packetHdr.dir/build.make
packetHdr: CMakeFiles/packetHdr.dir/link.txt
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --bold --progress-dir=/root/mcoco/packetHdr/build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_4) "Linking CXX executable packetHdr"
	$(CMAKE_COMMAND) -E cmake_link_script CMakeFiles/packetHdr.dir/link.txt --verbose=$(VERBOSE)

# Rule to build all files generated by this target.
CMakeFiles/packetHdr.dir/build: packetHdr

.PHONY : CMakeFiles/packetHdr.dir/build

CMakeFiles/packetHdr.dir/clean:
	$(CMAKE_COMMAND) -P CMakeFiles/packetHdr.dir/cmake_clean.cmake
.PHONY : CMakeFiles/packetHdr.dir/clean

CMakeFiles/packetHdr.dir/depend:
	cd /root/mcoco/packetHdr/build && $(CMAKE_COMMAND) -E cmake_depends "Unix Makefiles" /root/mcoco/packetHdr /root/mcoco/packetHdr /root/mcoco/packetHdr/build /root/mcoco/packetHdr/build /root/mcoco/packetHdr/build/CMakeFiles/packetHdr.dir/DependInfo.cmake --color=$(COLOR)
.PHONY : CMakeFiles/packetHdr.dir/depend
