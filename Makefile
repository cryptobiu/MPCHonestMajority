# CMAKE generated file: DO NOT EDIT!
# Generated by "Unix Makefiles" Generator, CMake Version 3.5

# Default target executed when no arguments are given to make.
default_target: all

.PHONY : default_target

# Allow only one "make -f Makefile2" at a time, but pass parallelism.
.NOTPARALLEL:


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
CMAKE_SOURCE_DIR = /home/meital/ClionProjects/Secret-Sharing

# The top-level build directory on which CMake was run.
CMAKE_BINARY_DIR = /home/meital/ClionProjects/Secret-Sharing

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
	/usr/bin/cmake -H$(CMAKE_SOURCE_DIR) -B$(CMAKE_BINARY_DIR)
.PHONY : rebuild_cache

# Special rule for the target rebuild_cache
rebuild_cache/fast: rebuild_cache

.PHONY : rebuild_cache/fast

# The main all target
all: cmake_check_build_system
	$(CMAKE_COMMAND) -E cmake_progress_start /home/meital/ClionProjects/Secret-Sharing/CMakeFiles /home/meital/ClionProjects/Secret-Sharing/CMakeFiles/progress.marks
	$(MAKE) -f CMakeFiles/Makefile2 all
	$(CMAKE_COMMAND) -E cmake_progress_start /home/meital/ClionProjects/Secret-Sharing/CMakeFiles 0
.PHONY : all

# The main clean target
clean:
	$(MAKE) -f CMakeFiles/Makefile2 clean
.PHONY : clean

# The main clean target
clean/fast: clean

.PHONY : clean/fast

# Prepare targets for installation.
preinstall: all
	$(MAKE) -f CMakeFiles/Makefile2 preinstall
.PHONY : preinstall

# Prepare targets for installation.
preinstall/fast:
	$(MAKE) -f CMakeFiles/Makefile2 preinstall
.PHONY : preinstall/fast

# clear depends
depend:
	$(CMAKE_COMMAND) -H$(CMAKE_SOURCE_DIR) -B$(CMAKE_BINARY_DIR) --check-build-system CMakeFiles/Makefile.cmake 1
.PHONY : depend

#=============================================================================
# Target rules for targets named Secret_Sharing

# Build rule for target.
Secret_Sharing: cmake_check_build_system
	$(MAKE) -f CMakeFiles/Makefile2 Secret_Sharing
.PHONY : Secret_Sharing

# fast build rule for target.
Secret_Sharing/fast:
	$(MAKE) -f CMakeFiles/Secret_Sharing.dir/build.make CMakeFiles/Secret_Sharing.dir/build
.PHONY : Secret_Sharing/fast

AES_PRG.o: AES_PRG.cpp.o

.PHONY : AES_PRG.o

# target to build an object file
AES_PRG.cpp.o:
	$(MAKE) -f CMakeFiles/Secret_Sharing.dir/build.make CMakeFiles/Secret_Sharing.dir/AES_PRG.cpp.o
.PHONY : AES_PRG.cpp.o

AES_PRG.i: AES_PRG.cpp.i

.PHONY : AES_PRG.i

# target to preprocess a source file
AES_PRG.cpp.i:
	$(MAKE) -f CMakeFiles/Secret_Sharing.dir/build.make CMakeFiles/Secret_Sharing.dir/AES_PRG.cpp.i
.PHONY : AES_PRG.cpp.i

AES_PRG.s: AES_PRG.cpp.s

.PHONY : AES_PRG.s

# target to generate assembly for a file
AES_PRG.cpp.s:
	$(MAKE) -f CMakeFiles/Secret_Sharing.dir/build.make CMakeFiles/Secret_Sharing.dir/AES_PRG.cpp.s
.PHONY : AES_PRG.cpp.s

ArithmeticCircuit.o: ArithmeticCircuit.cpp.o

.PHONY : ArithmeticCircuit.o

