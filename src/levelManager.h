#pragma once

#include "envItem.h"
#include <string>
#include <vector>

struct EnemySpawn {
  Vector2 position;
  int direction;
};

struct Level {
  std::string name;
  Vector2 playerStart;
  Rectangle goal;
  std::vector<EnvItem> envItems;
  std::vector<EnemySpawn> enemySpawns;
  Color skyColor;
};

class LevelManager {
public:
  LevelManager();
  const Level &getCurrentLevel() { return levels[currentLevelIndex]; };
  bool advanceLevel();
  void resetToFirstLevel() { currentLevelIndex = 0; };
  int getCurrentLevelNumber() { return currentLevelIndex + 1; };
  int getTotalLevels() { return levels.size(); };
  bool isLastLevel() { return currentLevelIndex >= levels.size() - 1; };

private:
  std::vector<Level> levels;
  size_t currentLevelIndex;

  void initializeLevels();
};