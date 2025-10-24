CC=clang
CFLAGS=-D _DEBUG -g -std=gnu++20 -O3 -Wall -Wextra -Wpedantic -Wno-inline -Wno-cast-align -fdiagnostics-color=always

LIB_SUBDIRS=terminal_decorator error_handling strings logger file_operations array_operations sort stack debug_utilites SPU SPU_assembler AVXXX_N SPU_disassembler list
EXECUTABLE_SUBDIRS=onegin_main SPU_vm_main SPU_assembler_main SPU_disassembler_main list_main
TEST_SUBDIRS=tests

LIB_SUBDIRS_CLEAN=$(addprefix clean/, $(LIB_SUBDIRS))
EXECUTABLE_SUBDIRS_CLEAN=$(addprefix clean/, $(EXECUTABLE_SUBDIRS))
TEST_SUBDIRS_CLEAN=$(addprefix clean/, $(TEST_SUBDIRS))

INCLUDE=$(abspath $(shell find . -type d))
INCLUDE_PARAMS=$(foreach d, $(INCLUDE), -I$d)

LIBS=sdl2

.EXPORT_ALL_VARIABLES:


.PHONY: docs all clean $(LIB_SUBDIRS) $(EXECUTABLE_SUBDIRS) $(TEST_SUBDIRS)

all: $(LIB_SUBDIRS) $(EXECUTABLE_SUBDIRS)

test: $(LIB_SUBDIRS) $(TEST_SUBDIRS)

clean: $(EXECUTABLE_SUBDIRS_CLEAN) $(TEST_SUBDIRS_CLEAN) $(LIB_SUBDIRS_CLEAN) 

docs: 
	doxygen

$(EXECUTABLE_SUBDIRS) $(TEST_SUBDIRS) $(LIB_SUBDIRS):
	$(MAKE) -C $@

$(EXECUTABLE_SUBDIRS_CLEAN) $(TEST_SUBDIRS_CLEAN) $(LIB_SUBDIRS_CLEAN):
	$(MAKE) clean -C $(notdir $@)