# target to build an object file
ArithmeticCircuit.cpp.o:
	$(MAKE) -f CMakeFiles/Secret_Sharing.dir/build.make CMakeFiles/Secret_Sharing.dir/ArithmeticCircuit.cpp.o
.PHONY : ArithmeticCircuit.cpp.o

ArithmeticCircuit.i: ArithmeticCircuit.cpp.i

.PHONY : ArithmeticCircuit.i

# target to preprocess a source file
ArithmeticCircuit.cpp.i:
	$(MAKE) -f CMakeFiles/Secret_Sharing.dir/build.make CMakeFiles/Secret_Sharing.dir/ArithmeticCircuit.cpp.i
.PHONY : ArithmeticCircuit.cpp.i

ArithmeticCircuit.s: ArithmeticCircuit.cpp.s

.PHONY : ArithmeticCircuit.s

# target to generate assembly for a file
ArithmeticCircuit.cpp.s:
	$(MAKE) -f CMakeFiles/Secret_Sharing.dir/build.make CMakeFiles/Secret_Sharing.dir/ArithmeticCircuit.cpp.s
.PHONY : ArithmeticCircuit.cpp.s

Communication.o: Communication.cpp.o

.PHONY : Communication.o

# target to build an object file
Communication.cpp.o:
	$(MAKE) -f CMakeFiles/Secret_Sharing.dir/build.make CMakeFiles/Secret_Sharing.dir/Communication.cpp.o
.PHONY : Communication.cpp.o

Communication.i: Communication.cpp.i

.PHONY : Communication.i

# target to preprocess a source file
Communication.cpp.i:
	$(MAKE) -f CMakeFiles/Secret_Sharing.dir/build.make CMakeFiles/Secret_Sharing.dir/Communication.cpp.i
.PHONY : Communication.cpp.i

Communication.s: Communication.cpp.s

.PHONY : Communication.s

# target to generate assembly for a file
Communication.cpp.s:
	$(MAKE) -f CMakeFiles/Secret_Sharing.dir/build.make CMakeFiles/Secret_Sharing.dir/Communication.cpp.s
.PHONY : Communication.cpp.s

HIM.o: HIM.cpp.o

.PHONY : HIM.o

# target to build an object file
HIM.cpp.o:
	$(MAKE) -f CMakeFiles/Secret_Sharing.dir/build.make CMakeFiles/Secret_Sharing.dir/HIM.cpp.o
.PHONY : HIM.cpp.o

HIM.i: HIM.cpp.i

.PHONY : HIM.i

# target to preprocess a source file
HIM.cpp.i:
	$(MAKE) -f CMakeFiles/Secret_Sharing.dir/build.make CMakeFiles/Secret_Sharing.dir/HIM.cpp.i
.PHONY : HIM.cpp.i

HIM.s: HIM.cpp.s

.PHONY : HIM.s

# target to generate assembly for a file
HIM.cpp.s:
	$(MAKE) -f CMakeFiles/Secret_Sharing.dir/build.make CMakeFiles/Secret_Sharing.dir/HIM.cpp.s
.PHONY : HIM.cpp.s

Protocol.o: Protocol.cpp.o

.PHONY : Protocol.o

# target to build an object file
Protocol.cpp.o:
	$(MAKE) -f CMakeFiles/Secret_Sharing.dir/build.make CMakeFiles/Secret_Sharing.dir/Protocol.cpp.o
.PHONY : Protocol.cpp.o

Protocol.i: Protocol.cpp.i

.PHONY : Protocol.i

# target to preprocess a source file
Protocol.cpp.i:
	$(MAKE) -f CMakeFiles/Secret_Sharing.dir/build.make CMakeFiles/Secret_Sharing.dir/Protocol.cpp.i
.PHONY : Protocol.cpp.i

Protocol.s: Protocol.cpp.s

.PHONY : Protocol.s

# target to generate assembly for a file
Protocol.cpp.s:
	$(MAKE) -f CMakeFiles/Secret_Sharing.dir/build.make CMakeFiles/Secret_Sharing.dir/Protocol.cpp.s
.PHONY : Protocol.cpp.s

