# Proyecto DCA

Juego 2D tipo Mario Bros.

GitHub Flow para gestión de ramas y GitHub Issues como sistema de Bugtracking.

### **Enlace Itch.io**

https://dma72-ua.itch.io/proyectodca

### Instrucciones entregable 1

[Entregable 1: MakeFile](ENTREGABLE_1.md)

### Instrucciones entregable 2

[Entregable 2: Flatpak](ENTREGABLE_2.md)

### Instrucciones entregable 3

[Entregable 3: CMake](ENTREGABLE_3.md)

### Instrucciones CPack

[Entregable CPack](ENTREGABLE_CPACK.md)


## Ejecución del juego

Primero darle permisos al fichero run.sh:

`chmod +x run.sh`

Luego ejecutar el script:

`./run.sh`


## Compilación y ejecutación del Juego manual

Desde el directorio raíz ejecutar:

`g++ -o game src/*.cpp -I src/ -I vendor/include/ -L vendor/lib -lraylib -lGL -lm -lpthread -lrt -lX11`

Si te error prueba este:

`g++ -std=c++17 src/*.cpp -I/usr/local/include -L/usr/local/lib   -o game  -lraylib -lGL -lm -lpthread -ldl -lrt -lX11`

y luego:

`./game`
