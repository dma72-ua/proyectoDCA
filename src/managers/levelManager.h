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
  std::vector<Vector2> coinPositions;
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
  std::string getLevelName(int index) {
    if (index >= 0 && static_cast<size_t>(index) < levels.size())
      return levels[index].name;
    return "";
  };

private:
  std::vector<Level> levels;
  size_t currentLevelIndex;

  void initializeLevels();
};