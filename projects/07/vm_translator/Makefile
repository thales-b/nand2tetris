CC = g++
CFLAGS = -std=c++17 -g -Wall 

SOURCES = parser.cpp code_writer.cpp main.cpp
OBJECTS = $(SOURCES:.cpp=.o)
EXECUTABLE = vm_translator

.PHONY: all clean

all: $(EXECUTABLE)

$(EXECUTABLE): $(OBJECTS)
	$(CC) $(CFLAGS) $(OBJECTS) -o $(EXECUTABLE)

.cpp.o:
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -f $(EXECUTABLE) $(OBJECTS)
