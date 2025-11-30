#include "enemy.h"
#include "envItem.h"
#include "levelManager.h"
#include "player.h"
#include <vector>

// ----------------- Estados de juego -----------------
enum class GameState { START, PLAYING, VICTORY, DEFEAT, ALL_LEVELS_COMPLETE };

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

  // Level Manager
  LevelManager levelManager;

  // Jugador
  Player player{};

  // Enemigos (vector que se rellenará según el nivel)
  std::vector<Enemy> enemies;

  // Estados
  GameState state = GameState::START;

  // Lambda para cargar el nivel actual
  auto loadCurrentLevel = [&]() {
    const Level &level = levelManager.getCurrentLevel();

    // Reset player
    player.reset(level.playerStart);

    // Create enemies from spawn data
    enemies.clear();
    for (const auto &spawn : level.enemySpawns) {
      enemies.push_back(Enemy(spawn.position, 32, 32, spawn.direction));
    }

    // Reset camera
    Rectangle pb = player.bounds();
    camera.target = {pb.x + pb.width * 0.5f, pb.y + pb.height * 0.5f};
  };

  loadCurrentLevel();

  while (!WindowShouldClose()) {
    float dt = GetFrameTime();
    const Level &currentLevel = levelManager.getCurrentLevel();

    // --------- INPUT de estado ---------
    if (state == GameState::START) {
      if (IsKeyPressed(KEY_ENTER) || IsKeyPressed(KEY_SPACE)) {
        loadCurrentLevel();
        state = GameState::PLAYING;
      }
    } else if (state == GameState::VICTORY) {
      if (IsKeyPressed(KEY_ENTER) || IsKeyPressed(KEY_SPACE)) {
        if (levelManager.advanceLevel()) {
          loadCurrentLevel();
          state = GameState::PLAYING;
        } else {
          state = GameState::ALL_LEVELS_COMPLETE;
        }
      }
    } else if (state == GameState::DEFEAT) {
      if (IsKeyPressed(KEY_ENTER)) {
        levelManager.resetToFirstLevel();
        loadCurrentLevel();
        state = GameState::START;
      }
      if (IsKeyPressed(KEY_R) || IsKeyPressed(KEY_SPACE)) {
        loadCurrentLevel();
        state = GameState::PLAYING;
      }
    } else if (state == GameState::ALL_LEVELS_COMPLETE) {
      if (IsKeyPressed(KEY_ENTER)) {
        levelManager.resetToFirstLevel();
        loadCurrentLevel();
        state = GameState::START;
      }
    }

    // --------- UPDATE (solo durante PLAYING) ---------
    if (state == GameState::PLAYING) {
      player.UpdatePlayer(currentLevel.envItems, dt);
      for (auto &e : enemies)
        e.update(dt, currentLevel.envItems);

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
            player.position.y = enemyTop;
            player.bounce(-320.0f);
            pb = player.bounds();
          } else {
            state = GameState::DEFEAT;
            diedThisFrame = true;
            break;
          }
        }
      }

      // Victoria: tocar el objetivo
      if (!diedThisFrame) {
        Rectangle goalArea = {currentLevel.goal.x - 6,
                              currentLevel.goal.y - currentLevel.goal.height,
                              40, currentLevel.goal.height};
        if (CheckCollisionRecs(pb, goalArea)) {
          state = GameState::VICTORY;
        }
      }

      // Cámara sigue al jugador
      camera.target = {pb.x + pb.width * 0.5f, pb.y + pb.height * 0.5f};
    }

    // --------- DRAW ---------
    BeginDrawing();
    ClearBackground(currentLevel.skyColor);

    DrawRetroBackgroundScreen();

    // Mundo
    BeginMode2D(camera);
    for (const auto &e : currentLevel.envItems) {
      if (e.blocking)
        DrawBricksFloor(e.rect);
      else
        DrawRectangleRec(e.rect, e.color);
    }
    DrawGoal(currentLevel.goal);

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

    // Level indicator during gameplay
    if (state == GameState::PLAYING) {
      const char *levelText =
          TextFormat("Nivel %d/%d", levelManager.getCurrentLevelNumber(),
                     levelManager.getTotalLevels());
      DrawText(levelText, GetScreenWidth() - 150, 10, 20, BLACK);
      DrawText(levelText, GetScreenWidth() - 152, 8, 20, YELLOW);
    }

    // Overlays por estado
    if (state == GameState::START) {
      DrawRectangle(0, 0, GetScreenWidth(), GetScreenHeight(),
                    (Color){0, 0, 0, 120});
      const char *t1 = "PROYECTO DCA";
      const char *t2 = "Pulsa ENTER o ESPACIO para jugar";
      const char *t3 =
          TextFormat("%d niveles disponibles", levelManager.getTotalLevels());
      int w1 = MeasureText(t1, 48);
      int w2 = MeasureText(t2, 24);
      int w3 = MeasureText(t3, 20);
      DrawText(t1, GetScreenWidth() / 2 - w1 / 2 + 2, 120 + 2, 48, BLACK);
      DrawText(t1, GetScreenWidth() / 2 - w1 / 2, 120, 48, RAYWHITE);
      DrawText(t2, GetScreenWidth() / 2 - w2 / 2 + 2, 200 + 2, 24, BLACK);
      DrawText(t2, GetScreenWidth() / 2 - w2 / 2, 200, 24, RAYWHITE);
      DrawText(t3, GetScreenWidth() / 2 - w3 / 2 + 2, 240 + 2, 20, BLACK);
      DrawText(t3, GetScreenWidth() / 2 - w3 / 2, 240, 20, GREEN);
    } else if (state == GameState::VICTORY) {
      DrawRectangle(0, 0, GetScreenWidth(), GetScreenHeight(),
                    (Color){0, 255, 0, 60});
      const char *t1 = levelManager.isLastLevel() ? "¡NIVEL FINAL COMPLETADO!"
                                                  : "¡NIVEL COMPLETADO!";
      const char *t2 = levelManager.isLastLevel()
                           ? "ENTER: volver al menú"
                           : "ENTER/ESPACIO: siguiente nivel";
      int w1 = MeasureText(t1, 40);
      int w2 = MeasureText(t2, 24);
      DrawText(t1, GetScreenWidth() / 2 - w1 / 2 + 2, 150 + 2, 40, BLACK);
      DrawText(t1, GetScreenWidth() / 2 - w1 / 2, 150, 40,
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
    } else if (state == GameState::ALL_LEVELS_COMPLETE) {
      DrawRectangle(0, 0, GetScreenWidth(), GetScreenHeight(),
                    (Color){255, 215, 0, 80});
      const char *t1 = "¡TODOS LOS NIVELES COMPLETADOS!";
      const char *t2 = "¡FELICIDADES!";
      const char *t3 = "ENTER: volver al menú";
      int w1 = MeasureText(t1, 36);
      int w2 = MeasureText(t2, 48);
      int w3 = MeasureText(t3, 24);
      DrawText(t1, GetScreenWidth() / 2 - w1 / 2 + 2, 100 + 2, 36, BLACK);
      DrawText(t1, GetScreenWidth() / 2 - w1 / 2, 100, 36, YELLOW);
      DrawText(t2, GetScreenWidth() / 2 - w2 / 2 + 2, 160 + 2, 48, BLACK);
      DrawText(t2, GetScreenWidth() / 2 - w2 / 2, 160, 48, GOLD);
      DrawText(t3, GetScreenWidth() / 2 - w3 / 2 + 2, 240 + 2, 24, BLACK);
      DrawText(t3, GetScreenWidth() / 2 - w3 / 2, 240, 24, RAYWHITE);
    }

    EndDrawing();
  }

  CloseWindow();
  return 0;
}