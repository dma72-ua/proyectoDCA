extern "C" {
#include "../vendor/include/raylib.h"
}

int main(void) {
  const int screenWidth = 800;
  const int screenHeight = 450;

  InitWindow(screenWidth, screenHeight, "Proyecto DCA");

  while (!WindowShouldClose()) {
    BeginDrawing();

    ClearBackground(LIGHTGRAY);

    EndDrawing();
  }

  CloseWindow();

  return 0;
}
