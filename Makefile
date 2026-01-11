CXX := g++
CXXFLAGS := -std=c++17 -Wall -MMD -MP
LDFLAGS := -L vendor/lib
LIBS := -lraylib -lGL -lm -lpthread -lrt -lX11 -ldl

# Directories
SRC_DIR := src
BUILD_DIR := build
VENDOR_INCLUDE := vendor/include
INCLUDES := -I $(SRC_DIR) -I $(VENDOR_INCLUDE)
TARGET := proyectoDCA
SOURCES := $(shell find $(SRC_DIR) -name '*.cpp')
OBJECTS := $(SOURCES:$(SRC_DIR)/%.cpp=$(BUILD_DIR)/%.o)
DEPS := $(OBJECTS:.o=.d)

.PHONY: all
all: $(TARGET)

$(TARGET): $(OBJECTS)
	@echo "Linking $@..."
	$(CXX) -o $@ $^ $(LDFLAGS) $(LIBS)
	@echo "Build complete: $(TARGET)"

$(BUILD_DIR)/%.o: $(SRC_DIR)/%.cpp
	@mkdir -p $(dir $@)
	@echo "Compiling $<..."
	$(CXX) $(CXXFLAGS) $(INCLUDES) -c $< -o $@

.PHONY: clean
clean:
	@echo "Cleaning build artifacts..."
	rm -rf $(BUILD_DIR) $(TARGET)
	@echo "Clean complete."

.PHONY: run
run: all
	@echo "Running $(TARGET)..."
	./$(TARGET)

.PHONY: install
install: all
	install -Dm755 $(TARGET) /app/bin/$(TARGET)

-include $(DEPS)