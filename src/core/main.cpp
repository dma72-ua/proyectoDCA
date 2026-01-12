#include "core/textureManager.h"
#include "entities/coin.h"
#include "entities/enemy.h"
#include "entities/player.h"
#include "entities/star.h"
#include "managers/envItem.h"
#include "managers/levelManager.h"
#include <cmath>
#include <filesystem>
#include <libintl.h>
#include <locale.h>
#include <vector>

#define _(String) gettext(String)

// ----------------- Estados de juego -----------------
enum class GameState {
  START,
  MENU,
  PLAYING,
  VICTORY,
  DEFEAT,
  ALL_LEVELS_COMPLETE
};
// ----------------- Constantes de interacción -----------------
static constexpr float STOMP_TOLERANCE = 10.0f;
static constexpr float FALLING_VY_MIN = 120.0f;

std::string GetAssetsPath() {
  namespace fs = std::filesystem;

  // Primero intenta la ruta local
  if (fs::exists("assets")) {
    return "assets/";
  }

#ifndef _WIN32
  // Si no está en la local, intentar la ruta de Flatpak (solo Linux)
  if (fs::exists("/app/share/proyectoDCA/assets")) {
    return "/app/share/proyectoDCA/assets/";
  }

  // Si no está en Flatpak, usar ruta del sistema
  if (fs::exists("/usr/share/proyectoDCA/assets")) {
    return "/usr/share/proyectoDCA/assets/";
  }
#endif

  return "assets/";
}

// ----------------- Estructura para gestionar audio -----------------
struct GameAudio {
  Music backgroundMusic;
  Sound jumpSound;
  Sound coinSound;
  Sound stompSound;
  Sound hurtSound;
  Sound victorySound;

  void Load() {
    std::string assetsPath = GetAssetsPath();

    backgroundMusic = LoadMusicStream((assetsPath + "music.mp3").c_str());
    jumpSound = LoadSound((assetsPath + "jump.mp3").c_str());
    coinSound = LoadSound((assetsPath + "coin.mp3").c_str());
    stompSound = LoadSound((assetsPath + "stomp.mp3").c_str());
    hurtSound = LoadSound((assetsPath + "hurt.mp3").c_str());
    victorySound = LoadSound((assetsPath + "victory.mp3").c_str());

    // Ajustar volúmenes (0.0f a 1.0f)
    SetMusicVolume(backgroundMusic, 0.45f);
    SetSoundVolume(jumpSound, 0.3f);
    SetSoundVolume(coinSound, 0.7f);
    SetSoundVolume(stompSound, 0.7f);
    SetSoundVolume(hurtSound, 0.6f);
    SetSoundVolume(victorySound, 0.6f);
  }

  void Unload() {
    UnloadMusicStream(backgroundMusic);
    UnloadSound(jumpSound);
    UnloadSound(coinSound);
    UnloadSound(stompSound);
    UnloadSound(hurtSound);
    UnloadSound(victorySound);
  }

  void Update() { UpdateMusicStream(backgroundMusic); }
};

// ----------------- Temas -----------------
struct Theme {
  const char *name;
  Color skyColor;
};

static const std::vector<Theme> themes = {
    {_("DIA"), Color{138, 197, 255, 255}},
    {_("NOCHE"), Color{20, 24, 82, 255}},
    {_("ATARDECER"), Color{255, 145, 77, 255}}};

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
  DrawCircle(220, 360, 100, Color{124, 197, 118, 255});
  DrawCircle(300, 380, 60, Color{124, 197, 118, 255});
}

// ----------------- Suelo/Plataformas ladrillo -----------------
static void DrawBricksFloor(const Rectangle &area) {
  const int tile = 16;
  for (int y = (int)area.y; y < (int)(area.y + area.height); y += tile) {
    for (int x = (int)area.x; x < (int)(area.x + area.width); x += tile) {
      Color c = (((x / tile) + (y / tile)) % 2 == 0) ? Color{191, 111, 60, 255}
                                                     : Color{173, 99, 52, 255};
      DrawRectangle(x, y, tile, tile, c);
      DrawRectangleLines(x, y, tile, tile, Color{110, 58, 28, 255});
    }
  }
}

