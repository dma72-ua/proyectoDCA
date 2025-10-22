# **Proyecto DCA — Videojuego 2D en C++ con raylib**

**Asignatura:** Desarrollo Colaborativo de Aplicaciones  
**Repositorio:** `proyectoDCA`  
**Motor/Librerías:** C++ + raylib  
**Sistemas objetivo:** Linux

---

## **Índice**
1. [Objetivo y visión del juego](#1-objetivo-y-visión-del-juego)  
2. [Organización del equipo y flujo de trabajo (GitHub Flow)](#2-organización-del-equipo-y-flujo-de-trabajo-github-flow)  
3. [Gestión de incidencias y planificación (GitHub Issues)](#3-gestión-de-incidencias-y-planificación-github-issues)  
4. [Requisitos obligatorios del juego y cómo se cumplen](#4-requisitos-obligatorios-del-juego-y-cómo-se-cumplen)  
5. [Diseño del juego: estados, mecánicas y arte](#5-diseño-del-juego-estados-mecánicas-y-arte)  
6. [Versionado semántico (SemVer) y releases](#6-versionado-semántico-semver-y-releases)

---

## **1) Objetivo y visión del juego**
Juego de **plataformas retro 2D** estilo Mario clásico: el jugador avanza por niveles, **salta** y **derrota enemigos pisándoles**, evitando morir por contacto lateral o caída. El nivel termina al alcanzar un objetivo. En esta primera iteración se priorizan **mecánicas básicas**, **control pulido** y **feedback claro** al jugador.

**Pilares del diseño**
- Mecánica central clara y divertida (saltar, evitar, derrotar).  
- Dificultad creciente por diseño de plataformas y colocación de enemigos.  
- Estética retro legible y consistente.

---

## **2) Organización del equipo y flujo de trabajo (GitHub Flow)**
**GitHub Flow**: rama principal estable y ramas de corta duración para cada cambio.

**Ramas**
- `main`: estable y siempre desplegable. Etiquetada con `vX.Y.Z` en releases.
- `feature/<nombre>`: nuevas funcionalidades o tareas.
- `fix/<bug>`: correcciones específicas.

**Reglas**
- Todo cambio a `main` entra mediante **Pull Request**.  
- `main` protegida: al menos un **code review** y CI en verde.  
- Commits atómicos y descriptivos. Convención: `feat: …`, `fix: …`, `docs: …`, `test: …`, `refactor: …`, `chore: …`.

**Rituales y comunicación**
- Reunión semanal breve: avance, bloqueos, decisiones.  
- Revisión rotatoria entre miembros.

---

## **3) Gestión de incidencias y planificación (GitHub Issues)**
**Sistema:** GitHub Issues  
- Etiquetas por tipo: `feat`, `bug`, `docs`, `tech-debt`, `good-first-issue`.  
- Milestones por iteración.  
- Asignación de responsables y definición de criterios de aceptación.

---

## **4) Requisitos obligatorios del juego y cómo se cumplen**
**Gestión de estados**
- **Menú principal**: iniciar partida, idioma (placeholder), salir.  
- **Juego**: bucle principal, HUD mínimo.  
- **Game Over**: reintentar o volver al menú.  
**Implementación:** gestor simple con `GameState` y funciones `update/draw` por estado.

**Gráficos**
- Personajes, enemigos y escenario con **tiles** y primitivas; preparado para sprites.  
- Gestión de recursos: carga única de `Texture2D`/`Sound` en futuras iteraciones.

**Mecánica principal**
- Plataformas 2D: movimiento lateral, salto, colisión con bloques.  
- Enemigos derrotables **pisando**; contacto lateral/desde abajo ⇒ **muerte/reinicio**.  
- Objetivo: alcanzar meta (bandera).

---

## **5) Diseño del juego: estados, mecánicas y arte**
**Controles**: `A/D` o `←/→` para moverse, `W` o `Espacio` para saltar.  
**Cámara**: centrada en el jugador, con límites de mapa.

**Estados**
- **Inicio**: pantalla con instrucciones; avanzar con `Enter` o `Espacio`.  
- **Juego**: actualización de jugador, enemigos y colisiones.  
- **Victoria**: al tocar la bandera; `Enter` regresa al menú, `R/Espacio` reintenta.  
- **Derrota**: por contacto enemigo; mismas teclas que victoria.

**Enemigos**
- Caminan y rebotan en obstáculos.  
- **Stomp**: si `vy` descendente y contacto superior dentro de tolerancia, mueren y el jugador rebota.  
- **Daño**: contacto lateral/desde abajo ⇒ reinicio/derrota.

**Arte**
- Estética retro: cielo, nubes, colinas y suelo de ladrillo.  
- Futuro: sprites animados y atlas de tiles.

**Audio** (hito 2)
- Efectos de salto, stomp y muerte; música de fondo.

---

## **6) Versionado semántico (SemVer) y releases**
Se sigue **SemVer 2.0.0**: `MAJOR.MINOR.PATCH` (`vX.Y.Z`).

**Cuándo incrementar**
- **MAJOR (X)**: cambios incompatibles hacia atrás.  
  Ejemplo: cambio de formato de niveles o API interna pública no retrocompatible.
- **MINOR (Y)**: nuevas funcionalidades compatibles.  
  Ejemplo: añadir pantalla de inicio/victoria/derrota, nuevos enemigos, nuevos niveles.
- **PATCH (Z)**: correcciones compatibles.  
  Ejemplo: arreglos de colisiones, ajuste de tolerancias de stomp, correcciones de HUD.

**Pre-releases y metadatos (opcional)**
- Pre-releases: `v1.3.0-alpha.1`, `v1.3.0-beta.2`, `v1.3.0-rc.1`.  
- Metadatos de build: `v1.3.0+build.20251022` (no afecta orden de versiones).

**Flujo de release**
1. Actualizar `CHANGELOG.md` (mover `Unreleased` a `vX.Y.Z - YYYY-MM-DD`).  
2. Commit:
   ```bash
   git add .
   git commit -m "chore(release): v1.3.0"
   ```
3. Crear tag y publicar:
   ```bash
   git tag -a v1.3.0 -m "Release v1.3.0"
   git push origin main --tags
   ```
4. Crear **GitHub Release** desde el tag `vX.Y.Z`, pegando las notas de `CHANGELOG.md`.

**Buenas prácticas**
- Mantener `main` siempre **deployable**; solo etiquetar versiones en `main`.
- Usar **Conventional Commits** (`feat`, `fix`, `chore`, …) para facilitar changelogs automáticos.
- No versionar ramas `feature/*` (la versión se decide al fusionar a `main`).


