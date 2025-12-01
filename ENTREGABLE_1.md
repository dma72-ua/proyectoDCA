# Entregable 1: Makefiles y ccache

## 1. Tiempos de ejecución con -jN

Hemos pruebas de compilación limpia (`make clean && time make -jN all`) con los siguientes resultados:

| N (Trabajos) | Tiempo Real |
|--------------|-------------|
| 1            | 0.304s      |
| 2            | 0.273s      |
| 4            | 0.246s      |
| 8            | 0.245s      |

**Conclusión:**
La mejora sustancial se observa hasta **N=4**. A partir de este valor (N=8), el tiempo de compilación se estabiliza y no hay una mejora significativa (apenas 0.001s de diferencia). Esto se debe probablemente a que el número de archivos a compilar (6 archivos fuente) es pequeño, y la sobrecarga de gestionar más hilos supera el beneficio de la paralelización adicional, o simplemente se ha alcanzado el límite de núcleos físicos disponibles.

## 2. Fallos de compilación en paralelo

**¿Hay fallos de compilación con ejecuciones en paralelo?**
No, no se han producido fallos de compilación durante las pruebas con `-j2`, `-j4` ni `-j8`.

**Análisis:**
Los fallos en compilación paralela suelen ocurrir cuando hay dependencias incorrectamente definidas en el Makefile. En nuestro caso, el Makefile utiliza un sistema de dependencias automático (`-MMD -MP`) y reglas de patrón bien definidas que aseguran que cada archivo objeto `.o` se compile independientemente antes de la fase de enlazado final. Como los archivos fuente no dependen unos de otros durante la fase de compilación (solo de sus cabeceras), la paralelización es segura.

## 3. Comparativa con ccache

Se ha comparado el tiempo de compilación completa (`make clean && make all`) en tres escenarios (usando `-j4`):

| Escenario             | Tiempo Real | Descripción                                   |
|-----------------------|-------------|-----------------------------------------------|
| **Sin ccache**        | 1.529s      | Compilación estándar con `g++`.               |
| **ccache (frío)**     | 1.538s      | Primera ejecución con `ccache` (cache vacía). |
| **ccache (caliente)** | 0.192s      | Segunda ejecución (cache hit).                |

**Conclusión:**
El uso de `ccache` reduce el tiempo de recompilación limpia, pasando de **~1.5s** a **~0.2s**. Esto supone una mejora de velocidad de casi **8x** en este proyecto. La sobrecarga inicial (cache fría) es despreciable (0.009s).

## 4. Estadísticas de ccache

Tras limpiar la cache y resetear estadísticas (`ccache -z -C`) y ejecutar dos compilaciones completas consecutivas, las estadísticas son:

```text
Cacheable calls:     12 /  14 (85.71%)
  Hits:               6 /  12 (50.00%)
    Direct:           6 /   6 (100.0%)
    Preprocessed:     0 /   6 ( 0.00%)
  Misses:             6 /  12 (50.00%)
Uncacheable calls:    2 /  14 (14.29%)
Local storage:
  Cache size (GiB): 0.0 / 5.0 ( 0.00%)
  Hits:               6 /  12 (50.00%)
  Misses:             6 /  12 (50.00%)
```

**Análisis:**
- **Misses (6):** Corresponden a la primera compilación de los 6 archivos fuente (`enemy.cpp`, `coin.cpp`, `player.cpp`, `textureManager.cpp`, `main.cpp`, `levelManager.cpp`).
- **Hits (6):** Corresponden a la segunda compilación, donde `ccache` detectó que los archivos no habían cambiado y reutilizó los objetos cacheados.
- **Tasa de acierto (50%):** Es el resultado esperado al ejecutar una vez en frío (0% aciertos) y una vez en caliente (100% aciertos) sobre el mismo conjunto de archivos.
