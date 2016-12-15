#ifndef TOWERDEFENSE_VIEWER_MAP_DRAWER_H
#define TOWERDEFENSE_VIEWER_MAP_DRAWER_H

#include <vector>
#include <SDL2/SDL.h>
#include <helpers/WorldDataSerializer.h>
#include <map>
#include <helpers/ViewerDataSerializer.h>

class MapDrawer {
private:
    // Basic informations
    int width;
    int height;
    int tileSize;
    int menuWidth;
    bool initStatus;
    bool quit;
    bool gameStatusChanged;
    bool towerOneActive;
    bool towerTwoActive;
    bool removeTowerActive;
    unsigned int score;
    unsigned int money;

    // SDL Objects
    SDL_Window* window;
    SDL_Renderer* renderer;
    std::map<std::string, SDL_Texture*>* textures;
    GameStatus gameStatus;
    OperationTowerData* nextTowerOperation;
    std::string folderPath;

    // Maps and objects
    WorldData* data;

    enum corner_side {LEFT_UP, LEFT_DOWN, RIGHT_UP, RIGHT_DOWN};
    enum marker_type {NORMAL, WRENCH, CROSS, BALL, EXPLOSION};
    enum tower_type {ONE_CANNON, TWO_CANNON};
    enum monster_type {MONSTER_1, MONSTER_2};
    enum bullet_type {BULLET_1, BULLET_2};

    bool init();
    bool loadTextures();
    void unloadTextures();
    void drawMenu();
    void drawScore();
    void drawMoney();
    void drawMenuTowerOne();
    void drawMenuTowerTwo();
    void drawMenuDemolishButton();
    void drawMenuPlayButton();
    void drawMenuPauseButton();
    void drawField(int x, int y);
    void drawRoadStraight(int x, int y, bool vertical);
    void drawRoadCorner(int x, int y, corner_side corner);
    void drawInsertPosition(int x, int y, marker_type marker);
    void drawTower(int x, int y, int angle, tower_type tower);
    void drawMonster(double x, double y, int angle, monster_type monster);
    void drawBullet(double x, double y, bullet_type bullet);
    int getDegrees(double radians);
    std::string getCurrentDir();

public:
    MapDrawer(int width, int height);
    bool initSuccessful();
    void updateWorldData(WorldData *data);
    void drawMap();
    bool handleEvents();
    GameStatus getGameStatus();
    OperationTowerData* getTowerOperation();
    bool isQuit();
    ~MapDrawer();
};

#endif