// ----------------- Dibujo del objetivo (bandera/palo) -----------------
static void DrawGoal(const Rectangle &goal) {
  DrawRectangle(goal.x, goal.y - goal.height, 8, goal.height,
                Color{180, 180, 180, 255});
  DrawRectangle(goal.x + 8, goal.y - goal.height + 12, 28, 18,
                Color{255, 85, 85, 255});
  DrawTriangle({goal.x + 36, goal.y - goal.height + 21},
               {goal.x + 8 + 28, goal.y - goal.height + 12},
               {goal.x + 8 + 28, goal.y - goal.height + 12 + 18},
               Color{220, 40, 40, 255});
}

void UpdateCameraPlayerBoundsPush(Camera2D *camera, Player *player) {
  static Vector2 bbox = {0.2f, 0.2f};

  // float delta = GetFrameTime();
  float width = GetScreenWidth();
  float height = GetScreenHeight();

  Vector2 bboxWorldMin = GetScreenToWorld2D(
      Vector2{(1 - bbox.x) * 0.5f * width, (1 - bbox.y) * 0.5f * height},
      *camera);
  Vector2 bboxWorldMax = GetScreenToWorld2D(
      Vector2{(1 + bbox.x) * 0.5f * width, (1 + bbox.y) * 0.5f * height},
      *camera);
  camera->offset =
      Vector2{(1 - bbox.x) * 0.5f * width, (1 - bbox.y) * 0.5f * height};

  if (player->position.x < bboxWorldMin.x)
    camera->target.x = player->position.x;
  if (player->position.y < bboxWorldMin.y)
    camera->target.y = player->position.y;
  if (player->position.x > bboxWorldMax.x)
    camera->target.x = bboxWorldMin.x + (player->position.x - bboxWorldMax.x);
  if (player->position.y > bboxWorldMax.y)
    camera->target.y = bboxWorldMin.y + (player->position.y - bboxWorldMax.y);
}

void ChangeLanguage(const char *lang) {
  setenv("LANGUAGE", lang, 1);
  setlocale(LC_ALL, "");
  // Reiniciar el dominio de texto para recargar las traducciones
  textdomain("proyectoDCA");
}

