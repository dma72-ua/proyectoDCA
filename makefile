CXX := g++
CXXFLAGS := -std=c++17 -I/usr/local/include -Isrc
LDFLAGS := -L/usr/local/lib -lraylib -lGL -lm -lpthread -ldl -lrt -lX11

SRC := src/main.cpp src/player.cpp src/enemy.cpp
OBJ := $(SRC:.cpp=.o)
BIN := build/juego

.RECIPEPREFIX := >

all: $(BIN)

$(BIN): $(OBJ)
> mkdir -p $(dir $@)
> $(CXX) $^ -o $@ $(LDFLAGS)

%.o: %.cpp
> $(CXX) $(CXXFLAGS) -c $< -o $@

run: $(BIN)
> ./$(BIN)

clean:
> rm -f $(OBJ) $(BIN)

.PHONY: all clean run
