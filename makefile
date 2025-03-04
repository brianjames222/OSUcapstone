# Compiler
CXX = g++

# Compiler flags
CXXFLAGS = -std=c++20 -Wall -Wextra -pedantic

# SDL2 flags
SDL_CXXFLAGS = -I/mingw64/include/SDL2
SDL_LDFLAGS = -L/mingw64/lib -lmingw32 -lSDL2 -mconsole

# Target executable
TARGET = emulator

# Source files
SRCS = CPU.cpp main.cpp tests.cpp ROM.cpp NES.cpp Bus.cpp APU.cpp PPU.cpp

# Object files
OBJS = $(SRCS:.cpp=.o)

# Default target
all: $(TARGET)

# Link the executable w/ SDL2 (audio)
$(TARGET): $(OBJS)
	$(CXX) $(CXXFLAGS) -o $@ $^ $(SDL_LDFLAGS)

# Compile source files into object files with SDL2 includes
%.o: %.cpp
	$(CXX) $(CXXFLAGS) $(SDL_CXXFLAGS) -c $< -o $@

# Clean up build files
clean:
	rm -f $(OBJS) $(TARGET)

# Phony targets
.PHONY: all clean
