# Engtregable CPack

En el fichero CMakeLists.txt se ha añadido un apartado que empaqueta mediante CPack instalables **RPM** y **DEB**

# Generación de los paquetes

## Linux

`cmake -B build && cd build && cpack`

Esto genera dos paquetes: un .deb y un .rpm, usa el apropiado para tu sistema

Puedes instalar el .deb (Ubuntu/Debian/Mint):

``sudo apt install ./<nombre-del-paquete>.deb`

Puedes instalar el .rpm (Fedora/Red Hat):

``sudo dnf install ./<nombre-del-paquete>.rpm`

Luego puedes correrlo desde la terminal simplemente poniendo desde cualquier directorio:

`proyectoDCA`

## Windows

`mkdir build && cd build`

`cmake .. && cmake --build . --config Release`

`cpack`

Ahora puedes ejecutar el **proyectoDCA-1.0.0-win64.exe** y después de seguir los pasos ya tendrás el juego instalado en tu sistema. Para jugar puedes buscarlo en el menu de inicio pulsando la tecla windows y escribiendo:

`proyectoDCA`


