SOURCES=$(shell find . -type f -iname '*.cpp')

OBJECTS+= $(foreach x, $(basename $(SOURCES)), $(x).o)
EXTERNAL_OBJECTS=$(foreach x, $(abspath $(addprefix ../, $(LIB_SUBDIRS))), $(shell find $(x) -type f -iname '*.o'))


TEST_EXECUTABLE=run_tests

.PHONY: all

all: $(OBJECTS) $(TEST_EXECUTABLE)

%.o: %.cpp
	$(CC) $(CFLAGS) $(INCLUDE_PARAMS) $^ -c -o $@

$(TEST_EXECUTABLE): $(OBJECTS) $(EXTERNAL_OBJECTS)
	$(CC) $(CFLAGS) $^ -o $@

clean:
	rm -f $(TEST_EXECUTABLE) $(OBJECTS)
