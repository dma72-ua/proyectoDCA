# Compilador y flags (con ccache)
CXX = ccache g++
CXXFLAGS = -I src/ -I vendor/include/
LDFLAGS = -L vendor/lib
LIBS = -lraylib -lGL -lm -lpthread -lrt -lX11

# Archivos y nombre del ejecutable
SRC = $(wildcard src/*.cpp)
BIN = game

# Regla principal
all: $(BIN)

$(BIN): $(SRC)
	$(CXX) -o $@ $^ $(CXXFLAGS) $(LDFLAGS) $(LIBS)

# Limpiar
clean:
	rm -f $(BIN)

run: all
	./game

.PHONY: all clean