int main() {
  // Inicialización de gettext
  setlocale(LC_ALL, "");
  bindtextdomain("proyectoDCA", "locales");
  textdomain("proyectoDCA");

  // Idioma por defecto: Español
  ChangeLanguage("es");

  InitWindow(960, 540, "proyectoDCA");
  SetTargetFPS(60);

  // Inicializar sistema de audio
  InitAudioDevice();
  GameAudio audio;
  audio.Load();

  std::string assetsPath = GetAssetsPath();
  TextureManager::Instance().Load("player",
                                  (assetsPath + "player.png").c_str());
  TextureManager::Instance().Load("enemy", (assetsPath + "enemy.png").c_str());
  TextureManager::Instance().Load("bricks",
                                  (assetsPath + "bricks.png").c_str());
  TextureManager::Instance().Load("pipe", (assetsPath + "pipe.png").c_str());
  TextureManager::Instance().Load("heart", (assetsPath + "heart.png").c_str());
  TextureManager::Instance().Load("star", (assetsPath + "star.png").c_str());
  TextureManager::Instance().Load("coin", (assetsPath + "coin.png").c_str());

  Camera2D camera = {0};
  camera.offset = {480, 350};
  camera.zoom = 0.875f;

  std::vector<Coin> coins;
  std::vector<Star> stars;
  int score = 0;
  int coinsCollected = 0;
  int totalCoinsInLevel = 0;

  int playerLives = 3;
  const int MAX_LIVES = 3;
  float invincibilityTimer = 0.0f;
  const float INVINCIBILITY_DURATION = 2.0f;

  LevelManager levelManager;
  Player player{};
  std::vector<Enemy> enemies;

  GameState state = GameState::START;
  int menuSelection = 0;
  long unsigned int themeSelection = 0;
  bool isEnglish = false;

  auto loadCurrentLevel = [&]() {
    const Level &level = levelManager.getCurrentLevel();
    player.reset(level.playerStart);

    enemies.clear();
    for (const auto &spawn : level.enemySpawns) {
      enemies.push_back(Enemy(spawn.position, 32, 32, spawn.direction));
    }

    coins.clear();
    for (const auto &pos : level.coinPositions) {
      coins.push_back(Coin(pos));
    }
    totalCoinsInLevel = coins.size();
    coinsCollected = 0;

    stars.clear();
    for (const auto &pos : level.starPositions) {
      stars.push_back(Star(pos));
    }

    playerLives = MAX_LIVES;
    printf("DEBUG: Nivel cargado. Vidas reseteadas a: %d\n", playerLives);

    Rectangle pb = player.bounds();
    camera.target = {pb.x + pb.width * 0.5f, pb.y + pb.height * 0.5f};
  };

  loadCurrentLevel();

  // Iniciar música de fondo
  PlayMusicStream(audio.backgroundMusic);

  while (!WindowShouldClose()) {
    float dt = GetFrameTime();
    const Level &currentLevel = levelManager.getCurrentLevel();

    // Actualizar música
    audio.Update();

    // --------- INPUT de estado ---------
    if (state == GameState::START) {
      if (IsKeyPressed(KEY_ENTER) || IsKeyPressed(KEY_SPACE)) {
        state = GameState::MENU;
      }
    } else if (state == GameState::MENU) {
      if (IsKeyPressed(KEY_DOWN)) {
        menuSelection++;
        if (menuSelection >= levelManager.getTotalLevels())
          menuSelection = 0;
      } else if (IsKeyPressed(KEY_UP)) {
        menuSelection--;
        if (menuSelection < 0)
          menuSelection = levelManager.getTotalLevels() - 1;
      } else if (IsKeyPressed(KEY_RIGHT)) {
        themeSelection++;
        if (themeSelection >= themes.size())
          themeSelection = 0;
      } else if (IsKeyPressed(KEY_LEFT)) {
        themeSelection--;
        if (themeSelection < 0)
          themeSelection = themes.size() - 1;
      } else if (IsKeyPressed(KEY_L)) {
        isEnglish = !isEnglish;
        ChangeLanguage(isEnglish ? "en" : "es");
      }

      if (IsKeyPressed(KEY_ENTER) || IsKeyPressed(KEY_SPACE)) {
        levelManager.setLevel(menuSelection);
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
    else if (state == GameState::PLAYING) {
      if (IsKeyPressed(KEY_ENTER)) {
        state = GameState::START;
      }
      if (invincibilityTimer > 0.0f) {
        invincibilityTimer -= dt;
      }

      // Perder la partida si se cae del mapa
      if (player.position.y >= 800) {
        PlaySound(audio.hurtSound);
        state = GameState::DEFEAT;
      }

      // Detectar salto para reproducir sonido
      bool wasJumping = player.vy() < 0;
      player.UpdatePlayer(currentLevel.envItems, currentLevel.teleporters, dt);
      bool isJumping = player.vy() < 0;

      if (isJumping && !wasJumping) {
        PlaySound(audio.jumpSound);
      }

      for (auto &e : enemies)
        e.update(dt, currentLevel.envItems);

      Rectangle pb = player.bounds();

      // Recolección de monedas
      for (auto &coin : coins) {
        if (coin.checkCollision(pb)) {
          if (!coin.collected) {
            coin.startCollect();
            coinsCollected++;
            score += 100;
            PlaySound(audio.coinSound);
          }
        }
      }

      // Recolección de estrellas
      for (auto &star : stars) {
        if (star.checkCollision(pb)) {
          if (!star.collected) {
            star.startCollect();

            if (playerLives < MAX_LIVES) {
              // Restaurar una vida
              playerLives++;
              printf("DEBUG: ¡Estrella! Vida restaurada. Vidas: %d\n",
                     playerLives);
              PlaySound(
                  audio.coinSound); // Usar sonido de moneda o crear uno nuevo
            } else {
              // Dar puntos de bonificación
              score += 500;
              printf("DEBUG: ¡Estrella! Bonus de 500 puntos. Total: %d\n",
                     score);
              PlaySound(audio.coinSound);
            }
          }
        }
      }

      // Colisión con enemigos
      bool diedThisFrame = false;
      for (auto &e : enemies) {
        if (!e.alive)
          continue;

        if (invincibilityTimer > 0.0f)
          continue;

        if (e.rect.y >= 600)
          e.reset(e.startingPos, e.startingDir);

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
            score += 200;
            PlaySound(audio.stompSound);
          } else {
            playerLives--;
            printf("DEBUG: Perdiste una vida! Vidas restantes: %d\n",
                   playerLives);

            invincibilityTimer = INVINCIBILITY_DURATION;
            PlaySound(audio.hurtSound);

            if (playerLives <= 0) {
              state = GameState::DEFEAT;
              diedThisFrame = true;
            } else {
              player.bounce(-200.0f);
            }
            break;
          }
        }
      }

      // Victoria
      if (!diedThisFrame) {
        Rectangle goalArea = {currentLevel.goal.x - 6,
                              currentLevel.goal.y - currentLevel.goal.height,
                              40, currentLevel.goal.height};
        if (CheckCollisionRecs(pb, goalArea)) {
          state = GameState::VICTORY;
          PlaySound(audio.victorySound);
        }
      }

      UpdateCameraPlayerBoundsPush(&camera, &player);
    }

    // --------- DRAW ---------
    BeginDrawing();
    ClearBackground(themes[themeSelection].skyColor);

    DrawRetroBackgroundScreen();

    BeginMode2D(camera);
    for (const auto &e : currentLevel.envItems) {
      bool drawn = false;
      if (e.textureId == 1) {
        Texture2D tex = TextureManager::Instance().Get("bricks");
        if (tex.id != 0) {
          float tileSize = 64.0f;

          for (float y = 0; y < e.rect.height; y += tileSize) {
            for (float x = 0; x < e.rect.width; x += tileSize) {
              float drawW =
                  (e.rect.width - x < tileSize) ? (e.rect.width - x) : tileSize;
              float drawH = (e.rect.height - y < tileSize) ? (e.rect.height - y)
                                                           : tileSize;

              float sourceW = (drawW / tileSize) * tex.width;
              float sourceH = (drawH / tileSize) * tex.height;

              Rectangle source = {0, 0, sourceW, sourceH};
              Rectangle dest = {e.rect.x + x, e.rect.y + y, drawW, drawH};

              DrawTexturePro(tex, source, dest, {0, 0}, 0.0f, WHITE);
            }
          }
          drawn = true;
        }
      } else if (e.textureId == 2) {
        Texture2D tex = TextureManager::Instance().Get("pipe");
        if (tex.id != 0) {
          DrawTexturePro(tex, {0, 0, (float)tex.width, (float)tex.height},
                         e.rect, {0, 0}, 0.0f, WHITE);
          drawn = true;
        }
      }

      if (!drawn) {
        if (e.blocking)
          DrawBricksFloor(e.rect);
        else
          DrawRectangleRec(e.rect, e.color);
      }
    }
    DrawGoal(currentLevel.goal);

    if (invincibilityTimer <= 0.0f ||
        fmod(invincibilityTimer * 10.0f, 1.0f) > 0.5f) {
      player.Draw();
    }
    for (const auto &en : enemies)
      en.draw();

    for (const auto &coin : coins)
      coin.draw();
    for (const auto &star : stars)
      star.draw();

    EndMode2D();

    // UI - Mostrar puntuación
    const char *scoreText =
        TextFormat(_("Monedas: %d/%d  Puntos: %d"), coinsCollected,
                   totalCoinsInLevel, score);
    DrawText(scoreText, 10, 50, 22, BLACK);
    DrawText(scoreText, 8, 48, 22, Color{255, 215, 0, 255});

    // Dibujar corazones
    Texture2D heartTex = TextureManager::Instance().Get("heart");
    if (heartTex.id != 0) {
      int heartSize = 30;
      int heartSpacing = 35;
      int startX = GetScreenWidth() - (MAX_LIVES * heartSpacing) - 10;
      int startY = 50;

      for (int i = 0; i < MAX_LIVES; i++) {
        if (i < playerLives) {
          DrawTexturePro(heartTex,
                         {0, 0, (float)heartTex.width, (float)heartTex.height},
                         {(float)(startX + i * heartSpacing), (float)startY,
                          (float)heartSize, (float)heartSize},
                         {0, 0}, 0.0f, WHITE);
        } else {
          DrawTexturePro(heartTex,
                         {0, 0, (float)heartTex.width, (float)heartTex.height},
                         {(float)(startX + i * heartSpacing), (float)startY,
                          (float)heartSize, (float)heartSize},
                         {0, 0}, 0.0f, Color{255, 255, 255, 80});
        }
      }
    }

    DrawText(
        _("Flechas/A D moverse, ESPACIO para saltar. R reintenta, ENTER menu."),
        12, 10, 18, BLACK);
    DrawText(
        _("Flechas/A D moverse, ESPACIO para saltar. R reintenta, ENTER menu."),
        10, 8, 18, RAYWHITE);

    if (state == GameState::PLAYING) {
      const char *levelText =
          TextFormat(_("Nivel %d/%d"), levelManager.getCurrentLevelNumber(),
                     levelManager.getTotalLevels());
      DrawText(levelText, GetScreenWidth() - 150, 10, 20, BLACK);
      DrawText(levelText, GetScreenWidth() - 152, 8, 20, YELLOW);
    }

    // Overlays por estado
    if (state == GameState::START) {
      DrawRectangle(0, 0, GetScreenWidth(), GetScreenHeight(),
                    Color{0, 0, 0, 120});
      const char *t1 = _("PROYECTO DCA");
      const char *t2 = _("Pulsa ENTER o ESPACIO para jugar");
      const char *t3 = TextFormat(_("%d niveles disponibles"),
                                  levelManager.getTotalLevels());
      int w1 = MeasureText(t1, 48);
      int w2 = MeasureText(t2, 24);
      int w3 = MeasureText(t3, 20);
      DrawText(t1, GetScreenWidth() / 2 - w1 / 2 + 2, 120 + 2, 48, BLACK);
      DrawText(t1, GetScreenWidth() / 2 - w1 / 2, 120, 48, RAYWHITE);
      DrawText(t2, GetScreenWidth() / 2 - w2 / 2 + 2, 200 + 2, 24, BLACK);
      DrawText(t2, GetScreenWidth() / 2 - w2 / 2, 200, 24, RAYWHITE);
      DrawText(t3, GetScreenWidth() / 2 - w3 / 2 + 2, 240 + 2, 20, BLACK);
      DrawText(t3, GetScreenWidth() / 2 - w3 / 2, 240, 20, GREEN);
    } else if (state == GameState::MENU) {
      DrawRectangle(0, 0, GetScreenWidth(), GetScreenHeight(),
                    Color{0, 0, 0, 120});
      const char *title = _("SELECCIONA UN NIVEL");
      int wTitle = MeasureText(title, 32);
      DrawText(title, GetScreenWidth() / 2 - wTitle / 2 + 2, 100 + 2, 32,
               BLACK);
      DrawText(title, GetScreenWidth() / 2 - wTitle / 2, 100, 32, RAYWHITE);

      int startY = 180;
      int spacing = 40;
      for (int i = 0; i < levelManager.getTotalLevels(); i++) {
        std::string name = levelManager.getLevelName(i);
        Color c = (i == menuSelection) ? YELLOW : RAYWHITE;
        if (i == menuSelection) {
          DrawText(">", 200, startY + i * spacing, 24, YELLOW);
        }
        DrawText(name.c_str(), 230, startY + i * spacing, 24, c);
      }

      int themeY = startY + levelManager.getTotalLevels() * spacing + 40;
      DrawText(_("TEMA (Izquierda/Derecha):"), 200, themeY, 20, RAYWHITE);
      const char *themeName = themes[themeSelection].name;
      DrawText(TextFormat("< %s >", themeName), 480, themeY, 24, YELLOW);

      int langY = themeY + 40;
      DrawText(_("IDIOMA (L):"), 200, langY, 20, RAYWHITE);
      DrawText(isEnglish ? "ENGLISH" : "ESPAÑOL", 480, langY, 24, YELLOW);
    } else if (state == GameState::VICTORY) {
      DrawRectangle(0, 0, GetScreenWidth(), GetScreenHeight(),
                    Color{0, 255, 0, 60});
      const char *t1 = levelManager.isLastLevel()
                           ? _("¡NIVEL FINAL COMPLETADO!")
                           : _("¡NIVEL COMPLETADO!");
      const char *t2 = levelManager.isLastLevel()
                           ? _("ENTER: volver al menú")
                           : _("ENTER/ESPACIO: siguiente nivel");
      int w1 = MeasureText(t1, 40);
      int w2 = MeasureText(t2, 24);
      DrawText(t1, GetScreenWidth() / 2 - w1 / 2 + 2, 150 + 2, 40, BLACK);
      DrawText(t1, GetScreenWidth() / 2 - w1 / 2, 150, 40,
               Color{255, 255, 180, 255});
      DrawText(t2, GetScreenWidth() / 2 - w2 / 2 + 2, 220 + 2, 24, BLACK);
      DrawText(t2, GetScreenWidth() / 2 - w2 / 2, 220, 24, RAYWHITE);
    } else if (state == GameState::DEFEAT) {
      DrawRectangle(0, 0, GetScreenWidth(), GetScreenHeight(),
                    Color{255, 0, 0, 60});
      const char *t1 = _("DERROTA");
      const char *t2 = _("ENTER: menu  |  R/ESPACIO: reintentar");
      int w1 = MeasureText(t1, 48);
      int w2 = MeasureText(t2, 24);
      DrawText(t1, GetScreenWidth() / 2 - w1 / 2 + 2, 150 + 2, 48, BLACK);
      DrawText(t1, GetScreenWidth() / 2 - w1 / 2, 150, 48,
               Color{255, 200, 200, 255});
      DrawText(t2, GetScreenWidth() / 2 - w2 / 2 + 2, 220 + 2, 24, BLACK);
      DrawText(t2, GetScreenWidth() / 2 - w2 / 2, 220, 24, RAYWHITE);
    } else if (state == GameState::ALL_LEVELS_COMPLETE) {
      DrawRectangle(0, 0, GetScreenWidth(), GetScreenHeight(),
                    Color{255, 215, 0, 80});
      const char *t1 = _("¡TODOS LOS NIVELES COMPLETADOS!");
      const char *t2 = _("¡FELICIDADES!");
      const char *t3 = _("ENTER: volver al menú");
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

  // Limpiar audio
  audio.Unload();
  CloseAudioDevice();

  TextureManager::Instance().UnloadAll();
  CloseWindow();
  return 0;
}
