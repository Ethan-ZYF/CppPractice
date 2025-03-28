# Compiler
CXX := g++-14
CXXFLAGS := -std=c++11 -Wall -Wextra -pedantic -O2

# Target executable name
TARGET := COW

# Source file
SRC := COW.cpp

# Object file (derived from SRC)
OBJ := $(SRC:.cpp=.o)

# Default target
all: $(TARGET)

# Link object files to create the executable
$(TARGET): $(OBJ)
	$(CXX) $(CXXFLAGS) -o $@ $^

# Compile source file into object file
%.o: %.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

# Clean build artifacts
clean:
	rm -f $(TARGET) $(OBJ)

.PHONY: all clean