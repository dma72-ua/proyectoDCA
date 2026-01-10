# Engtregable CPack

En el fichero CMakeLists.txt se ha añadido un apartado que empaqueta mediante CPack instalables **RPM** y **DEB**

## Generación de los paquetes

`cmake -B build && cd build && cpack`

Esto genera dos paquetes: un .deb y un .rpm, usa el apropiado para tu sistema



