# Proyecto DCA

Juego 2D tipo Mario Bros.

GitHub Flow para gestión de ramas y GitHub Issues como sistema de Bugtracking.

## Compilación y ejecutación del Juego

Desde el directorio raíz ejecutar:

`g++ -o game src/*.cpp -I src/ -I vendor/include/ -L vendor/lib -lraylib -lGL -lm -lpthread -lrt -lX11`

y luego:

`./game`
