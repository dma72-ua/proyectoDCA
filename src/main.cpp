#include "envItem.h"
#include "player.h"
#include <vector>

int main(void) {
  const int screenWidth = 800;
  const int screenHeight = 450;

  InitWindow(screenWidth, screenHeight, "Proyecto DCA");

  Player player = {{400, 280}, 0, false};
  std::vector<EnvItem> envItems = {{{0, 0, 1000, 400}, 0, LIGHTGRAY},
                                   {{0, 400, 1000, 200}, 1, GRAY},
                                   {{300, 200, 400, 10}, 1, GRAY},
                                   {{250, 300, 100, 10}, 1, GRAY},
                                   {{650, 300, 100, 10}, 1, GRAY}};

  Camera2D camera = {
      {screenWidth / 2.0f, screenHeight / 2.0f}, player.position, 0.0f, 1.0f};

  SetTargetFPS(60);
  while (!WindowShouldClose()) {

    float deltaTime = GetFrameTime();

    player.updatePlayer(deltaTime, envItems);

    BeginDrawing();

    ClearBackground(LIGHTGRAY);

    for (auto &envItem : envItems)
      DrawRectangleRec(envItem.rect, envItem.color);

    Rectangle playerRect = {player.position.x - 20, player.position.y - 40,
                            40.0f, 40.0f};
    DrawRectangleRec(playerRect, BLUE);
    EndDrawing();
  }

  CloseWindow();

  return 0;
}

void UpdateCameraCenterInsideMap(Camera2D &camera, Player &player,
                                 std::vector<EnvItem> &envItems, float delta) {
  float width = GetScreenWidth();
  float height = GetScreenHeight();

  camera.target = player.position;
  camera.offset = (Vector2){GetScreenWidth() / 2.0f, GetScreenHeight() / 2.0f};
  float minX = 1000, minY = 1000, maxX = -1000, maxY = -1000;

  for (auto &envItem : envItems) {
    minX = std::min(envItem.rect.x, minX);
    maxX = std::max(envItem.rect.x + envItem.rect.width, maxX);
    minY = std::min(envItem.rect.y, minY);
    maxY = std::max(envItem.rect.y + envItem.rect.height, maxY);
  }

  Vector2 max = GetWorldToScreen2D((Vector2){maxX, maxY}, camera);
  Vector2 min = GetWorldToScreen2D((Vector2){minX, minY}, camera);

  if (max.x < width)
    camera.offset.x = width - (max.x - width / 2);
  if (max.y < height)
    camera.offset.y = height - (max.y - height / 2);
  if (min.x > 0)
    camera.offset.x = width / 2 - min.x;
  if (min.y > 0)
    camera.offset.y = height / 2 - min.y;
}
