# Makefile for WiFiSimulator project

# Compiler and flags
CXX = g++
CXXFLAGS = -std=c++17 -Wall -O2

# Project settings
SRC = wifi5.cpp           # The source file (your wifi5.cpp)
OBJ = wifi5.o             # Object file generated from the source
EXEC = wifi5      # The final executable

# The default target to build the executable
all: $(EXEC)

# Rule to link object files and create the executable
$(EXEC): $(OBJ)
	$(CXX) $(OBJ) -o $(EXEC)

# Rule to compile the source code into an object file
$(OBJ): $(SRC)
	$(CXX) $(CXXFLAGS) -c $(SRC)

# Clean up compiled files
clean:
	rm -f $(OBJ) $(EXEC)

# To run the program (Optional, can be called after make)
run: $(EXEC)
	./$(EXEC)

# To run the clean target (Optional)
rebuild: clean all

