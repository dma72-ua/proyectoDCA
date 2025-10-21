#include "envItem.h"
#include "player.h"
#include <vector>

void updateCameraCenterInsideMap(Camera2D &camera, Player &player,
                                 std::vector<EnvItem> &envItems, float delta);

int main(void) {
  const int screenWidth = 800;
  const int screenHeight = 450;

  InitWindow(screenWidth, screenHeight, "Proyecto DCA");

  Player player = {{200, 280}, 0, false};

  std::vector<EnvItem> envItems = {
      // Fondo y suelo principal
      {{0, -200, 3500, 700}, 0, SKYBLUE},
      {{0, 400, 3500, 100}, 1, DARKGREEN},

      // === ZONA 1: TUTORIAL (Saltos básicos) ===
      {{300, 340, 100, 20}, 1, DARKBROWN},
      {{450, 280, 120, 20}, 1, DARKBROWN},
      {{620, 220, 100, 20}, 1, DARKBROWN},
      {{750, 280, 80, 20}, 1, DARKBROWN},
      {{870, 340, 100, 20}, 1, DARKBROWN},

      // === ZONA 2: ESCALERA ASCENDENTE ===
      {{1000, 340, 90, 20}, 1, DARKBROWN},
      {{1120, 290, 90, 20}, 1, DARKBROWN},
      {{1240, 240, 90, 20}, 1, DARKBROWN},
      {{1360, 190, 90, 20}, 1, DARKBROWN},
      {{1480, 140, 90, 20}, 1, DARKBROWN},

      // === ZONA 3: PLATAFORMAS FLOTANTES (ritmo) ===
      {{1600, 200, 70, 15}, 1, GRAY},
      {{1720, 200, 70, 15}, 1, GRAY},
      {{1840, 200, 70, 15}, 1, GRAY},
      {{1960, 150, 70, 15}, 1, GRAY},
      {{2080, 100, 70, 15}, 1, GRAY},

      // === ZONA 4: ZIGZAG DESCENDENTE ===
      {{2180, 150, 80, 15}, 1, GRAY},
      {{2290, 200, 80, 15}, 1, GRAY},
      {{2400, 250, 80, 15}, 1, GRAY},
      {{2510, 300, 80, 15}, 1, GRAY},

      // === ZONA 5: DESAFÍO VERTICAL (mini torre) ===
      {{2630, 220, 70, 15}, 1, BROWN},

      // === ZONA 6: SALTO DE FE (gap grande) ===
      {{2730, 200, 80, 20}, 1, BROWN},
      {{2900, 200, 80, 20}, 1, BROWN}, // Gap de 90 unidades

      // === ZONA 7: TORRE FINAL (ascenso épico) ===
      {{3020, 160, 70, 15}, 1, ORANGE},
      {{3020, 110, 70, 15}, 1, ORANGE},

      // === PLATAFORMA DE VICTORIA ===
      {{3120, 60, 200, 20}, 1, GOLD}};

  Camera2D camera = {
      {screenWidth / 2.0f, screenHeight / 2.0f}, player.position, 0.0f, 1.0f};

  SetTargetFPS(60);
  while (!WindowShouldClose()) {

    float deltaTime = GetFrameTime();

    player.updatePlayer(deltaTime, envItems);
    updateCameraCenterInsideMap(camera, player, envItems, deltaTime);

    BeginDrawing();

    ClearBackground(LIGHTGRAY);

    BeginMode2D(camera);

    for (auto &envItem : envItems)
      DrawRectangleRec(envItem.rect, envItem.color);

    Rectangle playerRect = {player.position.x - 20, player.position.y - 40,
                            40.0f, 40.0f};
    DrawRectangleRec(playerRect, BLUE);

    EndMode2D();

    EndDrawing();
  }

  CloseWindow();

  return 0;
}

void updateCameraCenterInsideMap(Camera2D &camera, Player &player,
                                 std::vector<EnvItem> &envItems, float delta) {
  float width = GetScreenWidth();
  float height = GetScreenHeight();

  // Calcular límites del mapa
  float minX = 1000, minY = 1000, maxX = -1000, maxY = -1000;
  for (auto &envItem : envItems) {
    minX = std::min(envItem.rect.x, minX);
    maxX = std::max(envItem.rect.x + envItem.rect.width, maxX);
    minY = std::min(envItem.rect.y, minY);
    maxY = std::max(envItem.rect.y + envItem.rect.height, maxY);
  }

  // La cámara sigue al jugador
  camera.target = player.position;
  camera.offset = (Vector2){width / 2.0f, height / 2.0f};

  // Calcular los límites de la cámara en coordenadas del mundo
  float camMinX = camera.target.x - camera.offset.x / camera.zoom;
  float camMaxX = camera.target.x + camera.offset.x / camera.zoom;
  float camMinY = camera.target.y - camera.offset.y / camera.zoom;
  float camMaxY = camera.target.y + camera.offset.y / camera.zoom;

  // Ajustar horizontalmente
  if (camMinX < minX) {
    camera.target.x = minX + camera.offset.x / camera.zoom;
  }
  if (camMaxX > maxX) {
    camera.target.x = maxX - camera.offset.x / camera.zoom;
  }

  // Ajustar verticalmente
  if (camMinY < minY) {
    camera.target.y = minY + camera.offset.y / camera.zoom;
  }
  if (camMaxY > maxY) {
    camera.target.y = maxY - camera.offset.y / camera.zoom;
  }
}