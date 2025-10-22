# **Proyecto DCA — Videojuego 2D en C++ con raylib**

**Asignatura:** Desarrollo Colaborativo de Aplicaciones  
**Repositorio:** `proyectoDCA`  
**Motor/Librerías:** C++ + raylib  
**Sistemas objetivo:** Linux 

---

## **Índice**
1. Objetivo y visión del juego
2. Organización del equipo y flujo de trabajo (GitHub Flow)
3. Gestión de incidencias y planificación (GitHub Issues)
4. Requisitos obligatorios del juego y cómo se cumplen
5. Diseño del juego: estados, mecánicas y arte

---

## **1) Objetivo y visión del juego**
Juego de **plataformas retro 2D** estilo Mario clásico: el jugador avanza por niveles, **salta** y **derrota enemigos pisándoles**, evitando morir por contacto lateral o caída. El nivel termina al alcanzar un objetivo. En esta primera iteración se priorizan **mecánicas básicas**, **control pulido** y **feedback claro** al jugador.

**Pilares del diseño**
- Mecánica central clara y divertida (saltar/evitar/derrotar).
- Dificultad creciente por diseño de plataformas y colocación de enemigos.
- Estética retro legible y consistente.

---

## **2) Organización del equipo y flujo de trabajo (GitHub Flow)**
**GitHub Flow** es un flujo sencillo basado en rama principal estable y ramas de funcionalidad cortas:

**Ramas**
- `main`: rama estable, siempre desplegable. Etiquetada con `vX.Y.Z` en releases.
- `feature/<nombre>`: cada nueva funcionalidad o tarea. Vida corta.
- `fix/<bug>`: correcciones específicas.

**Reglas**
- Todo cambio a `main` se hace mediante **Pull Request** (PR).
- Branch protegida: exigir al menos 1 **code review** y CI verde antes de poder fusionar.
- Commits atómicos y descriptivos. Convención: `feat: …`, `fix: …`, `docs: …`, `test: …`, `refactor: …`, `chore: …`.

**Rituales y comunicación**
- Reunión breve semanal : avance, bloqueos, decisiones.
- El rol de revisión rota entre los miembros.

---

## **3) Gestión de incidencias y planificación (GitHub Issues)**
**Sistema de gestión:** GitHub Issues 

---

## **4) Requisitos obligatorios del juego y cómo se cumplen**
**Gestión de estados**
- **Menú principal**: iniciar partida, idioma, salir.
- **Juego**: bucle principal, HUD.
- **Game Over**: reintentar o volver al menú.
> Implementación: `StateManager` sencillo con `IState { update(dt), draw() }` y estados `Menu`, `Game`, `GameOver`.

**Gráficos (sprites + gestor de contenido)**
- Personajes, enemigos y escenario con **sprites/tiles**.
- **Gestor de contenido** para cachear `Texture2D`/`Sound` y evitar cargas duplicadas.

**Mecánica principal**
- Plataformas 2D: movimiento lateral, salto, colisión con bloques.
- Enemigos derrotables **pisando**; contacto lateral/desde abajo ⇒ **muerte/reinicio**.
- Objetivo: alcanzar meta o recoger objeto clave.



---

## **5) Diseño del juego: estados, mecánicas y arte**
**Controles**: `A/D` o `←/→` para moverse, `W` o `Espacio` para saltar.  
**Cámara**: centrada en el jugador con límites del mapa.

**Enemigos**
- Caminan y rebotan en obstáculos.
- **Stomp**: si `vy` descendente y contacto superior dentro de tolerancia, enemigo muere y el jugador rebota.
- **Daño**: contacto lateral/desde abajo ⇒ reinicio de nivel.

**Arte**
- Estética retro con tiles de ladrillo, nubes y colinas. Más adelante, sprites animados.

**Audio** (para el hito 2)
- SFX de salto, stomp y muerte; música de fondo.

---

