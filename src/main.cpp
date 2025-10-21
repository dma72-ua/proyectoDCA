#include "envItem.h"
#include "player.h"
#include <vector>

void updateCameraCenterInsideMap(Camera2D &camera, Player &player,
                                 std::vector<EnvItem> &envItems, float delta);

int main(void) {
  const int screenWidth = 800;
  const int screenHeight = 450;
  bool victory = false;

  InitWindow(screenWidth, screenHeight, "Proyecto DCA");

  Player player = {{200, 280}, 0, false};

  std::vector<EnvItem> envItems = {
      {{0, -200, 3500, 700}, 0, SKYBLUE},
      {{0, 400, 3500, 100}, 1, DARKGREEN},

      {{300, 340, 100, 20}, 1, DARKBROWN},
      {{450, 280, 120, 20}, 1, DARKBROWN},
      {{620, 220, 100, 20}, 1, DARKBROWN},
      {{750, 280, 80, 20}, 1, DARKBROWN},
      {{870, 340, 100, 20}, 1, DARKBROWN},

      {{1000, 340, 90, 20}, 1, DARKBROWN},
      {{1120, 290, 90, 20}, 1, DARKBROWN},
      {{1240, 240, 90, 20}, 1, DARKBROWN},
      {{1360, 190, 90, 20}, 1, DARKBROWN},
      {{1480, 140, 90, 20}, 1, DARKBROWN},

      {{1600, 200, 70, 15}, 1, GRAY},
      {{1720, 200, 70, 15}, 1, GRAY},
      {{1840, 200, 70, 15}, 1, GRAY},
      {{1960, 150, 70, 15}, 1, GRAY},
      {{2080, 100, 70, 15}, 1, GRAY},

      {{2180, 150, 80, 15}, 1, GRAY},
      {{2290, 200, 80, 15}, 1, GRAY},
      {{2400, 250, 80, 15}, 1, GRAY},
      {{2510, 300, 80, 15}, 1, GRAY},

      {{2630, 220, 70, 15}, 1, BROWN},

      {{2730, 200, 80, 20}, 1, BROWN},
      {{2900, 200, 80, 20}, 1, BROWN}, // Gap de 90 unidades

      {{3020, 160, 70, 15}, 1, ORANGE},
      {{3020, 110, 70, 15}, 1, ORANGE},

      {{3120, 60, 200, 20}, 1, GOLD}};

  Camera2D camera = {
      {screenWidth / 2.0f, screenHeight / 2.0f}, player.position, 0.0f, 1.0f};

  SetTargetFPS(60);
  while (!WindowShouldClose()) {

    float deltaTime = GetFrameTime();
    Rectangle playerRect = {player.position.x - 20, player.position.y - 40,
                            40.0f, 40.0f};

    if (!victory) {
      player.updatePlayer(deltaTime, envItems);
      updateCameraCenterInsideMap(camera, player, envItems, deltaTime);

      Rectangle victoryPlatform = envItems.back().rect;
      bool isAbovePlatform =
          playerRect.y + playerRect.height >= victoryPlatform.y &&
          playerRect.y + playerRect.height <= victoryPlatform.y + 20;
      bool isWithinXBounds =
          playerRect.x + playerRect.width > victoryPlatform.x &&
          playerRect.x < victoryPlatform.x + victoryPlatform.width;

      if (isAbovePlatform && isWithinXBounds)
        victory = true;

    } else {
      if (IsKeyPressed(KEY_SPACE)) {
        player.position = {200, 280};
        victory = false;
      }
    }

    BeginDrawing();

    ClearBackground(LIGHTGRAY);

    BeginMode2D(camera);

    for (auto &envItem : envItems)
      DrawRectangleRec(envItem.rect, envItem.color);

    DrawRectangleRec(playerRect, BLUE);

    EndMode2D();

    if (victory) {
      DrawRectangle(0, 0, screenWidth, screenHeight, Fade(BLACK, 0.7f));

      const char *victoryText = "¡VICTORIA!";
      int textWidth = MeasureText(victoryText, 60);
      DrawText(victoryText, screenWidth / 2 - textWidth / 2,
               screenHeight / 2 - 60, 60, GOLD);

      const char *restartText = "Presiona ESPACIO para jugar de nuevo";
      int restartWidth = MeasureText(restartText, 20);
      DrawText(restartText, screenWidth / 2 - restartWidth / 2,
               screenHeight / 2 + 20, 20, WHITE);
    }

    EndDrawing();
  }

  CloseWindow();

  return 0;
}

void updateCameraCenterInsideMap(Camera2D &camera, Player &player,
                                 std::vector<EnvItem> &envItems, float delta) {
  float width = GetScreenWidth();
  float height = GetScreenHeight();

  float minX = 1000, minY = 1000, maxX = -1000, maxY = -1000;
  for (auto &envItem : envItems) {
    minX = std::min(envItem.rect.x, minX);
    maxX = std::max(envItem.rect.x + envItem.rect.width, maxX);
    minY = std::min(envItem.rect.y, minY);
    maxY = std::max(envItem.rect.y + envItem.rect.height, maxY);
  }

  camera.target = player.position;
  camera.offset = (Vector2){width / 2.0f, height / 2.0f};

  float camMinX = camera.target.x - camera.offset.x / camera.zoom;
  float camMaxX = camera.target.x + camera.offset.x / camera.zoom;
  float camMinY = camera.target.y - camera.offset.y / camera.zoom;
  float camMaxY = camera.target.y + camera.offset.y / camera.zoom;

  if (camMinX < minX) {
    camera.target.x = minX + camera.offset.x / camera.zoom;
  }
  if (camMaxX > maxX) {
    camera.target.x = maxX - camera.offset.x / camera.zoom;
  }

  if (camMinY < minY) {
    camera.target.y = minY + camera.offset.y / camera.zoom;
  }
  if (camMaxY > maxY) {
    camera.target.y = maxY - camera.offset.y / camera.zoom;
  }
}