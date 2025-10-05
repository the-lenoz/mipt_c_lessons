CFLAGS=-lm -D _DEBUG -g -ggdb3 -std=c++20 -O0 -Wall -Wextra -Wpedantic -Weffc++ -Waggressive-loop-optimizations -Wc++14-compat \
-Wmissing-declarations -Wcast-align -Wcast-qual -Wchar-subscripts -Wconditionally-supported -Wconversion -Wctor-dtor-privacy \
-Wempty-body -Wfloat-equal -Wformat-nonliteral -Wformat-security -Wformat-signedness -Wformat=2 -Winline -Wlogical-op \
-Wnon-virtual-dtor -Wopenmp-simd -Woverloaded-virtual -Wpacked -Wpointer-arith -Winit-self -Wredundant-decls -Wshadow \
-Wsign-conversion -Wsign-promo -Wstrict-null-sentinel -Wstrict-overflow=2 -Wsuggest-attribute=noreturn \
-Wsuggest-final-methods -Wsuggest-final-types -Wsuggest-override -Wswitch-default -Wswitch-enum -Wsync-nand -Wundef \
-Wunreachable-code -Wunused -Wuseless-cast -Wvariadic-macros -Wno-literal-suffix -Wno-missing-field-initializers \
-Wno-narrowing -Wno-old-style-cast -Wno-varargs -Wstack-protector -fcheck-new -fsized-deallocation -fstack-protector \
-fstrict-overflow -fno-omit-frame-pointer -Wlarger-than=65536 -Wstack-usage=65536 -pie -fPIE -Wno-format-nonliteral -fsanitize=address \
-fsanitize=undefined -Wstrict-aliasing=2

LIB_SUBDIRS=terminal_decorator error_handling strings logger file_operations array_operations sort stack calculator debug_utilites SPU
EXECUTABLE_SUBDIRS=onegin_main calculator_main SPU_vm_main SPU_assembler_main SPU_disassembler_main
TEST_SUBDIRS=tests

LIB_SUBDIRS_CLEAN=$(addprefix clean/, $(LIB_SUBDIRS))
EXECUTABLE_SUBDIRS_CLEAN=$(addprefix clean/, $(EXECUTABLE_SUBDIRS))
TEST_SUBDIRS_CLEAN=$(addprefix clean/, $(TEST_SUBDIRS))

INCLUDE=$(abspath $(shell find . -type d))
INCLUDE_PARAMS=$(foreach d, $(INCLUDE), -I$d)


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

