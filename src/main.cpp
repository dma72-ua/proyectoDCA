#include "enemy.h"
#include "envItem.h"
#include "player.h"
#include <vector>

// ----------------- Estados de juego -----------------
enum class GameState { START, PLAYING, VICTORY, DEFEAT };

// ----------------- Constantes de interacción -----------------
static constexpr float STOMP_TOLERANCE = 10.0f;
static constexpr float FALLING_VY_MIN = 120.0f;

// ----------------- Fondo retro (screen space) -----------------
static void DrawRetroBackgroundScreen() {
  auto cloud = [](int x, int y, int w) {
    DrawRectangle(x, y, w, w / 2, RAYWHITE);
    DrawCircle(x + w * 0.2f, y + w * 0.5f, w * 0.25f, RAYWHITE);
    DrawCircle(x + w * 0.5f, y + w * 0.5f, w * 0.35f, RAYWHITE);
    DrawCircle(x + w * 0.8f, y + w * 0.5f, w * 0.25f, RAYWHITE);
  };
  cloud(120, 60, 120);
  cloud(420, 90, 100);
  cloud(780, 50, 130);
  DrawCircle(220, 360, 100, (Color){124, 197, 118, 255});
  DrawCircle(300, 380, 60, (Color){124, 197, 118, 255});
}

// ----------------- Suelo/Plataformas ladrillo -----------------
static void DrawBricksFloor(const Rectangle &area) {
  const int tile = 16;
  for (int y = (int)area.y; y < (int)(area.y + area.height); y += tile) {
    for (int x = (int)area.x; x < (int)(area.x + area.width); x += tile) {
      Color c = (((x / tile) + (y / tile)) % 2 == 0)
                    ? (Color){191, 111, 60, 255}
                    : (Color){173, 99, 52, 255};
      DrawRectangle(x, y, tile, tile, c);
      DrawRectangleLines(x, y, tile, tile, (Color){110, 58, 28, 255});
    }
  }
}

// ----------------- Dibujo del objetivo (bandera/palo) -----------------
static void DrawGoal(const Rectangle &goal) {
  // Palo
  DrawRectangle(goal.x, goal.y - goal.height, 8, goal.height,
                (Color){180, 180, 180, 255});
  // Bandera
  DrawRectangle(goal.x + 8, goal.y - goal.height + 12, 28, 18,
                (Color){255, 85, 85, 255});
  DrawTriangle({goal.x + 36, goal.y - goal.height + 21},
               {goal.x + 8 + 28, goal.y - goal.height + 12},
               {goal.x + 8 + 28, goal.y - goal.height + 12 + 18},
               (Color){220, 40, 40, 255});
}

