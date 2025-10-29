#!/bin/bash

# Compilar el juego
echo "Compilando el juego..."
g++ -o game src/*.cpp -I src/ -I vendor/include/ -L vendor/lib -lraylib -lGL -lm -lpthread -lrt -lX11

# Verificar si la compilación fue exitosa
if [ $? -eq 0 ]; then
  echo "Compilación exitosa!"
  echo "Ejecutando el juego..."
  ./game
else
  echo "Error en la compilación."
  exit 1
fi
