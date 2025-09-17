SOURCES=$(shell find . -type f -iname '*.cpp')

OBJECTS+= $(foreach x, $(basename $(SOURCES)), $(x).o)

.PHONY: all

all: $(OBJECTS)

%.o: %.cpp
	$(CC) $(CFLAGS) $(INCLUDE_PARAMS) $< -c -o $@

clean:
	rm -f $(OBJECTS)
