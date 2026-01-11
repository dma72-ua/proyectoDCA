#include <boost/test/unit_test.hpp>
#include "entities/enemy.h"
#include "entities/player.h"
#include "managers/envItem.h"
#include <vector>

BOOST_AUTO_TEST_SUITE(PhysicsTests)

BOOST_AUTO_TEST_CASE(EnemyMovement) {
    // Enemy moves 80px/s. 
    // Initial pos (0,0), dir +1 (right)
    Enemy enemy({0,0}, 32, 32, 1);
    std::vector<EnvItem> items; // Empty environment
    
    float dt = 1.0f;
    enemy.update(dt, items);
    
    // Should move right: x = 0 + 80 * 1 = 80
    BOOST_CHECK_CLOSE(enemy.rect.x, 80.0f, 0.1f);
    
    // Should fall due to gravity (900 px/s^2)
    // y = 0 + 0*1 + 0.5*900*1^2 ? 
    // The code does: vel.y += GRAVITY * dt; rect.y += vel.y * dt;
    // vel.y becomes 900. rect.y becomes 900.
    BOOST_CHECK_GT(enemy.rect.y, 0.0f);
}

BOOST_AUTO_TEST_CASE(EnemyWallCollision) {
    // Enemy at (0,0) moving right (+1)
    Enemy enemy({0,0}, 32, 32, 1);
    
    // Wall at x=50, blocking
    EnvItem wall = {{50, 0, 20, 100}, 1, BLACK, 1};
    wall.blocking = true;
    std::vector<EnvItem> items = { wall };
    
    // Move in small steps to ensure collision detection works
    // Total time 1.0s, step 0.016s (approx 60fps)
    float dt = 0.016f;
    for(int i=0; i<60; ++i) {
        enemy.update(dt, items);
    }
    
    // Should be pushed back to left of wall (50 - 32 = 18)
    // And moving left
    BOOST_CHECK_LE(enemy.rect.x, 50.0f); 
    BOOST_CHECK_EQUAL(enemy.dir, -1);
}

BOOST_AUTO_TEST_CASE(PlayerGravity) {
    // Player at (0,0)
    Player player({0,0}, 0, false);
    std::vector<EnvItem> items;
    std::vector<Teleporter> teleporters;
    
    // Update multiple frames
    float dt = 0.016f;
    for(int i=0; i<10; ++i) {
        player.UpdatePlayer(items, teleporters, dt);
    }
    
    // Speed should increase (positive is down)
    BOOST_CHECK_GT(player.speed, 0.0f);
    // Position should increase (fall)
    BOOST_CHECK_GT(player.position.y, 0.0f);
}

BOOST_AUTO_TEST_CASE(PlayerGroundCollision) {
    // Player at (0,0), falling
    Player player({0,0}, 100.0f, false);
    
    // Ground at y=50
    // Player height is ~48 (from header) or 35 (from constructor if texture fails).
    // Let's assume height is at least > 0.
    // Player bounds: y is position.y - height.
    // If position is (0,0), feet are at 0.
    // Let's put ground at y=10.
    
    EnvItem ground = {{-100, 10, 200, 20}, 1, BLACK, 1};
    ground.blocking = true;
    std::vector<EnvItem> items = { ground };
    std::vector<Teleporter> teleporters;
    
    // Move player slightly above ground
    player.position = {0, 10}; // Feet at 10
    player.speed = 100.0f; // Moving down
    
    // Update
    player.UpdatePlayer(items, teleporters, 0.1f);
    
    // Should stop at ground
    // Logic: if falling and hit ground, snap to top of ground (y=10) and speed=0
    BOOST_CHECK_EQUAL(player.position.y, 10.0f);
    BOOST_CHECK_EQUAL(player.speed, 0.0f);
    BOOST_CHECK_EQUAL(player.canJump, true);
}

BOOST_AUTO_TEST_SUITE_END()
