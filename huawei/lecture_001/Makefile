SOURCES=$(shell find . -type f -iname '*.cpp')

OBJECTS+= $(foreach x, $(basename $(SOURCES)), $(x).o)

EXTERN_OBJECTS=$(foreach x, $(basename $(LIB_SUBDIRS)), $(shell find .. -type f -iname '*.o'))

MAIN_EXECUTABLE=lecture_001

.PHONY: all

all: $(MAIN_EXECUTABLE)

$(MAIN_EXECUTABLE): $(OBJECTS) $(EXTERN_OBJECTS)
	$(CC) $(CFLAGS) $^ -o $@

$(OBJECTS): $(SOURCES)
	$(CC) $(CFLAGS) $(INCLUDE_PARAMS) $< -c -o $@

clean:
	rm -f $(MAIN_EXECUTABLE) $(OBJECTS)
