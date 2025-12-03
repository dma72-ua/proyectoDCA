# Engtregable 2: Flatpak

El fichero dca.juego.plataformas.yml es el manifiesto que usa flatpak-builder para empaquetar el juego. Tiene las dependencias y crea la estructura del paquete.

## Instalar y ejecutar el paquete Flatpak

Para instalarlo ejecutar (En el directorio donde está dca.juego.plataformas.flatpak):

`flatpak install dca.juego.plataformas.flatpak`

Para ejecutar el juego usar:

`flatpak run dca.juego.plataformas`


## Crear el paquete Flatpak

Para crear el paquete ejecutar:

`flatpak-builder build-dir dca.juego.plataformas.yml --force-clean --install --user`

Para empaquetarlo en .flatpak y poder compartirlo ejecutar:

`flatpak build-bundle ~/.local/share/flatpak/repo dca.juego.plataformas.flatpak dca.juego.plataformas`
