#include <boost/test/unit_test.hpp>
#include "managers/levelManager.h"
#include "core/textureManager.h"

BOOST_AUTO_TEST_SUITE(ManagerTests)

BOOST_AUTO_TEST_CASE(LevelManagerInitialization) {
    LevelManager lm;
    BOOST_CHECK_EQUAL(lm.getCurrentLevelNumber(), 1);
    BOOST_CHECK(lm.getTotalLevels() > 0);
}

BOOST_AUTO_TEST_CASE(LevelManagerNavigation) {
    LevelManager lm;
    int total = lm.getTotalLevels();
    
    // Reset
    lm.resetToFirstLevel();
    BOOST_CHECK_EQUAL(lm.getCurrentLevelNumber(), 1);
    
    // Set level
    if (total > 1) {
        lm.setLevel(1);
        BOOST_CHECK_EQUAL(lm.getCurrentLevelNumber(), 2);
    }
    
    // Advance through all levels
    lm.resetToFirstLevel();
    for (int i = 0; i < total - 1; ++i) {
        BOOST_CHECK(lm.advanceLevel());
    }
    
    // Should be at last level
    BOOST_CHECK(lm.isLastLevel());
    
    // Advance again should fail (or return false)
    BOOST_CHECK(!lm.advanceLevel());
}

BOOST_AUTO_TEST_CASE(TextureManagerSingleton) {
    TextureManager& tm1 = TextureManager::Instance();
    TextureManager& tm2 = TextureManager::Instance();
    
    BOOST_CHECK_EQUAL(&tm1, &tm2);
}

BOOST_AUTO_TEST_CASE(TextureManagerGetMissing) {
    TextureManager& tm = TextureManager::Instance();
    Texture2D tex = tm.Get("non_existent_texture");
    BOOST_CHECK_EQUAL(tex.id, 0);
}

BOOST_AUTO_TEST_SUITE_END()