TField.o: TField.cpp.o

.PHONY : TField.o

# target to build an object file
TField.cpp.o:
	$(MAKE) -f CMakeFiles/Secret_Sharing.dir/build.make CMakeFiles/Secret_Sharing.dir/TField.cpp.o
.PHONY : TField.cpp.o

TField.i: TField.cpp.i

.PHONY : TField.i

# target to preprocess a source file
TField.cpp.i:
	$(MAKE) -f CMakeFiles/Secret_Sharing.dir/build.make CMakeFiles/Secret_Sharing.dir/TField.cpp.i
.PHONY : TField.cpp.i

TField.s: TField.cpp.s

.PHONY : TField.s

# target to generate assembly for a file
TField.cpp.s:
	$(MAKE) -f CMakeFiles/Secret_Sharing.dir/build.make CMakeFiles/Secret_Sharing.dir/TField.cpp.s
.PHONY : TField.cpp.s

TFieldElement.o: TFieldElement.cpp.o

.PHONY : TFieldElement.o

# target to build an object file
TFieldElement.cpp.o:
	$(MAKE) -f CMakeFiles/Secret_Sharing.dir/build.make CMakeFiles/Secret_Sharing.dir/TFieldElement.cpp.o
.PHONY : TFieldElement.cpp.o

TFieldElement.i: TFieldElement.cpp.i

.PHONY : TFieldElement.i

# target to preprocess a source file
TFieldElement.cpp.i:
	$(MAKE) -f CMakeFiles/Secret_Sharing.dir/build.make CMakeFiles/Secret_Sharing.dir/TFieldElement.cpp.i
.PHONY : TFieldElement.cpp.i

TFieldElement.s: TFieldElement.cpp.s

.PHONY : TFieldElement.s

# target to generate assembly for a file
TFieldElement.cpp.s:
	$(MAKE) -f CMakeFiles/Secret_Sharing.dir/build.make CMakeFiles/Secret_Sharing.dir/TFieldElement.cpp.s
.PHONY : TFieldElement.cpp.s

TFieldElementGF2E.o: TFieldElementGF2E.cpp.o

.PHONY : TFieldElementGF2E.o

# target to build an object file
TFieldElementGF2E.cpp.o:
	$(MAKE) -f CMakeFiles/Secret_Sharing.dir/build.make CMakeFiles/Secret_Sharing.dir/TFieldElementGF2E.cpp.o
.PHONY : TFieldElementGF2E.cpp.o

TFieldElementGF2E.i: TFieldElementGF2E.cpp.i

.PHONY : TFieldElementGF2E.i

# target to preprocess a source file
TFieldElementGF2E.cpp.i:
	$(MAKE) -f CMakeFiles/Secret_Sharing.dir/build.make CMakeFiles/Secret_Sharing.dir/TFieldElementGF2E.cpp.i
.PHONY : TFieldElementGF2E.cpp.i

TFieldElementGF2E.s: TFieldElementGF2E.cpp.s

.PHONY : TFieldElementGF2E.s

# target to generate assembly for a file
TFieldElementGF2E.cpp.s:
	$(MAKE) -f CMakeFiles/Secret_Sharing.dir/build.make CMakeFiles/Secret_Sharing.dir/TFieldElementGF2E.cpp.s
.PHONY : TFieldElementGF2E.cpp.s

TFieldElementZp.o: TFieldElementZp.cpp.o

.PHONY : TFieldElementZp.o

# target to build an object file
TFieldElementZp.cpp.o:
	$(MAKE) -f CMakeFiles/Secret_Sharing.dir/build.make CMakeFiles/Secret_Sharing.dir/TFieldElementZp.cpp.o
.PHONY : TFieldElementZp.cpp.o

TFieldElementZp.i: TFieldElementZp.cpp.i

.PHONY : TFieldElementZp.i

# target to preprocess a source file
TFieldElementZp.cpp.i:
	$(MAKE) -f CMakeFiles/Secret_Sharing.dir/build.make CMakeFiles/Secret_Sharing.dir/TFieldElementZp.cpp.i
