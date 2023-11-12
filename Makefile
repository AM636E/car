# CMAKE generated file: DO NOT EDIT!
# Generated by "Unix Makefiles" Generator, CMake Version 3.22

# Default target executed when no arguments are given to make.
default_target: all
.PHONY : default_target

# Allow only one "make -f Makefile2" at a time, but pass parallelism.
.NOTPARALLEL:

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
CMAKE_COMMAND = /usr/bin/cmake

# The command to remove a file.
RM = /usr/bin/cmake -E rm -f

# Escaping for special characters.
EQUALS = =

# The top-level source directory on which CMake was run.
CMAKE_SOURCE_DIR = /home/nazarevic/projects/car

# The top-level build directory on which CMake was run.
CMAKE_BINARY_DIR = /home/nazarevic/projects/car

#=============================================================================
# Targets provided globally by CMake.

# Special rule for the target edit_cache
edit_cache:
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --cyan "No interactive CMake dialog available..."
	/usr/bin/cmake -E echo No\ interactive\ CMake\ dialog\ available.
.PHONY : edit_cache

# Special rule for the target edit_cache
edit_cache/fast: edit_cache
.PHONY : edit_cache/fast

# Special rule for the target rebuild_cache
rebuild_cache:
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --cyan "Running CMake to regenerate build system..."
	/usr/bin/cmake --regenerate-during-build -S$(CMAKE_SOURCE_DIR) -B$(CMAKE_BINARY_DIR)
.PHONY : rebuild_cache

# Special rule for the target rebuild_cache
rebuild_cache/fast: rebuild_cache
.PHONY : rebuild_cache/fast

# The main all target
all: cmake_check_build_system
	$(CMAKE_COMMAND) -E cmake_progress_start /home/nazarevic/projects/car/CMakeFiles /home/nazarevic/projects/car//CMakeFiles/progress.marks
	$(MAKE) $(MAKESILENT) -f CMakeFiles/Makefile2 all
	$(CMAKE_COMMAND) -E cmake_progress_start /home/nazarevic/projects/car/CMakeFiles 0
.PHONY : all

# The main clean target
clean:
	$(MAKE) $(MAKESILENT) -f CMakeFiles/Makefile2 clean
.PHONY : clean

# The main clean target
clean/fast: clean
.PHONY : clean/fast

# Prepare targets for installation.
preinstall: all
	$(MAKE) $(MAKESILENT) -f CMakeFiles/Makefile2 preinstall
.PHONY : preinstall

# Prepare targets for installation.
preinstall/fast:
	$(MAKE) $(MAKESILENT) -f CMakeFiles/Makefile2 preinstall
.PHONY : preinstall/fast

# clear depends
depend:
	$(CMAKE_COMMAND) -S$(CMAKE_SOURCE_DIR) -B$(CMAKE_BINARY_DIR) --check-build-system CMakeFiles/Makefile.cmake 1
.PHONY : depend

#=============================================================================
# Target rules for targets named smash

# Build rule for target.
smash: cmake_check_build_system
	$(MAKE) $(MAKESILENT) -f CMakeFiles/Makefile2 smash
.PHONY : smash

# fast build rule for target.
smash/fast:
	$(MAKE) $(MAKESILENT) -f CMakeFiles/smash.dir/build.make CMakeFiles/smash.dir/build
.PHONY : smash/fast

src/bluetooth.o: src/bluetooth.c.o
.PHONY : src/bluetooth.o

# target to build an object file
src/bluetooth.c.o:
	$(MAKE) $(MAKESILENT) -f CMakeFiles/smash.dir/build.make CMakeFiles/smash.dir/src/bluetooth.c.o
.PHONY : src/bluetooth.c.o

src/bluetooth.i: src/bluetooth.c.i
.PHONY : src/bluetooth.i

# target to preprocess a source file
src/bluetooth.c.i:
	$(MAKE) $(MAKESILENT) -f CMakeFiles/smash.dir/build.make CMakeFiles/smash.dir/src/bluetooth.c.i
.PHONY : src/bluetooth.c.i

src/bluetooth.s: src/bluetooth.c.s
.PHONY : src/bluetooth.s

# target to generate assembly for a file
src/bluetooth.c.s:
	$(MAKE) $(MAKESILENT) -f CMakeFiles/smash.dir/build.make CMakeFiles/smash.dir/src/bluetooth.c.s
.PHONY : src/bluetooth.c.s

src/car.o: src/car.c.o
.PHONY : src/car.o

# target to build an object file
src/car.c.o:
	$(MAKE) $(MAKESILENT) -f CMakeFiles/smash.dir/build.make CMakeFiles/smash.dir/src/car.c.o
.PHONY : src/car.c.o

src/car.i: src/car.c.i
.PHONY : src/car.i

# target to preprocess a source file
src/car.c.i:
	$(MAKE) $(MAKESILENT) -f CMakeFiles/smash.dir/build.make CMakeFiles/smash.dir/src/car.c.i
.PHONY : src/car.c.i

src/car.s: src/car.c.s
.PHONY : src/car.s

# target to generate assembly for a file
src/car.c.s:
	$(MAKE) $(MAKESILENT) -f CMakeFiles/smash.dir/build.make CMakeFiles/smash.dir/src/car.c.s
.PHONY : src/car.c.s

src/gpio.o: src/gpio.c.o
.PHONY : src/gpio.o

# target to build an object file
src/gpio.c.o:
	$(MAKE) $(MAKESILENT) -f CMakeFiles/smash.dir/build.make CMakeFiles/smash.dir/src/gpio.c.o
.PHONY : src/gpio.c.o

src/gpio.i: src/gpio.c.i
.PHONY : src/gpio.i

