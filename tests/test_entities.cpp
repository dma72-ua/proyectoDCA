#include "entities/coin.h"
#include "entities/enemy.h"
#include "entities/player.h"
#include "entities/star.h"
#include <boost/test/unit_test.hpp>

BOOST_AUTO_TEST_SUITE(EntityTests)

BOOST_AUTO_TEST_CASE(PlayerInitialization) {
  Vector2 startPos = {100.0f, 200.0f};
  Player player(startPos, 0.0f, false);

  BOOST_CHECK_EQUAL(player.position.x, startPos.x);
  BOOST_CHECK_EQUAL(player.position.y, startPos.y);
  BOOST_CHECK_EQUAL(player.speed, 0.0f);
  BOOST_CHECK_EQUAL(player.canJump, false);
}

BOOST_AUTO_TEST_CASE(PlayerReset) {
  Player player({0, 0}, 0, false);
  player.position = {500, 500};
  player.speed = 100;
  player.canJump = true;

  Vector2 resetPos = {10, 10};
  player.reset(resetPos);

  BOOST_CHECK_EQUAL(player.position.x, resetPos.x);
  BOOST_CHECK_EQUAL(player.position.y, resetPos.y);
  BOOST_CHECK_EQUAL(player.speed, 0.0f);
  BOOST_CHECK_EQUAL(player.canJump, false);
}

BOOST_AUTO_TEST_CASE(PlayerBounds) {
  Player player({100, 100}, 0, false);

  Rectangle b = player.bounds();
  BOOST_CHECK_EQUAL(b.x, 100.0f - 40.0f * 0.5f);
  BOOST_CHECK_EQUAL(b.y, 100.0f - 48.0f);
  BOOST_CHECK_EQUAL(b.width, 40.0f);
  BOOST_CHECK_EQUAL(b.height, 48.0f);
}

BOOST_AUTO_TEST_CASE(EnemyInitialization) {
  Vector2 pos = {200, 200};
  Enemy enemy(pos, 32, 32, 1);

  BOOST_CHECK_EQUAL(enemy.rect.x, pos.x);
  BOOST_CHECK_EQUAL(enemy.rect.y, pos.y);
  BOOST_CHECK_EQUAL(enemy.alive, true);
  BOOST_CHECK_EQUAL(enemy.dir, 1);
}

BOOST_AUTO_TEST_CASE(EnemyKill) {
  Enemy enemy({0, 0});
  BOOST_CHECK(enemy.alive);
  enemy.kill();
  BOOST_CHECK(!enemy.alive);
}

BOOST_AUTO_TEST_CASE(CoinInitialization) {
  Vector2 pos = {50, 50};
  Coin coin(pos);
  BOOST_CHECK_EQUAL(coin.rect.x, pos.x);
  BOOST_CHECK_EQUAL(coin.rect.y, pos.y);
  BOOST_CHECK(!coin.collected);
}

BOOST_AUTO_TEST_CASE(CoinCollection) {
  Coin coin({0, 0});
  BOOST_CHECK(!coin.collected);
  coin.startCollect();
  BOOST_CHECK(coin.collected);
}

BOOST_AUTO_TEST_CASE(StarInitialization) {
  Vector2 pos = {60, 60};
  Star star(pos);
  BOOST_CHECK_EQUAL(star.rect.x, pos.x);
  BOOST_CHECK_EQUAL(star.rect.y, pos.y);
  BOOST_CHECK(!star.collected);
}

BOOST_AUTO_TEST_CASE(StarCollection) {
  Star star({0, 0});
  BOOST_CHECK(!star.collected);
  star.startCollect();
  BOOST_CHECK(star.collected);
}

BOOST_AUTO_TEST_CASE(CollisionCheck) {
  Coin coin({100, 100}, 20);
  Rectangle playerRect = {100, 100, 30, 30};

  BOOST_CHECK(coin.checkCollision(playerRect));

  Rectangle farRect = {200, 200, 30, 30};
  BOOST_CHECK(!coin.checkCollision(farRect));
}

BOOST_AUTO_TEST_SUITE_END()