.PHONY : TFieldElementZp.cpp.i

TFieldElementZp.s: TFieldElementZp.cpp.s

.PHONY : TFieldElementZp.s

# target to generate assembly for a file
TFieldElementZp.cpp.s:
	$(MAKE) -f CMakeFiles/Secret_Sharing.dir/build.make CMakeFiles/Secret_Sharing.dir/TFieldElementZp.cpp.s
.PHONY : TFieldElementZp.cpp.s

TFieldGF2E.o: TFieldGF2E.cpp.o

.PHONY : TFieldGF2E.o

# target to build an object file
TFieldGF2E.cpp.o:
	$(MAKE) -f CMakeFiles/Secret_Sharing.dir/build.make CMakeFiles/Secret_Sharing.dir/TFieldGF2E.cpp.o
.PHONY : TFieldGF2E.cpp.o

TFieldGF2E.i: TFieldGF2E.cpp.i

.PHONY : TFieldGF2E.i

# target to preprocess a source file
TFieldGF2E.cpp.i:
	$(MAKE) -f CMakeFiles/Secret_Sharing.dir/build.make CMakeFiles/Secret_Sharing.dir/TFieldGF2E.cpp.i
.PHONY : TFieldGF2E.cpp.i

TFieldGF2E.s: TFieldGF2E.cpp.s

.PHONY : TFieldGF2E.s

# target to generate assembly for a file
TFieldGF2E.cpp.s:
	$(MAKE) -f CMakeFiles/Secret_Sharing.dir/build.make CMakeFiles/Secret_Sharing.dir/TFieldGF2E.cpp.s
.PHONY : TFieldGF2E.cpp.s

TFieldZp.o: TFieldZp.cpp.o

.PHONY : TFieldZp.o

# target to build an object file
TFieldZp.cpp.o:
	$(MAKE) -f CMakeFiles/Secret_Sharing.dir/build.make CMakeFiles/Secret_Sharing.dir/TFieldZp.cpp.o
.PHONY : TFieldZp.cpp.o

TFieldZp.i: TFieldZp.cpp.i

.PHONY : TFieldZp.i

# target to preprocess a source file
TFieldZp.cpp.i:
	$(MAKE) -f CMakeFiles/Secret_Sharing.dir/build.make CMakeFiles/Secret_Sharing.dir/TFieldZp.cpp.i
.PHONY : TFieldZp.cpp.i

TFieldZp.s: TFieldZp.cpp.s

.PHONY : TFieldZp.s

# target to generate assembly for a file
TFieldZp.cpp.s:
	$(MAKE) -f CMakeFiles/Secret_Sharing.dir/build.make CMakeFiles/Secret_Sharing.dir/TFieldZp.cpp.s
.PHONY : TFieldZp.cpp.s

TParty.o: TParty.cpp.o

.PHONY : TParty.o

# target to build an object file
TParty.cpp.o:
	$(MAKE) -f CMakeFiles/Secret_Sharing.dir/build.make CMakeFiles/Secret_Sharing.dir/TParty.cpp.o
.PHONY : TParty.cpp.o

TParty.i: TParty.cpp.i

.PHONY : TParty.i

# target to preprocess a source file
TParty.cpp.i:
	$(MAKE) -f CMakeFiles/Secret_Sharing.dir/build.make CMakeFiles/Secret_Sharing.dir/TParty.cpp.i
.PHONY : TParty.cpp.i

TParty.s: TParty.cpp.s

.PHONY : TParty.s

# target to generate assembly for a file
TParty.cpp.s:
	$(MAKE) -f CMakeFiles/Secret_Sharing.dir/build.make CMakeFiles/Secret_Sharing.dir/TParty.cpp.s
.PHONY : TParty.cpp.s

VDM.o: VDM.cpp.o

.PHONY : VDM.o

# target to build an object file
VDM.cpp.o:
	$(MAKE) -f CMakeFiles/Secret_Sharing.dir/build.make CMakeFiles/Secret_Sharing.dir/VDM.cpp.o
