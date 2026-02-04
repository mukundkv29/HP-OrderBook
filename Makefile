# Makefile for Stock Market Order Book
# Alternative to CMake for quick compilation

CXX := g++
CXXFLAGS := -std=c++17 -Wall -Wextra -Wpedantic
LDFLAGS := -lpthread

# Release build flags (production)
RELEASE_FLAGS := -O3 -DNDEBUG -march=native -flto -ffast-math -funroll-loops

# Debug build flags
DEBUG_FLAGS := -g -O0 -DDEBUG

# Default to release build
BUILD_TYPE ?= release

ifeq ($(BUILD_TYPE),debug)
    CXXFLAGS += $(DEBUG_FLAGS)
else
    CXXFLAGS += $(RELEASE_FLAGS)
endif

# Source files
SOURCES := OrderBook.cpp MatchingEngine.cpp CLI.cpp main.cpp
OBJECTS := $(SOURCES:.cpp=.o)
TARGET := matching_engine

# Header dependencies
HEADERS := Order.h OrderBook.h MatchingEngine.h CLI.h

.PHONY: all clean run debug release help

all: $(TARGET)

# Build executable
$(TARGET): $(OBJECTS)
	@echo "🔗 Linking..."
	$(CXX) $(CXXFLAGS) -o $@ $^ $(LDFLAGS)
	@echo "✅ Build complete: $(TARGET)"

# Compile source files
%.o: %.cpp $(HEADERS)
	@echo "🔨 Compiling $<..."
	$(CXX) $(CXXFLAGS) -c $< -o $@

# Run the program
run: $(TARGET)
	@echo "🚀 Running matching engine..."
	@./$(TARGET)

# Debug build
debug:
	@$(MAKE) BUILD_TYPE=debug all

# Release build (explicit)
release:
	@$(MAKE) BUILD_TYPE=release all

# Clean build artifacts
clean:
	@echo "🧹 Cleaning..."
	@rm -f $(OBJECTS) $(TARGET)
	@echo "✅ Clean complete"

# Performance profiling (requires perf tools)
profile: release
	@echo "📊 Running with profiling..."
	@perf record -g ./$(TARGET)
	@perf report

# Help message
help:
	@echo "Stock Market Order Book - Build System"
	@echo ""
	@echo "Usage:"
	@echo "  make              Build in release mode (optimized)"
	@echo "  make debug        Build in debug mode (with symbols)"
	@echo "  make run          Build and run the program"
	@echo "  make clean        Remove build artifacts"
	@echo "  make profile      Build and profile with perf"
	@echo "  make help         Show this help message"
	@echo ""
	@echo "Build types:"
	@echo "  BUILD_TYPE=release  Optimized build (-O3, -march=native)"
	@echo "  BUILD_TYPE=debug    Debug build (-g, -O0)"