# target to preprocess a source file
src/gpio.c.i:
	$(MAKE) $(MAKESILENT) -f CMakeFiles/smash.dir/build.make CMakeFiles/smash.dir/src/gpio.c.i
.PHONY : src/gpio.c.i

src/gpio.s: src/gpio.c.s
.PHONY : src/gpio.s

# target to generate assembly for a file
src/gpio.c.s:
	$(MAKE) $(MAKESILENT) -f CMakeFiles/smash.dir/build.make CMakeFiles/smash.dir/src/gpio.c.s
.PHONY : src/gpio.c.s

src/joystick.o: src/joystick.c.o
.PHONY : src/joystick.o

# target to build an object file
src/joystick.c.o:
	$(MAKE) $(MAKESILENT) -f CMakeFiles/smash.dir/build.make CMakeFiles/smash.dir/src/joystick.c.o
.PHONY : src/joystick.c.o

src/joystick.i: src/joystick.c.i
.PHONY : src/joystick.i

# target to preprocess a source file
src/joystick.c.i:
	$(MAKE) $(MAKESILENT) -f CMakeFiles/smash.dir/build.make CMakeFiles/smash.dir/src/joystick.c.i
.PHONY : src/joystick.c.i

src/joystick.s: src/joystick.c.s
.PHONY : src/joystick.s

# target to generate assembly for a file
src/joystick.c.s:
	$(MAKE) $(MAKESILENT) -f CMakeFiles/smash.dir/build.make CMakeFiles/smash.dir/src/joystick.c.s
.PHONY : src/joystick.c.s

src/main.o: src/main.c.o
.PHONY : src/main.o

# target to build an object file
src/main.c.o:
	$(MAKE) $(MAKESILENT) -f CMakeFiles/smash.dir/build.make CMakeFiles/smash.dir/src/main.c.o
.PHONY : src/main.c.o

src/main.i: src/main.c.i
.PHONY : src/main.i

# target to preprocess a source file
src/main.c.i:
	$(MAKE) $(MAKESILENT) -f CMakeFiles/smash.dir/build.make CMakeFiles/smash.dir/src/main.c.i
.PHONY : src/main.c.i

src/main.s: src/main.c.s
.PHONY : src/main.s

# target to generate assembly for a file
src/main.c.s:
	$(MAKE) $(MAKESILENT) -f CMakeFiles/smash.dir/build.make CMakeFiles/smash.dir/src/main.c.s
.PHONY : src/main.c.s

src/motor.o: src/motor.c.o
.PHONY : src/motor.o

# target to build an object file
src/motor.c.o:
	$(MAKE) $(MAKESILENT) -f CMakeFiles/smash.dir/build.make CMakeFiles/smash.dir/src/motor.c.o
.PHONY : src/motor.c.o

src/motor.i: src/motor.c.i
.PHONY : src/motor.i

# target to preprocess a source file
src/motor.c.i:
	$(MAKE) $(MAKESILENT) -f CMakeFiles/smash.dir/build.make CMakeFiles/smash.dir/src/motor.c.i
.PHONY : src/motor.c.i

src/motor.s: src/motor.c.s
.PHONY : src/motor.s

# target to generate assembly for a file
src/motor.c.s:
	$(MAKE) $(MAKESILENT) -f CMakeFiles/smash.dir/build.make CMakeFiles/smash.dir/src/motor.c.s
.PHONY : src/motor.c.s

src/stack.o: src/stack.c.o
.PHONY : src/stack.o

# target to build an object file
src/stack.c.o:
	$(MAKE) $(MAKESILENT) -f CMakeFiles/smash.dir/build.make CMakeFiles/smash.dir/src/stack.c.o
.PHONY : src/stack.c.o

src/stack.i: src/stack.c.i
.PHONY : src/stack.i

# target to preprocess a source file
src/stack.c.i:
	$(MAKE) $(MAKESILENT) -f CMakeFiles/smash.dir/build.make CMakeFiles/smash.dir/src/stack.c.i
.PHONY : src/stack.c.i

src/stack.s: src/stack.c.s
.PHONY : src/stack.s

# target to generate assembly for a file
src/stack.c.s:
	$(MAKE) $(MAKESILENT) -f CMakeFiles/smash.dir/build.make CMakeFiles/smash.dir/src/stack.c.s
.PHONY : src/stack.c.s

# Help Target
help:
	@echo "The following are some of the valid targets for this Makefile:"
	@echo "... all (the default if no target is provided)"
	@echo "... clean"
	@echo "... depend"
	@echo "... edit_cache"
	@echo "... rebuild_cache"
	@echo "... smash"
	@echo "... src/bluetooth.o"
	@echo "... src/bluetooth.i"
	@echo "... src/bluetooth.s"
	@echo "... src/car.o"
	@echo "... src/car.i"
	@echo "... src/car.s"
	@echo "... src/gpio.o"
	@echo "... src/gpio.i"
	@echo "... src/gpio.s"
	@echo "... src/joystick.o"
	@echo "... src/joystick.i"
	@echo "... src/joystick.s"
	@echo "... src/main.o"
	@echo "... src/main.i"
	@echo "... src/main.s"
	@echo "... src/motor.o"
	@echo "... src/motor.i"
	@echo "... src/motor.s"
	@echo "... src/stack.o"
	@echo "... src/stack.i"
	@echo "... src/stack.s"
.PHONY : help



#=============================================================================
# Special targets to cleanup operation of make.

# Special rule to run CMake to check the build system integrity.
# No rule that depends on this can have commands that come from listfiles
# because they might be regenerated.
cmake_check_build_system:
	$(CMAKE_COMMAND) -S$(CMAKE_SOURCE_DIR) -B$(CMAKE_BINARY_DIR) --check-build-system CMakeFiles/Makefile.cmake 0
.PHONY : cmake_check_build_system