.PHONY : VDM.cpp.o

VDM.i: VDM.cpp.i

.PHONY : VDM.i

# target to preprocess a source file
VDM.cpp.i:
	$(MAKE) -f CMakeFiles/Secret_Sharing.dir/build.make CMakeFiles/Secret_Sharing.dir/VDM.cpp.i
.PHONY : VDM.cpp.i

VDM.s: VDM.cpp.s

.PHONY : VDM.s

# target to generate assembly for a file
VDM.cpp.s:
	$(MAKE) -f CMakeFiles/Secret_Sharing.dir/build.make CMakeFiles/Secret_Sharing.dir/VDM.cpp.s
.PHONY : VDM.cpp.s

main.o: main.cpp.o

.PHONY : main.o

# target to build an object file
main.cpp.o:
	$(MAKE) -f CMakeFiles/Secret_Sharing.dir/build.make CMakeFiles/Secret_Sharing.dir/main.cpp.o
.PHONY : main.cpp.o

main.i: main.cpp.i

.PHONY : main.i

# target to preprocess a source file
main.cpp.i:
	$(MAKE) -f CMakeFiles/Secret_Sharing.dir/build.make CMakeFiles/Secret_Sharing.dir/main.cpp.i
.PHONY : main.cpp.i

main.s: main.cpp.s

.PHONY : main.s

# target to generate assembly for a file
main.cpp.s:
	$(MAKE) -f CMakeFiles/Secret_Sharing.dir/build.make CMakeFiles/Secret_Sharing.dir/main.cpp.s
.PHONY : main.cpp.s

# Help Target
help:
	@echo "The following are some of the valid targets for this Makefile:"
	@echo "... all (the default if no target is provided)"
	@echo "... clean"
	@echo "... depend"
	@echo "... edit_cache"
	@echo "... rebuild_cache"
	@echo "... Secret_Sharing"
	@echo "... AES_PRG.o"
	@echo "... AES_PRG.i"
	@echo "... AES_PRG.s"
	@echo "... ArithmeticCircuit.o"
	@echo "... ArithmeticCircuit.i"
	@echo "... ArithmeticCircuit.s"
	@echo "... Communication.o"
	@echo "... Communication.i"
	@echo "... Communication.s"
	@echo "... HIM.o"
	@echo "... HIM.i"
	@echo "... HIM.s"
	@echo "... Protocol.o"
	@echo "... Protocol.i"
	@echo "... Protocol.s"
	@echo "... TField.o"
	@echo "... TField.i"
	@echo "... TField.s"
	@echo "... TFieldElement.o"
	@echo "... TFieldElement.i"
	@echo "... TFieldElement.s"
	@echo "... TFieldElementGF2E.o"
	@echo "... TFieldElementGF2E.i"
	@echo "... TFieldElementGF2E.s"
	@echo "... TFieldElementZp.o"
	@echo "... TFieldElementZp.i"
	@echo "... TFieldElementZp.s"
	@echo "... TFieldGF2E.o"
	@echo "... TFieldGF2E.i"
	@echo "... TFieldGF2E.s"
	@echo "... TFieldZp.o"
	@echo "... TFieldZp.i"
	@echo "... TFieldZp.s"
	@echo "... TParty.o"
	@echo "... TParty.i"
	@echo "... TParty.s"
	@echo "... VDM.o"
	@echo "... VDM.i"
	@echo "... VDM.s"
	@echo "... main.o"
	@echo "... main.i"
	@echo "... main.s"
.PHONY : help



#=============================================================================
# Special targets to cleanup operation of make.

# Special rule to run CMake to check the build system integrity.
# No rule that depends on this can have commands that come from listfiles
# because they might be regenerated.
cmake_check_build_system:
	$(CMAKE_COMMAND) -H$(CMAKE_SOURCE_DIR) -B$(CMAKE_BINARY_DIR) --check-build-system CMakeFiles/Makefile.cmake 0
.PHONY : cmake_check_build_system

