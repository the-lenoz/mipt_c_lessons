SOURCES=$(shell find . -type f -iname '*.cpp')

OBJECTS+= $(foreach x, $(basename $(SOURCES)), $(x).o)

TEST_EXECUTABLE=run_tests

.PHONY: all

all: $(OBJECTS) $(TEST_EXECUTABLE)

%.o: %.cpp
	$(CC) $(CFLAGS) $(INCLUDE_PARAMS) $^ -c -o $@

$(TEST_EXECUTABLE): $(OBJECTS)
	$(CC) $(CFLAGS) $^ -o $@

clean:
	rm -f $(TEST_EXECUTABLE) $(OBJECTS)
