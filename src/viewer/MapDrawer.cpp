#include <iostream>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_ttf.h>
#include <unistd.h>
#include "MapDrawer.h"
#include "aux/Cleanup.h"

MapDrawer::MapDrawer(int width, int height) {
    this->width = width;
    this->height = height;
    this->tileSize = 80;
    this->menuWidth = 100;
    this->towerOneActive = false;
    this->towerTwoActive = false;
    this->removeTowerActive = false;
    this->quit = false;
    this->gameStatusChanged = false;
    this->gameStatus = GameStatus::PAUSE;
    this->nextTowerOperation = nullptr;

    this->textures = new std::map<std::string, SDL_Texture*>();

    this->initStatus = init();

    if(initStatus)
        this->initStatus &= loadTextures();

    if(!initStatus)
        unloadTextures();
}

MapDrawer::~MapDrawer() {
    delete this->data;
    this->data = nullptr;

    if(initStatus) {
        unloadTextures();

        delete this->textures;

        cleanup(this->renderer, this->window);

        TTF_Quit();
        IMG_Quit();
        SDL_Quit();
    }
}

bool MapDrawer::init() {
    if(SDL_Init(SDL_INIT_VIDEO) != 0) {
        std::cout << "SDL_Init Error: " << SDL_GetError() << std::endl;
        return false;
    }

    int imgFlags = IMG_INIT_PNG;
    if((IMG_Init(imgFlags) & imgFlags) != imgFlags) {
        std::cout << "IMG_Init Error: " << IMG_GetError() << std::endl;
        return false;
    }

    //Initialize SDL_ttf
    if( TTF_Init() == -1 ) {
        std::cout << "TTF_Init Error: " << TTF_GetError() << std::endl;
        return false;
    }

    this->window = SDL_CreateWindow("Tower Defense", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, this->width, this->height, SDL_WINDOW_SHOWN);
    if (this->window == nullptr){
        std::cout << "SDL_CreateWindow Error: " << SDL_GetError() << std::endl;
        SDL_Quit();
        return false;
    }

    this->renderer = SDL_CreateRenderer(this->window, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
    if (this->renderer == nullptr){
        cleanup(this->window);
        std::cout << "SDL_CreateRenderer Error: " << SDL_GetError() << std::endl;
        SDL_Quit();
        return false;
    }

    int err = SDL_SetRenderDrawColor(this->renderer, 255, 255,255, 255);
    if(err != 0) {
        cleanup(this->renderer, this->window);
        std::cout << "SDL_SetRenderDrawColor Error: " << SDL_GetError() << std::endl;
        return false;
    }

    return true;
}

void MapDrawer::updateWorldData(WorldData *data) {
    delete this->data;
    this->data = data;
}

void MapDrawer::drawMap() {
    SDL_RenderClear(this->renderer);
    SDL_SetRenderDrawColor(this->renderer, 255, 255, 255, 255);

    for(int i = 0; i < this->data->map_.size(); ++i)
        for(int j = 0; j < this->data->map_[i].size(); ++j) {
            switch (this->data->map_[i][j]) {
                case PositionState::EMPTY:
                    this->drawField(i, j);
                    break;
                case PositionState::PATH:
                    if(i-1 >= 0 && j-1 >= 0 &&
                       this->data->map_[i-1][j] == PositionState::PATH &&
                       this->data->map_[i][j-1] == PositionState::PATH)
                        this->drawRoadCorner(i, j, LEFT_UP);
                    else if(i-1 >= 0 && j+1 >= 0 &&
                            this->data->map_[i-1][j] == PositionState::PATH &&
                            this->data->map_[i][j+1] == PositionState::PATH)
                        this->drawRoadCorner(i, j, LEFT_DOWN);
                    else if(i+1 >= 0 && j-1 >= 0 &&
                            this->data->map_[i+1][j] == PositionState::PATH &&
                            this->data->map_[i][j-1] == PositionState::PATH)
                        this->drawRoadCorner(i, j, RIGHT_UP);
                    else if(i+1 >= 0 && j+1 >= 0 &&
                            this->data->map_[i+1][j] == PositionState::PATH &&
                            this->data->map_[i][j+1] == PositionState::PATH)
                        this->drawRoadCorner(i, j, RIGHT_DOWN);
                    else
                        this->drawRoadStraight(i, j,
                                               !((i - 1 >= 0 && this->data->map_[i - 1][j] == PositionState::PATH) ||
                                                 (i+1 >= 0 && this->data->map_[i+1][j] == PositionState::PATH)));
                    break;
                case PositionState::TOWER:
                    this->drawField(i, j);
                    this->drawInsertPosition(i, j, NORMAL);
                    break;
            }
        }

    for(std::vector<TowerData>::iterator it = this->data->towers_.begin(); it != this->data->towers_.end(); ++it) {
        this->drawTower(it->position_.get_x(), it->position_.get_y(), this->getDegrees(it->angle_),
                        (it->type_ == TowerType::SIMPLE ? ONE_CANNON : TWO_CANNON));
    }

    for(std::vector<MonsterData>::iterator it = this->data->monsters_.begin(); it != this->data->monsters_.end(); ++it) {
        this->drawMonster(it->position_.get_x(), it->position_.get_y(), this->getDegrees(it->angle_),
                          (it->type_ == MonsterType::BASIC ? MONSTER_1 : MONSTER_2));
    }

    for(std::vector<BulletData>::iterator it = this->data->bullets_.begin(); it != this->data->bullets_.end(); ++it) {
        this->drawBullet(it->position_.get_x(), it->position_.get_y(), (it->damage_ < 10 ? BULLET_1 : BULLET_2));
    }

    this->drawMenu();

    SDL_RenderPresent(this->renderer);
}

bool MapDrawer::handleEvents() {
    SDL_Event e;

    while(SDL_PollEvent(&e)) {
        switch (e.type) {
            case SDL_QUIT:
                this->quit = true;
                break;
            case SDL_MOUSEBUTTONDOWN:
                bool leftPressed = false;
                int x, y;

                if(SDL_GetMouseState(&x, &y) & SDL_BUTTON(SDL_BUTTON_LEFT))
                    leftPressed = true;

                bool towerOne = leftPressed &&
                        x >= this->width - this->menuWidth + 5 &&
                        x <= this->width - 10 &&
                        y >= 210 &&
                        y <= 210 + 120;

                bool towerTwo = leftPressed &&
                        x >= this->width - this->menuWidth + 5 &&
                        x <= this->width - 10 &&
                        y >= 210 &&
                        y <= 340 + 120;

                int i = (int) rint(x / this->tileSize);
                int j = (int) rint(y / this->tileSize);

                bool towerPlacement = leftPressed &&
                        i >= 0 && i < this->data->map_.size() &&
                        j >= 0 && j < this->data->map_[i].size() &&
                        this->data->map_[i][j] == PositionState::TOWER;

                if(towerOne) {
                    this->towerOneActive = !this->towerOneActive;
                    this->towerTwoActive = false;
                } else if(towerTwo) {
                    this->towerTwoActive = !this->towerTwoActive;
                    this->towerOneActive = false;
                } else if(towerPlacement && (this->towerOneActive || this->towerTwoActive || this->removeTowerActive)) {
                    Position<int> pos(i, j);
                    this->nextTowerOperation = new OperationTowerData(
                        (this->removeTowerActive ? TowerOperation::REMOVE : TowerOperation::INSERT), 
                        pos, (this->towerOneActive ? TowerType::SIMPLE : TowerType::COMPLEX));
                }

                
                
                break;
        }
    }

    return this->nextTowerOperation != nullptr || this->gameStatusChanged;
}

OperationTowerData* MapDrawer::getTowerOperation() {
    OperationTowerData* data = this->nextTowerOperation;
    this->nextTowerOperation = nullptr;

    return data;
}

GameStatus MapDrawer::getGameStatus() {
    this->gameStatusChanged = false;
    return this->gameStatus;
}

bool MapDrawer::isQuit() {
    return this->quit;
}

bool MapDrawer::loadTextures() {
    std::string folderPath = this->getCurrentDir() + "/textures/";

    // Full border
    std::string tilePath = folderPath + "tile01.png";
    SDL_Surface *tile = IMG_Load(tilePath.c_str());
    if (tile == nullptr){
        std::cout << "IMG_Load Error: " << SDL_GetError() << std::endl;
        return false;
    }

    SDL_Texture *tileTex = SDL_CreateTextureFromSurface(renderer, tile);
    SDL_FreeSurface(tile);
    if (tileTex == nullptr){
        std::cout << "SDL_CreateTextureFromSurface Error: " << SDL_GetError() << std::endl;
        return false;
    } else {
        this->textures->emplace("full_border", tileTex);
    }

    // Small corner border
    tilePath = folderPath + "tile02.png";
    tile = IMG_Load(tilePath.c_str());
    if (tile == nullptr){
        std::cout << "IMG_Load Error: " << SDL_GetError() << std::endl;
        return false;
    }

    tileTex = SDL_CreateTextureFromSurface(renderer, tile);
    SDL_FreeSurface(tile);
    if (tileTex == nullptr){
        std::cout << "SDL_CreateTextureFromSurface Error: " << SDL_GetError() << std::endl;
        return false;
    } else {
        this->textures->emplace("small_corner_border", tileTex);
    }

    // Big corner border
    tilePath = folderPath + "tile03.png";
    tile = IMG_Load(tilePath.c_str());
    if (tile == nullptr){
        std::cout << "IMG_Load Error: " << SDL_GetError() << std::endl;
        return false;
    }

    tileTex = SDL_CreateTextureFromSurface(renderer, tile);
    SDL_FreeSurface(tile);
    if (tileTex == nullptr){
        std::cout << "SDL_CreateTextureFromSurface Error: " << SDL_GetError() << std::endl;
        return false;
    } else {
        this->textures->emplace("big_corner_border", tileTex);
    }

    // Normal marker
    tilePath = folderPath + "tile04.png";
    tile = IMG_Load(tilePath.c_str());
    if (tile == nullptr){
        std::cout << "IMG_Load Error: " << SDL_GetError() << std::endl;
        return false;
    }

    tileTex = SDL_CreateTextureFromSurface(renderer, tile);
    SDL_FreeSurface(tile);
    if (tileTex == nullptr){
        std::cout << "SDL_CreateTextureFromSurface Error: " << SDL_GetError() << std::endl;
        return false;
    } else {
        this->textures->emplace("normal_marker", tileTex);
    }

    // Wrench marker
    tilePath = folderPath + "tile05.png";
    tile = IMG_Load(tilePath.c_str());
    if (tile == nullptr){
        std::cout << "IMG_Load Error: " << SDL_GetError() << std::endl;
        return false;
    }

    tileTex = SDL_CreateTextureFromSurface(renderer, tile);
    SDL_FreeSurface(tile);
    if (tileTex == nullptr){
        std::cout << "SDL_CreateTextureFromSurface Error: " << SDL_GetError() << std::endl;
        return false;
    } else {
        this->textures->emplace("wrench_marker", tileTex);
    }

    // Cross marker
    tilePath = folderPath + "tile06.png";
    tile = IMG_Load(tilePath.c_str());
    if (tile == nullptr){
        std::cout << "IMG_Load Error: " << SDL_GetError() << std::endl;
        return false;
    }

    tileTex = SDL_CreateTextureFromSurface(renderer, tile);
    SDL_FreeSurface(tile);
    if (tileTex == nullptr){
        std::cout << "SDL_CreateTextureFromSurface Error: " << SDL_GetError() << std::endl;
        return false;
    } else {
        this->textures->emplace("cross_marker", tileTex);
    }

    // Ball marker
    tilePath = folderPath + "tile07.png";
    tile = IMG_Load(tilePath.c_str());
    if (tile == nullptr){
        std::cout << "IMG_Load Error: " << SDL_GetError() << std::endl;
        return false;
    }

    tileTex = SDL_CreateTextureFromSurface(renderer, tile);
    SDL_FreeSurface(tile);
    if (tileTex == nullptr){
        std::cout << "SDL_CreateTextureFromSurface Error: " << SDL_GetError() << std::endl;
        return false;
    } else {
        this->textures->emplace("ball_marker", tileTex);
    }

    // Explosion marker
    tilePath = folderPath + "tile08.png";
    tile = IMG_Load(tilePath.c_str());
    if (tile == nullptr){
        std::cout << "IMG_Load Error: " << SDL_GetError() << std::endl;
        return false;
    }

    tileTex = SDL_CreateTextureFromSurface(renderer, tile);
    SDL_FreeSurface(tile);
    if (tileTex == nullptr){
        std::cout << "SDL_CreateTextureFromSurface Error: " << SDL_GetError() << std::endl;
        return false;
    } else {
        this->textures->emplace("explosion_marker", tileTex);
    }

    // Field
    tilePath = folderPath + "tile09.png";
    tile = IMG_Load(tilePath.c_str());
    if (tile == nullptr){
        std::cout << "IMG_Load Error: " << SDL_GetError() << std::endl;
        return false;
    }

    tileTex = SDL_CreateTextureFromSurface(renderer, tile);
    SDL_FreeSurface(tile);
    if (tileTex == nullptr){
        std::cout << "SDL_CreateTextureFromSurface Error: " << SDL_GetError() << std::endl;
        return false;
    } else {
        this->textures->emplace("field", tileTex);
    }

    // Path
    tilePath = folderPath + "tile10.png";
    tile = IMG_Load(tilePath.c_str());
    if (tile == nullptr){
        std::cout << "IMG_Load Error: " << SDL_GetError() << std::endl;
        return false;
    }

    tileTex = SDL_CreateTextureFromSurface(renderer, tile);
    SDL_FreeSurface(tile);
    if (tileTex == nullptr){
        std::cout << "SDL_CreateTextureFromSurface Error: " << SDL_GetError() << std::endl;
        return false;
    } else {
        this->textures->emplace("path", tileTex);
    }

    // Tower with one cannon
    tilePath = folderPath + "tower01.png";
    tile = IMG_Load(tilePath.c_str());
    if (tile == nullptr){
        std::cout << "IMG_Load Error: " << SDL_GetError() << std::endl;
        return false;
    }

    tileTex = SDL_CreateTextureFromSurface(renderer, tile);
    SDL_FreeSurface(tile);
    if (tileTex == nullptr){
        std::cout << "SDL_CreateTextureFromSurface Error: " << SDL_GetError() << std::endl;
        return false;
    } else {
        this->textures->emplace("tower_one", tileTex);
    }

    // Tower with two cannons
    tilePath = folderPath + "tower02.png";
    tile = IMG_Load(tilePath.c_str());
    if (tile == nullptr){
        std::cout << "IMG_Load Error: " << SDL_GetError() << std::endl;
        return false;
    }

    tileTex = SDL_CreateTextureFromSurface(renderer, tile);
    SDL_FreeSurface(tile);
    if (tileTex == nullptr){
        std::cout << "SDL_CreateTextureFromSurface Error: " << SDL_GetError() << std::endl;
        return false;
    } else {
        this->textures->emplace("tower_two", tileTex);
    }

    // Monster 1
    tilePath = folderPath + "monster01.png";
    tile = IMG_Load(tilePath.c_str());
    if (tile == nullptr){
        std::cout << "IMG_Load Error: " << SDL_GetError() << std::endl;
        return false;
    }

    tileTex = SDL_CreateTextureFromSurface(renderer, tile);
    SDL_FreeSurface(tile);
    if (tileTex == nullptr){
        std::cout << "SDL_CreateTextureFromSurface Error: " << SDL_GetError() << std::endl;
        return false;
    } else {
        this->textures->emplace("monster_one", tileTex);
    }

    // Monster 2
    tilePath = folderPath + "monster02.png";
    tile = IMG_Load(tilePath.c_str());
    if (tile == nullptr){
        std::cout << "IMG_Load Error: " << SDL_GetError() << std::endl;
        return false;
    }

    tileTex = SDL_CreateTextureFromSurface(renderer, tile);
    SDL_FreeSurface(tile);
    if (tileTex == nullptr){
        std::cout << "SDL_CreateTextureFromSurface Error: " << SDL_GetError() << std::endl;
        return false;
    } else {
        this->textures->emplace("monster_two", tileTex);
    }

    // Bullet 1
    tilePath = folderPath + "bullet01.png";
    tile = IMG_Load(tilePath.c_str());
    if (tile == nullptr){
        std::cout << "IMG_Load Error: " << SDL_GetError() << std::endl;
        return false;
    }

    tileTex = SDL_CreateTextureFromSurface(renderer, tile);
    SDL_FreeSurface(tile);
    if (tileTex == nullptr){
        std::cout << "SDL_CreateTextureFromSurface Error: " << SDL_GetError() << std::endl;
        return false;
    } else {
        this->textures->emplace("bullet_one", tileTex);
    }

    // Bullet 2
    tilePath = folderPath + "bullet02.png";
    tile = IMG_Load(tilePath.c_str());
    if (tile == nullptr){
        std::cout << "IMG_Load Error: " << SDL_GetError() << std::endl;
        return false;
    }

    tileTex = SDL_CreateTextureFromSurface(renderer, tile);
    SDL_FreeSurface(tile);
    if (tileTex == nullptr){
        std::cout << "SDL_CreateTextureFromSurface Error: " << SDL_GetError() << std::endl;
        return false;
    } else {
        this->textures->emplace("bullet_two", tileTex);
    }

    std::string fontPath = this->getCurrentDir() + "/fonts/sans.ttf";
    TTF_Font* sans = TTF_OpenFont(fontPath.c_str(), 24);
    SDL_Color black = {0, 0, 0};

    // Score text
    SDL_Surface* surfaceMessage = TTF_RenderText_Solid(sans, "SCORE", black);
    if(surfaceMessage == nullptr) {
        std::cout << "TTF_RenderText_Solid Error: " << SDL_GetError() << std::endl;
        return false;
    }

    SDL_Texture* message = SDL_CreateTextureFromSurface(renderer, surfaceMessage);
    SDL_FreeSurface(surfaceMessage);
    if(message == nullptr) {
        std::cout << "SDL_CreateTextureFromSurface Error: " << SDL_GetError() << std::endl;
        return false;
    } else {
        this->textures->emplace("score_text", message);
    }

    // Money text
    surfaceMessage = TTF_RenderText_Solid(sans, "MONEY", black);
    if(surfaceMessage == nullptr) {
        std::cout << "TTF_RenderText_Solid Error: " << SDL_GetError() << std::endl;
        return false;
    }

    message = SDL_CreateTextureFromSurface(renderer, surfaceMessage);
    SDL_FreeSurface(surfaceMessage);
    if(message == nullptr) {
        std::cout << "SDL_CreateTextureFromSurface Error: " << SDL_GetError() << std::endl;
        return false;
    } else {
        this->textures->emplace("money_text", message);
    }

    // Tower one price text
    surfaceMessage = TTF_RenderText_Solid(sans, "$050", black);
    if(surfaceMessage == nullptr) {
        std::cout << "TTF_RenderText_Solid Error: " << SDL_GetError() << std::endl;
        return false;
    }

    message = SDL_CreateTextureFromSurface(renderer, surfaceMessage);
    SDL_FreeSurface(surfaceMessage);
    if(message == nullptr) {
        std::cout << "SDL_CreateTextureFromSurface Error: " << SDL_GetError() << std::endl;
        return false;
    } else {
        this->textures->emplace("tower_one_price", message);
    }

    // Money text
    surfaceMessage = TTF_RenderText_Solid(sans, "$100", black);
    if(surfaceMessage == nullptr) {
        std::cout << "TTF_RenderText_Solid Error: " << SDL_GetError() << std::endl;
        return false;
    }

    message = SDL_CreateTextureFromSurface(renderer, surfaceMessage);
    SDL_FreeSurface(surfaceMessage);
    if(message == nullptr) {
        std::cout << "SDL_CreateTextureFromSurface Error: " << SDL_GetError() << std::endl;
        return false;
    } else {
        this->textures->emplace("tower_two_price", message);
    }

    return true;
}

void MapDrawer::unloadTextures() {
    std::map<std::string, SDL_Texture*>::iterator it = this->textures->begin();
    while (it != this->textures->end()) {
        cleanup(it->second);
        it = this->textures->erase(it);
    }
}

bool MapDrawer::initSuccessful() {
    return this->initStatus;
}

void MapDrawer::drawMenu() {
    SDL_Rect dest;

    // Menu background
    dest.x = this->width - this->menuWidth;
    dest.y = 0;
    dest.w = this->menuWidth;
    dest.h = this->height;

    SDL_SetRenderDrawColor(this->renderer, 211, 211, 211, 255);
    SDL_RenderFillRect(this->renderer, &dest);

    // Score text
    dest.x = this->width - this->menuWidth + 10;
    dest.y = 10;
    dest.w = this->menuWidth - 20;
    dest.h = 30;

    SDL_RenderCopy(this->renderer, this->textures->find("score_text")->second, nullptr, &dest);

    // Score box
    dest.x = this->width - this->menuWidth + 5;
    dest.y = 50;
    dest.w = this->menuWidth - 10;
    dest.h = 50;

    SDL_SetRenderDrawColor(this->renderer, 0, 0, 0, 255);
    SDL_RenderFillRect(this->renderer, &dest);

    this->drawScore();

    // Money text
    dest.x = this->width - this->menuWidth + 10;
    dest.y = 110;
    dest.w = this->menuWidth - 20;
    dest.h = 30;

    SDL_RenderCopy(this->renderer, this->textures->find("money_text")->second, nullptr, &dest);

    // Money box
    dest.x = this->width - this->menuWidth + 5;
    dest.y = 150;
    dest.w = this->menuWidth - 10;
    dest.h = 50;

    SDL_SetRenderDrawColor(this->renderer, 0, 0, 0, 255);
    SDL_RenderFillRect(this->renderer, &dest);

    this->drawMoney();

    this->drawMenuTowerOne();

    this->drawMenuTowerTwo();
}

void MapDrawer::drawScore() {
    std::string fontPath = this->getCurrentDir() + "/fonts/sans.ttf";
    TTF_Font* sans = TTF_OpenFont(fontPath.c_str(), 24);
    SDL_Color red = {255, 0, 0};

    SDL_Rect dest;

    dest.x = this->width - this->menuWidth + 10;
    dest.y = 60;
    dest.w = this->menuWidth - 20;
    dest.h = 30;

    // Score value text
    SDL_Surface* surfaceMessage = TTF_RenderText_Solid(sans, "0000", red);
    TTF_CloseFont(sans);
    if(surfaceMessage == nullptr) {
        std::cout << "TTF_RenderText_Solid Error: " << SDL_GetError() << std::endl;
        return;
    }

    SDL_Texture* message = SDL_CreateTextureFromSurface(renderer, surfaceMessage);
    SDL_FreeSurface(surfaceMessage);
    if(message == nullptr) {
        std::cout << "SDL_CreateTextureFromSurface Error: " << SDL_GetError() << std::endl;
        return;
    }

    SDL_RenderCopy(this->renderer, message, nullptr, &dest);

    cleanup(message);
}

void MapDrawer::drawMoney() {
    std::string fontPath = this->getCurrentDir() + "/fonts/sans.ttf";
    TTF_Font* sans = TTF_OpenFont(fontPath.c_str(), 24);
    SDL_Color red = {255, 0, 0};

    SDL_Rect dest;

    dest.x = this->width - this->menuWidth + 10;
    dest.y = 160;
    dest.w = this->menuWidth - 20;
    dest.h = 30;

    // Score text
    SDL_Surface* surfaceMessage = TTF_RenderText_Solid(sans, "$000", red);
    TTF_CloseFont(sans);
    if(surfaceMessage == nullptr) {
        std::cout << "TTF_RenderText_Solid Error: " << SDL_GetError() << std::endl;
        return;
    }

    SDL_Texture* message = SDL_CreateTextureFromSurface(renderer, surfaceMessage);
    SDL_FreeSurface(surfaceMessage);
    if(message == nullptr) {
        std::cout << "SDL_CreateTextureFromSurface Error: " << SDL_GetError() << std::endl;
        return;
    }

    SDL_RenderCopy(this->renderer, message, nullptr, &dest);

    cleanup(message);
}

void MapDrawer::drawMenuTowerOne() {
    SDL_Rect dest;

    // Tower one box
    dest.x = this->width - this->menuWidth + 5;
    dest.y = 210;
    dest.w = this->menuWidth - 10;
    dest.h = 120;

    if(this->towerOneActive)
        SDL_SetRenderDrawColor(this->renderer, 255, 0, 0, 255);
    else
        SDL_SetRenderDrawColor(this->renderer, 255, 255, 255, 255);

    SDL_RenderDrawRect(this->renderer, &dest);

    // Tower one icon
    dest.x = this->width - this->menuWidth + 10;
    dest.y = 215;
    dest.w = this->menuWidth - 20;
    dest.h = this->menuWidth - 20;

    SDL_RenderCopy(this->renderer, this->textures->find("tower_one")->second, nullptr, &dest);

    // Tower one price
    dest.x = this->width - this->menuWidth + 10;
    dest.y = 215 + this->menuWidth - 20;
    dest.w = this->menuWidth - 20;
    dest.h = 30;

    SDL_RenderCopy(this->renderer, this->textures->find("tower_one_price")->second, nullptr, &dest);
}

void MapDrawer::drawMenuTowerTwo() {
    SDL_Rect dest;

    // Tower two box
    dest.x = this->width - this->menuWidth + 5;
    dest.y = 340;
    dest.w = this->menuWidth - 10;
    dest.h = 120;

    if(this->towerTwoActive)
        SDL_SetRenderDrawColor(this->renderer, 255, 0, 0, 255);
    else
        SDL_SetRenderDrawColor(this->renderer, 255, 255, 255, 255);

    SDL_RenderDrawRect(this->renderer, &dest);

    // Tower two icon
    dest.x = this->width - this->menuWidth + 10;
    dest.y = 345;
    dest.w = this->menuWidth - 20;
    dest.h = this->menuWidth - 20;

    SDL_RenderCopy(this->renderer, this->textures->find("tower_two")->second, nullptr, &dest);

    // Tower two price
    dest.x = this->width - this->menuWidth + 10;
    dest.y = 345 + this->menuWidth - 20;
    dest.w = this->menuWidth - 20;
    dest.h = 30;

    SDL_RenderCopy(this->renderer, this->textures->find("tower_two_price")->second, nullptr, &dest);
}

void MapDrawer::drawField(int x, int y) {
    SDL_Rect dest;

    int tmp_x = x * this->tileSize;
    int tmp_y = y * this->tileSize;

    dest.x = tmp_x;
    dest.y = tmp_y;
    dest.w = this->tileSize;
    dest.h = this->tileSize;

    SDL_RenderCopy(this->renderer, this->textures->find("field")->second, nullptr, &dest);
}

void MapDrawer::drawRoadStraight(int x, int y, bool vertical) {
    SDL_Rect upper_left;
    SDL_Rect upper_right;
    SDL_Rect lower_left;
    SDL_Rect lower_right;

    int tmp_x = x * this->tileSize;
    int tmp_y = y * this->tileSize;

    upper_left.x = tmp_x;
    upper_left.y = tmp_y;
    upper_left.h = this->tileSize / 2;
    upper_left.w = this->tileSize / 2;

    upper_right.x = tmp_x + this->tileSize / 2;
    upper_right.y = tmp_y;
    upper_right.h = this->tileSize / 2;
    upper_right.w = this->tileSize / 2;

    lower_left.x = tmp_x;
    lower_left.y = tmp_y + this->tileSize / 2;
    lower_left.h = this->tileSize / 2;
    lower_left.w = this->tileSize / 2;

    lower_right.x = tmp_x + this->tileSize / 2;
    lower_right.y = tmp_y + this->tileSize / 2;
    lower_right.h = this->tileSize / 2;
    lower_right.w = this->tileSize / 2;

    SDL_Texture *texture = this->textures->find("full_border")->second;

    if (!vertical) {
        SDL_RenderCopyEx(this->renderer, texture, nullptr, &upper_left, 0, nullptr, SDL_FLIP_VERTICAL);
        SDL_RenderCopyEx(this->renderer, texture, nullptr, &upper_right, 0, nullptr, SDL_FLIP_VERTICAL);
        SDL_RenderCopyEx(this->renderer, texture, nullptr, &lower_left, 0, nullptr, SDL_FLIP_NONE);
        SDL_RenderCopyEx(this->renderer, texture, nullptr, &lower_right, 0, nullptr, SDL_FLIP_NONE);
    } else {
        SDL_RenderCopyEx(this->renderer, texture, nullptr, &upper_left, 90, nullptr, SDL_FLIP_NONE);
        SDL_RenderCopyEx(this->renderer, texture, nullptr, &upper_right, -90, nullptr, SDL_FLIP_NONE);
        SDL_RenderCopyEx(this->renderer, texture, nullptr, &lower_left, 90, nullptr, SDL_FLIP_NONE);
        SDL_RenderCopyEx(this->renderer, texture, nullptr, &lower_right, -90, nullptr, SDL_FLIP_NONE);
    }
}

void MapDrawer::drawRoadCorner(int x, int y, corner_side corner) {
    SDL_Rect upper_left;
    SDL_Rect upper_right;
    SDL_Rect lower_left;
    SDL_Rect lower_right;

    int tmp_x = x * this->tileSize;
    int tmp_y = y * this->tileSize;

    SDL_Texture* small_corner = this->textures->find("small_corner_border")->second;
    SDL_Texture* big_corner = this->textures->find("big_corner_border")->second;
    SDL_Texture* full_border = this->textures->find("full_border")->second;

    upper_left.x = tmp_x;
    upper_left.y = tmp_y;
    upper_left.h = this->tileSize / 2;
    upper_left.w = this->tileSize / 2;

    upper_right.x = tmp_x + this->tileSize / 2;
    upper_right.y = tmp_y;
    upper_right.h = this->tileSize / 2;
    upper_right.w = this->tileSize / 2;

    lower_left.x = tmp_x;
    lower_left.y = tmp_y + this->tileSize / 2;
    lower_left.h = this->tileSize / 2;
    lower_left.w = this->tileSize / 2;

    lower_right.x = tmp_x + this->tileSize / 2;
    lower_right.y = tmp_y + this->tileSize / 2;
    lower_right.h = this->tileSize / 2;
    lower_right.w = this->tileSize / 2;

    switch (corner) {
        case LEFT_UP:
            SDL_RenderCopyEx(this->renderer, small_corner, nullptr, &upper_left, 0, nullptr, SDL_FLIP_VERTICAL);
            SDL_RenderCopyEx(this->renderer, full_border, nullptr, &upper_right, 270, nullptr, SDL_FLIP_NONE);
            SDL_RenderCopy(this->renderer, full_border, nullptr, &lower_left);
            SDL_RenderCopyEx(this->renderer, big_corner, nullptr, &lower_right, 180, nullptr, SDL_FLIP_NONE);
            break;
        case LEFT_DOWN:
            SDL_RenderCopyEx(this->renderer, full_border, nullptr, &upper_left, 0, nullptr, SDL_FLIP_VERTICAL);
            SDL_RenderCopyEx(this->renderer, big_corner, nullptr, &upper_right, 0, nullptr, SDL_FLIP_HORIZONTAL);
            SDL_RenderCopy(this->renderer, small_corner, nullptr, &lower_left);
            SDL_RenderCopyEx(this->renderer, full_border, nullptr, &lower_right, 270, nullptr, SDL_FLIP_NONE);
            break;
        case RIGHT_UP:
            SDL_RenderCopyEx(this->renderer, full_border, nullptr, &upper_left, 90, nullptr, SDL_FLIP_NONE);
            SDL_RenderCopyEx(this->renderer, small_corner, nullptr, &upper_right, 180, nullptr, SDL_FLIP_NONE);
            SDL_RenderCopyEx(this->renderer, big_corner, nullptr, &lower_left, 0, nullptr, SDL_FLIP_VERTICAL);
            SDL_RenderCopy(this->renderer, full_border, nullptr, &lower_right);
            break;
        case RIGHT_DOWN:
            SDL_RenderCopy(this->renderer, big_corner, nullptr, &upper_left);
            SDL_RenderCopyEx(this->renderer, full_border, nullptr, &upper_right, 0, nullptr, SDL_FLIP_VERTICAL);
            SDL_RenderCopyEx(this->renderer, full_border, nullptr, &lower_left, 90, nullptr, SDL_FLIP_NONE);
            SDL_RenderCopyEx(this->renderer, small_corner, nullptr, &lower_right, 0, nullptr, SDL_FLIP_HORIZONTAL);
            break;
    }
}

void MapDrawer::drawInsertPosition(int x, int y, marker_type marker) {
    SDL_Rect dest;

    int tmp_x = x * this->tileSize;
    int tmp_y = y * this->tileSize;

    dest.x = tmp_x;
    dest.y = tmp_y;
    dest.w = this->tileSize;
    dest.h = this->tileSize;

    switch (marker) {
        case NORMAL:
            SDL_RenderCopy(this->renderer, this->textures->find("normal_marker")->second, nullptr, &dest);
            break;
        case WRENCH:
            SDL_RenderCopy(this->renderer, this->textures->find("wrench_marker")->second, nullptr, &dest);
            break;
        case CROSS:
            SDL_RenderCopy(this->renderer, this->textures->find("cross_marker")->second, nullptr, &dest);
            break;
        case BALL:
            SDL_RenderCopy(this->renderer, this->textures->find("ball_marker")->second, nullptr, &dest);
            break;
        case EXPLOSION:
            SDL_RenderCopy(this->renderer, this->textures->find("explosion_marker")->second, nullptr, &dest);
            break;
    }
}

void MapDrawer::drawTower(int x, int y, int angle, tower_type tower) {
    SDL_Rect dest;

    int tmp_x = x * this->tileSize;
    int tmp_y = y * this->tileSize;

    dest.x = tmp_x;
    dest.y = tmp_y;
    dest.w = this->tileSize;
    dest.h = this->tileSize;

    switch (tower) {
        case ONE_CANNON:
            SDL_RenderCopyEx(this->renderer, this->textures->find("tower_one")->second, nullptr, &dest, -angle+90, nullptr,
                             SDL_FLIP_NONE);
            break;
        case TWO_CANNON:
            SDL_RenderCopyEx(this->renderer, this->textures->find("tower_two")->second, nullptr, &dest, -angle+90, nullptr,
                             SDL_FLIP_NONE);
            break;
    }
}

void MapDrawer::drawMonster(double x, double y, int angle, monster_type monster) {
    SDL_Rect dest;

    int tmp_x = (int) (rint(x * this->tileSize) - this->tileSize / 2);
    int tmp_y = (int) (rint(y * this->tileSize) - this->tileSize / 2);

    dest.x = tmp_x;
    dest.y = tmp_y;
    dest.w = this->tileSize;
    dest.h = this->tileSize;

    switch (monster) {
        case MONSTER_1:
            SDL_RenderCopyEx(this->renderer, this->textures->find("monster_one")->second, nullptr, &dest, -angle, nullptr,
                             SDL_FLIP_NONE);
            break;
        case MONSTER_2:
            SDL_RenderCopyEx(this->renderer, this->textures->find("monster_two")->second, nullptr, &dest, -angle, nullptr,
                             SDL_FLIP_NONE);
            break;
    }
}

void MapDrawer::drawBullet(double x, double y, bullet_type bullet) {
    SDL_Rect dest;

    int tmp_x = (int) (rint(x * this->tileSize) - this->tileSize / 2);
    int tmp_y = (int) (rint(y * this->tileSize) - this->tileSize / 2);

    dest.x = tmp_x;
    dest.y = tmp_y;
    dest.w = this->tileSize;
    dest.h = this->tileSize;

    switch (bullet) {
        case BULLET_1:
            SDL_RenderCopy(this->renderer, this->textures->find("bullet_one")->second, nullptr, &dest);
            break;
        case BULLET_2:
            SDL_RenderCopy(this->renderer, this->textures->find("bullet_two")->second, nullptr, &dest);
            break;
    }
}

int MapDrawer::getDegrees(double radians) {
    constexpr double halfC = 180.0 / M_PI;
    return rint(radians * halfC);
}

std::string MapDrawer::getCurrentDir() {
    char cwd[1024];
    getcwd(cwd, sizeof(cwd));
    std::string path(cwd);
    size_t pos = path.find("/tower-defense");
    std::cout << path.substr(0, pos + 15) << std::endl;
    return path.substr(0, pos + 15);
}
