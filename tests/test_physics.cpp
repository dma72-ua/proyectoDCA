#include "entities/enemy.h"
#include "entities/player.h"
#include "managers/envItem.h"
#include <boost/test/unit_test.hpp>
#include <vector>

BOOST_AUTO_TEST_SUITE(PhysicsTests)

BOOST_AUTO_TEST_CASE(EnemyMovement) {
  Enemy enemy({0, 0}, 32, 32, 1);
  std::vector<EnvItem> items; // Empty environment

  float dt = 1.0f;
  enemy.update(dt, items);

  BOOST_CHECK_CLOSE(enemy.rect.x, 80.0f, 0.1f);

  BOOST_CHECK_GT(enemy.rect.y, 0.0f);
}

BOOST_AUTO_TEST_CASE(EnemyWallCollision) {
  Enemy enemy({0, 0}, 32, 32, 1);

  EnvItem wall = {{50, 0, 20, 100}, 1, BLACK, 1};
  wall.blocking = true;
  std::vector<EnvItem> items = {wall};

  float dt = 0.016f;
  for (int i = 0; i < 60; ++i) {
    enemy.update(dt, items);
  }

  BOOST_CHECK_LE(enemy.rect.x, 50.0f);
  BOOST_CHECK_EQUAL(enemy.dir, -1);
}

BOOST_AUTO_TEST_CASE(PlayerGravity) {
  Player player({0, 0}, 0, false);
  std::vector<EnvItem> items;
  std::vector<Teleporter> teleporters;

  float dt = 0.016f;
  for (int i = 0; i < 10; ++i) {
    player.UpdatePlayer(items, teleporters, dt);
  }

  BOOST_CHECK_GT(player.speed, 0.0f);
  BOOST_CHECK_GT(player.position.y, 0.0f);
}

BOOST_AUTO_TEST_CASE(PlayerGroundCollision) {
  Player player({0, 0}, 100.0f, false);

  EnvItem ground = {{-100, 10, 200, 20}, 1, BLACK, 1};
  ground.blocking = true;
  std::vector<EnvItem> items = {ground};
  std::vector<Teleporter> teleporters;

  player.position = {0, 10}; // Feet at 10
  player.speed = 100.0f;     // Moving down

  player.UpdatePlayer(items, teleporters, 0.1f);

  BOOST_CHECK_EQUAL(player.position.y, 10.0f);
  BOOST_CHECK_EQUAL(player.speed, 0.0f);
  BOOST_CHECK_EQUAL(player.canJump, true);
}

BOOST_AUTO_TEST_SUITE_END()
