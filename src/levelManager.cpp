#include "levelManager.h"

LevelManager::LevelManager() {
  initializeLevels();
  currentLevelIndex = 0;
}

bool LevelManager::advanceLevel() {
  if (currentLevelIndex < levels.size() + 1) {
    currentLevelIndex++;
    return true;
  }
  return false;
}

void LevelManager::initializeLevels() {
  // LEVEL 1 - Tutorial/Easy
  levels.push_back({
      "Nivel 1 - Primeros Pasos",
      {60, 400},           // playerStart
      {1700, 400, 8, 160}, // goal
      {
          // envItems
          {{-4000, 400, 8000, 200}, 1, (Color){191, 111, 60, 255}},
          {{200, 340, 160, 20}, 1, (Color){191, 111, 60, 255}},
          {{500, 300, 160, 20}, 1, (Color){191, 111, 60, 255}},
          {{820, 260, 120, 20}, 1, (Color){191, 111, 60, 255}},
          {{1100, 360, 48, 40}, 1, (Color){53, 148, 61, 255}},
      },
      {
          // enemySpawns
          {{420, 0}, -1},
          {{860, 0}, +1},
          {{1300, 0}, -1},
      },
      {138, 197, 255, 255}, // skyColor

      // MONEDAS NIVEL 1 - En plataformas y rutas principales
      {
        {240, 300},   // Primera plataforma
        {540, 260},   // Segunda plataforma  
        {860, 220},   // Tercera plataforma
        {1120, 320},  // Antes del primer tubo
        {1350, 360},  // Entre enemigos
        {1500, 360},  // Camino al objetivo
      }
  });

  // LEVEL 2 - Intermediate
  levels.push_back({
      "Nivel 2 - El Desafío",
      {60, 300},           // playerStart
      {2200, 400, 8, 160}, // goal
      {
          // envItems
          {{-4000, 400, 8000, 200}, 1, (Color){191, 111, 60, 255}},
          {{150, 340, 120, 20}, 1, (Color){191, 111, 60, 255}},
          {{350, 280, 100, 20}, 1, (Color){191, 111, 60, 255}},
          {{550, 220, 140, 20}, 1, (Color){191, 111, 60, 255}},
          {{800, 280, 100, 20}, 1, (Color){191, 111, 60, 255}},
          {{1000, 340, 120, 20}, 1, (Color){191, 111, 60, 255}},
          {{1200, 280, 100, 20}, 1, (Color){191, 111, 60, 255}},
          {{1450, 360, 48, 40}, 1, (Color){53, 148, 61, 255}},
          {{1700, 320, 80, 20}, 1, (Color){191, 111, 60, 255}},
          {{1900, 360, 48, 40}, 1, (Color){53, 148, 61, 255}},
      },
      {
          // enemySpawns
          {{300, 0}, +1},
          {{650, 0}, -1},
          {{900, 0}, +1},
          {{1150, 0}, -1},
          {{1600, 0}, +1},
      },
      {120, 180, 255, 255}, // skyColor - slightly different

      {
        {190, 300},   // Primera plataforma
        {400, 240},   // Plataforma alta
        {600, 180},   // Punto más alto del nivel
        {850, 240},   // Después del salto
        {1050, 300},  // Plataforma descendente
        {1250, 240},  // Antes del tubo
        {1480, 320},  // Sobre el primer tubo
        {1750, 280},  // Plataforma final
        {1920, 320},  // Último tubo antes de la meta
      }
  });

  // LEVEL 3 - Advanced
  levels.push_back({
      "Nivel 3 - Maestría",
      {60, 350},           // playerStart
      {2800, 400, 8, 160}, // goal
      {
          // envItems
          {{-4000, 400, 8000, 200}, 1, (Color){191, 111, 60, 255}},
          // Long jump section
          {{200, 340, 80, 20}, 1, (Color){191, 111, 60, 255}},
          {{380, 300, 80, 20}, 1, (Color){191, 111, 60, 255}},
          {{560, 260, 80, 20}, 1, (Color){191, 111, 60, 255}},
          // Tight platforming
          {{750, 220, 60, 20}, 1, (Color){191, 111, 60, 255}},
          {{870, 260, 60, 20}, 1, (Color){191, 111, 60, 255}},
          {{990, 220, 60, 20}, 1, (Color){191, 111, 60, 255}},
          // Pipe obstacle course
          {{1150, 360, 48, 40}, 1, (Color){53, 148, 61, 255}},
          {{1300, 340, 100, 20}, 1, (Color){191, 111, 60, 255}},
          {{1500, 360, 48, 40}, 1, (Color){53, 148, 61, 255}},
          {{1650, 300, 100, 20}, 1, (Color){191, 111, 60, 255}},
          {{1850, 360, 48, 40}, 1, (Color){53, 148, 61, 255}},
          // Final stretch
          {{2000, 340, 120, 20}, 1, (Color){191, 111, 60, 255}},
          {{2200, 280, 100, 20}, 1, (Color){191, 111, 60, 255}},
          {{2400, 340, 120, 20}, 1, (Color){191, 111, 60, 255}},
      },
      {
          // enemySpawns
          {{320, 0}, -1},
          {{500, 0}, +1},
          {{810, 0}, -1},
          {{1000, 0}, +1},
          {{1250, 0}, -1},
          {{1600, 0}, +1},
          {{2100, 0}, -1},
          {{2300, 0}, +1},
      },
      {100, 150, 240, 255}, // skyColor - darker for difficulty

          // MONEDAS NIVEL 3 - En lugares que requieren precisión
      {
        {240, 300},   // Primera plataforma
        {420, 260},   // Segunda plataforma
        {600, 220},   // Tercera plataforma
        {780, 180},   // Plataforma pequeña alta
        {900, 220},   // Plataforma descendente
        {1020, 180},  // Plataforma pequeña final
        {1180, 320},  // Antes del primer tubo
        {1350, 300},  // Plataforma entre tubos
        {1520, 320},  // Sobre segundo tubo
        {1700, 260},  // Plataforma alta
        {1880, 320},  // Sobre tercer tubo
        {2060, 300},  // Inicio del final
        {2250, 240},  // Plataforma alta final
        {2460, 300},  // Antes de la meta
      }
  });
}