int main() {
  InitWindow(960, 540, "proyectoDCA");
  SetTargetFPS(60);

  // Cámara
  Camera2D camera = {0};
  camera.offset = {480, 350};
  camera.zoom = 1.0f;

  // Escenario
  std::vector<EnvItem> envItems = {
      {{-4000, 400, 8000, 200}, 1, (Color){191, 111, 60, 255}}, // suelo
      {{200, 340, 160, 20}, 1, (Color){191, 111, 60, 255}},
      {{500, 300, 160, 20}, 1, (Color){191, 111, 60, 255}},
      {{820, 260, 120, 20}, 1, (Color){191, 111, 60, 255}},
      {{1100, 360, 48, 40}, 1, (Color){53, 148, 61, 255}}, // tubería
  };

  // Jugador
  Player player{};
  Vector2 startPos = {60, 400};
  player.reset(startPos);

  // Enemigos
  std::vector<Enemy> enemies = {
      Enemy({420, 0}, 32, 32, -1),
      Enemy({860, 0}, 32, 32, +1),
      Enemy({1300, 0}, 32, 32, -1),
  };

  // Objetivo (palo de bandera sobre el suelo)
  // El suelo superior está en y=400 => colocamos el palo en x=1700
  Rectangle goal = {1700, 400, 8, 160}; // (x, y suelo, ancho palo, alto)

  // Estados
  GameState state = GameState::START;

  auto resetLevel = [&]() {
    player.reset(startPos);
    enemies[0].reset({420, 0}, -1);
    enemies[1].reset({860, 0}, +1);
    enemies[2].reset({1300, 0}, -1);
    // cámara cerca del jugador al iniciar
    Rectangle pb = player.bounds();
    camera.target = {pb.x + pb.width * 0.5f, pb.y + pb.height * 0.5f};
  };

  resetLevel();

  while (!WindowShouldClose()) {
    float dt = GetFrameTime();

    // --------- INPUT de estado ---------
    if (state == GameState::START) {
      if (IsKeyPressed(KEY_ENTER) || IsKeyPressed(KEY_SPACE)) {
        resetLevel();
        state = GameState::PLAYING;
      }
    } else if (state == GameState::VICTORY) {
      if (IsKeyPressed(KEY_ENTER)) {
        resetLevel();
        state = GameState::START;
      }
      if (IsKeyPressed(KEY_R) || IsKeyPressed(KEY_SPACE)) {
        resetLevel();
        state = GameState::PLAYING;
      }
    } else if (state == GameState::DEFEAT) {
      if (IsKeyPressed(KEY_ENTER)) {
        resetLevel();
        state = GameState::START;
      }
      if (IsKeyPressed(KEY_R) || IsKeyPressed(KEY_SPACE)) {
        resetLevel();
        state = GameState::PLAYING;
      }
    }

    // --------- UPDATE (solo durante PLAYING) ---------
    if (state == GameState::PLAYING) {
      player.UpdatePlayer(envItems, dt);
      for (auto &e : enemies)
        e.update(dt, envItems);

      Rectangle pb = player.bounds();

      // Colisión con enemigos (stomp vs derrota)
      bool diedThisFrame = false;
      for (auto &e : enemies) {
        if (!e.alive)
          continue;
        Rectangle eb = e.bounds();
        if (CheckCollisionRecs(pb, eb)) {
          float pbBottom = pb.y + pb.height;
          float enemyTop = eb.y;
          bool falling = (player.vy() > FALLING_VY_MIN);

          bool stomp = falling && pbBottom >= enemyTop &&
                       (pbBottom - enemyTop) <= STOMP_TOLERANCE;

          if (stomp) {
            e.kill();
            player.position.y = enemyTop; // colocar pies sobre el enemigo
            player.bounce(-320.0f);
            pb = player.bounds();
          } else {
            state = GameState::DEFEAT;
            diedThisFrame = true;
            break;
          }
        }
      }

      // Victoria: tocar el objetivo (palo de la bandera)
      if (!diedThisFrame) {
        if (CheckCollisionRecs(
                pb, {goal.x - 6, goal.y - goal.height, 40, goal.height})) {
          state = GameState::VICTORY;
        }
      }

      // Cámara sigue al jugador
      camera.target = {pb.x + pb.width * 0.5f, pb.y + pb.height * 0.5f};
    }

    // --------- DRAW ---------
    BeginDrawing();
    ClearBackground((Color){138, 197, 255, 255}); // cielo

    // Fondo estático
    DrawRetroBackgroundScreen();

    // Mundo
    BeginMode2D(camera);
    for (const auto &e : envItems) {
      if (e.blocking)
        DrawBricksFloor(e.rect);
      else
        DrawRectangleRec(e.rect, e.color);
    }
    // Objetivo
    DrawGoal(goal);

    // Entidades
    player.Draw();
    for (const auto &en : enemies)
      en.draw();

    EndMode2D();

    // UI Base
    DrawText("← →/A D moverse, ESPACIO para saltar. R reintenta, ENTER menu.",
             12, 10, 18, BLACK);
    DrawText("← →/A D moverse, ESPACIO para saltar. R reintenta, ENTER menu.",
             10, 8, 18, RAYWHITE);

    // Overlays por estado
    if (state == GameState::START) {
      DrawRectangle(0, 0, GetScreenWidth(), GetScreenHeight(),
                    (Color){0, 0, 0, 120});
      const char *t1 = "PROYECTO DCA";
      const char *t2 = "Pulsa ENTER o ESPACIO para jugar";
      int w1 = MeasureText(t1, 48);
      int w2 = MeasureText(t2, 24);
      DrawText(t1, GetScreenWidth() / 2 - w1 / 2 + 2, 150 + 2, 48, BLACK);
      DrawText(t1, GetScreenWidth() / 2 - w1 / 2, 150, 48, RAYWHITE);
      DrawText(t2, GetScreenWidth() / 2 - w2 / 2 + 2, 220 + 2, 24, BLACK);
      DrawText(t2, GetScreenWidth() / 2 - w2 / 2, 220, 24, RAYWHITE);
    } else if (state == GameState::VICTORY) {
      DrawRectangle(0, 0, GetScreenWidth(), GetScreenHeight(),
                    (Color){0, 255, 0, 60});
      const char *t1 = "¡VICTORIA!";
      const char *t2 = "ENTER: menu  |  R/ESPACIO: jugar de nuevo";
      int w1 = MeasureText(t1, 48);
      int w2 = MeasureText(t2, 24);
      DrawText(t1, GetScreenWidth() / 2 - w1 / 2 + 2, 150 + 2, 48, BLACK);
      DrawText(t1, GetScreenWidth() / 2 - w1 / 2, 150, 48,
               (Color){255, 255, 180, 255});
      DrawText(t2, GetScreenWidth() / 2 - w2 / 2 + 2, 220 + 2, 24, BLACK);
      DrawText(t2, GetScreenWidth() / 2 - w2 / 2, 220, 24, RAYWHITE);
    } else if (state == GameState::DEFEAT) {
      DrawRectangle(0, 0, GetScreenWidth(), GetScreenHeight(),
                    (Color){255, 0, 0, 60});
      const char *t1 = "DERROTA";
      const char *t2 = "ENTER: menu  |  R/ESPACIO: reintentar";
      int w1 = MeasureText(t1, 48);
      int w2 = MeasureText(t2, 24);
      DrawText(t1, GetScreenWidth() / 2 - w1 / 2 + 2, 150 + 2, 48, BLACK);
      DrawText(t1, GetScreenWidth() / 2 - w1 / 2, 150, 48,
               (Color){255, 200, 200, 255});
      DrawText(t2, GetScreenWidth() / 2 - w2 / 2 + 2, 220 + 2, 24, BLACK);
      DrawText(t2, GetScreenWidth() / 2 - w2 / 2, 220, 24, RAYWHITE);
    }

    EndDrawing();
  }

  CloseWindow();
  return 0;
}