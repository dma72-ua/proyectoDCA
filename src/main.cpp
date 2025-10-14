#include "envItem.h"
#include "player.h"
#include <iterator>
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

  int envItemsLength = sizeof(envItems) / sizeof(envItems[0]);

  while (!WindowShouldClose()) {

    float deltaTime = GetFrameTime();

    player.updatePlayer(deltaTime);

    BeginDrawing();

    ClearBackground(LIGHTGRAY);

    Rectangle playerRect = {player.position.x - 20, player.position.y - 40,
                            40.0f, 40.0f};
    DrawRectangleRec(playerRect, BLUE);

    EndDrawing();
  }

  CloseWindow();

  return 0;
}
