#ifndef TOWERDEFENSE_DEFINITIONS_H
#define TOWERDEFENSE_DEFINITIONS_H

#define NOISE

#ifdef NOISE
#define NOISE_VALUE 0.15
#endif

enum class TowerRotation { LEFT, RIGHT };
enum class MonsterRotation { LEFT, RIGHT };
enum class MonsterMovement { FRONT, LEFT, BACK, RIGHT };
enum class TowerType { SIMPLE, COMPLEX };
enum class MonsterType { BASIC, INSANE };
enum class PositionState { EMPTY, PATH, TOWER };

enum class TowerOperation { INSERT, REMOVE };
enum class GameStatus { PLAY, PAUSE, EXIT };
enum class ViewerRequest { GAME_STATUS, TOWER };

#endif //TOWERDEFENSE_DEFINITIONS_H
