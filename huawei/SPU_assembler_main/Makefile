SOURCES=$(shell find . -type f -iname '*.cpp')

OBJECTS+= $(foreach x, $(basename $(SOURCES)), $(x).o)

EXTERNAL_OBJECTS=$(foreach x, $(abspath $(addprefix ../, $(LIB_SUBDIRS))), $(shell find $(x) -type f -iname '*.o'))

MAIN_EXECUTABLE=assembler

.PHONY: all

all: $(MAIN_EXECUTABLE)

$(MAIN_EXECUTABLE): $(OBJECTS) $(EXTERNAL_OBJECTS)
	$(CC) $(CFLAGS) $(shell pkg-config --cflags --libs $(LIBS)) $^ -o $@

$(OBJECTS): $(SOURCES)
	$(CC) $(CFLAGS) $(INCLUDE_PARAMS) $< -c -o $@

clean:
	rm -f $(MAIN_EXECUTABLE) $(OBJECTS)
