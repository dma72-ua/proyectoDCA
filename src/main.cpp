#include <vector>
#include "envItem.h"
#include "player.h"
#include "enemy.h"

static constexpr float STOMP_TOLERANCE = 10.0f;
static constexpr float FALLING_VY_MIN  = 120.0f;

static void DrawRetroBackgroundScreen() {
    auto cloud = [](int x, int y, int w){
        DrawRectangle(x, y, w, w/2, RAYWHITE);
        DrawCircle(x + w*0.2f, y + w*0.5f, w*0.25f, RAYWHITE);
        DrawCircle(x + w*0.5f, y + w*0.5f, w*0.35f, RAYWHITE);
        DrawCircle(x + w*0.8f, y + w*0.5f, w*0.25f, RAYWHITE);
    };
    cloud(120, 60, 120); cloud(420, 90, 100); cloud(780, 50, 130);
    DrawCircle(220, 360, 100, (Color){124,197,118,255});
    DrawCircle(300, 380,  60, (Color){124,197,118,255});
}

static void DrawBricksFloor(const Rectangle& area) {
    const int tile = 16;
    for (int y = (int)area.y; y < (int)(area.y + area.height); y += tile)
        for (int x = (int)area.x; x < (int)(area.x + area.width); x += tile) {
            Color c = (((x/tile)+(y/tile)) % 2 == 0)
                    ? (Color){191,111,60,255} : (Color){173,99,52,255};
            DrawRectangle(x, y, tile, tile, c);
            DrawRectangleLines(x, y, tile, tile, (Color){110,58,28,255});
        }
}

int main() {
    InitWindow(960, 540, "proyectoDCA – mapa retro y enemigos");
    SetTargetFPS(60);

    Camera2D camera = {0};
    camera.offset = {480,270};
    camera.zoom   = 1.0f;

    std::vector<EnvItem> envItems = {
        {{-4000, 400, 8000, 200}, 1, (Color){191,111,60,255}},
        {{  200, 340,  160,  20}, 1, (Color){191,111,60,255}},
        {{  500, 300,  160,  20}, 1, (Color){191,111,60,255}},
        {{  820, 260,  120,  20}, 1, (Color){191,111,60,255}},
        {{ 1100, 360,   48,  40}, 1, (Color){ 53,148, 61,255}}, // tubería
    };

    Player player{};
    Vector2 startPos = {60, 400};
    player.reset(startPos);

    std::vector<Enemy> enemies = {
        Enemy({ 420, 0 }, 32, 32, -1),
        Enemy({ 860, 0 }, 32, 32, +1),
        Enemy({1300, 0 }, 32, 32, -1),
    };

    auto resetLevel = [&](){
        player.reset(startPos);
        enemies[0].reset({ 420,0}, -1);
        enemies[1].reset({ 860,0}, +1);
        enemies[2].reset({1300,0}, -1);
    };

    while (!WindowShouldClose()) {
        float dt = GetFrameTime();

        player.UpdatePlayer(envItems, dt);
        for (auto& e : enemies) e.update(dt, envItems);

        Rectangle pb = player.bounds();
        bool diedThisFrame = false;

        for (auto& e : enemies) {
            if (!e.alive) continue;
            Rectangle eb = e.bounds();

            if (CheckCollisionRecs(pb, eb)) {
                float pbBottom = pb.y + pb.height;
                float enemyTop = eb.y;
                bool  falling  = (player.vy() > FALLING_VY_MIN);

                bool stomp = falling && pbBottom >= enemyTop
                                       && (pbBottom - enemyTop) <= STOMP_TOLERANCE;

                if (stomp) {
                    e.kill();
                    player.position.y = enemyTop; // colocar pies sobre el enemigo
                    player.bounce(-320.0f);
                    pb = player.bounds();
                } else {
                    resetLevel();
                    diedThisFrame = true;
                    break;
                }
            }
        }

        if (diedThisFrame) {
            BeginDrawing(); ClearBackground((Color){138,197,255,255}); EndDrawing();
            continue;
        }

        camera.target = { pb.x + pb.width*0.5f, pb.y + pb.height*0.5f };

        BeginDrawing();
        ClearBackground((Color){138,197,255,255});
        DrawRetroBackgroundScreen();

        BeginMode2D(camera);
        for (const auto& e : envItems) {
            if (e.blocking) DrawBricksFloor(e.rect);
            else DrawRectangleRec(e.rect, e.color);
        }
        player.Draw();
        for (const auto& e : enemies) e.draw();
        EndMode2D();

        DrawText("Salta sobre los enemigos para derrotarlos. Si te tocan, reinicias.",
                 12, 10, 18, BLACK);
        DrawText("Salta sobre los enemigos para derrotarlos. Si te tocan, reinicias.",
                 10, 8, 18, RAYWHITE);

        EndDrawing();
    }

    CloseWindow();
    return 0;
}
