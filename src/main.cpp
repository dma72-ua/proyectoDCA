#include "player.h"

int main(void) {
  const int screenWidth = 800;
  const int screenHeight = 450;

  InitWindow(screenWidth, screenHeight, "Proyecto DCA");

  Player player = {{400, 280}, 0, false};

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
