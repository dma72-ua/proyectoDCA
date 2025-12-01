# =============================================================================
# Generic Makefile for Multi-Directory C++ Projects with ccache
# =============================================================================
# This Makefile automatically discovers source files in subdirectories,
# compiles them with dependency tracking, and links them into an executable.
# Minimal changes needed when adding new files or directories.
# =============================================================================

# Compiler and tools
CXX := ccache g++
CXXFLAGS := -std=c++17 -Wall -MMD -MP
LDFLAGS := -L vendor/lib
LIBS := -lraylib -lGL -lm -lpthread -lrt -lX11

# Directories
SRC_DIR := src
BUILD_DIR := build
VENDOR_INCLUDE := vendor/include

# Include paths - automatically add src and vendor
INCLUDES := -I $(SRC_DIR) -I $(VENDOR_INCLUDE)

# Target executable
TARGET := game

# Automatically find all .cpp files recursively in src/
SOURCES := $(shell find $(SRC_DIR) -name '*.cpp')

# Generate object file paths in build/ mirroring src/ structure
# Example: src/core/main.cpp -> build/core/main.o
OBJECTS := $(SOURCES:$(SRC_DIR)/%.cpp=$(BUILD_DIR)/%.o)

# Generate dependency files (.d) for automatic header dependency tracking
DEPS := $(OBJECTS:.o=.d)

# =============================================================================
# Rules
# =============================================================================

# Default target: build the executable
.PHONY: all
all: $(TARGET)

# Link object files into final executable
$(TARGET): $(OBJECTS)
	@echo "Linking $@..."
	$(CXX) -o $@ $^ $(LDFLAGS) $(LIBS)
	@echo "Build complete: $(TARGET)"

# Compile .cpp files to .o files with automatic dependency generation
# Pattern rule: build/x/y.o depends on src/x/y.cpp
$(BUILD_DIR)/%.o: $(SRC_DIR)/%.cpp
	@mkdir -p $(dir $@)
	@echo "Compiling $<..."
	$(CXX) $(CXXFLAGS) $(INCLUDES) -c $< -o $@

# Clean build artifacts and executable
.PHONY: clean
clean:
	@echo "Cleaning build artifacts..."
	rm -rf $(BUILD_DIR) $(TARGET)
	@echo "Clean complete."

# Run the game (bonus target)
.PHONY: run
run: all
	@echo "Running $(TARGET)..."
	./$(TARGET)

# Include auto-generated dependency files (if they exist)
# This enables automatic recompilation when headers change
-include $(DEPS)

# =============================================================================
# Help target (bonus - shows available commands)
# =============================================================================
.PHONY: help
help:
	@echo "Available targets:"
	@echo "  all     - Build the project (default)"
	@echo "  clean   - Remove all build artifacts"
	@echo "  run     - Build and run the game"
	@echo "  help    - Show this help message"
	@echo ""
	@echo "Build with parallel jobs: make -j4 all"
