extern "C" {
#include "../vendor/include/raylib.h"
}

class Player {
public:
  Player(Vector2, float, bool);
  void updatePlayer(float);
  Vector2 position;

private:
  float speed;
  bool canJump;

  float PLAYER_MOVE_SPEED = 200.0f;
  float PLAYER_JUMP_SPEED = 350.0f;
};
