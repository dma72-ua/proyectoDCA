#pragma once

#include "envItem.h"
#include <string>
#include <vector>

struct EnemySpawn {
  Vector2 position;
  int direction;
};

struct Teleporter {
  Rectangle entry;
  Vector2 destination;
};

struct Level {
  std::string name;
  Vector2 playerStart;
  Rectangle goal;
  std::vector<EnvItem> envItems;
  std::vector<EnemySpawn> enemySpawns;
  Color skyColor;
  std::vector<Vector2> coinPositions;
  std::vector<Vector2> starPositions;
  std::vector<Teleporter> teleporters;
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
  void setLevel(int index) {
    if (index >= 0 && static_cast<size_t>(index) < levels.size())
      currentLevelIndex = index;
  };
  std::string getLevelName(int index);

private:
  std::vector<Level> levels;
  size_t currentLevelIndex;

  void initializeLevels();
};