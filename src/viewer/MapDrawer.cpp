#include <iostream>
#include <sstream>
#include <iomanip>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_ttf.h>
#include <unistd.h>
#include "MapDrawer.h"
#include "aux/Cleanup.h"

MapDrawer::MapDrawer(int width, int height) : viewerData(std::make_unique<GameStatusData>(GameStatus::PLAY)) {
    this->width = width;
    this->height = height;
    this->tileSize = 80;
    this->menuWidth = 100;
    this->towerOneActive = false;
    this->towerTwoActive = false;
    this->removeTowerActive = false;
    this->quit = false;
    this->gameStatusChanged = false;

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
    this->folderPath = this->getCurrentDir();

    if(SDL_Init(SDL_INIT_VIDEO | SDL_INIT_EVENTS) != 0) {
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
    this->dataVector.push_back(data);
}

void MapDrawer::drawMap() {
    SDL_RenderClear(this->renderer);
    SDL_SetRenderDrawColor(this->renderer, 255, 255, 255, 255);

    if(!this->dataVector.empty()) {
        delete this->data;
        this->data = this->dataVector.front();
        this->dataVector.erase(this->dataVector.begin());
    } else {
        this->data = nullptr;
    }

    if(data != nullptr) {
        std::string windowTitle = "Tower Defense - Level " + std::to_string(this->data->level_);
        SDL_SetWindowTitle(this->window, windowTitle.c_str());
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

        this->drawInsertPosition(this->data->start_.get_x(), this->data->start_.get_y(), marker_type::BALL);
        this->drawInsertPosition(this->data->end_.get_x(), this->data->end_.get_y(), marker_type::CROSS);

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
}

bool MapDrawer::handleEvents() {
    SDL_Event e;

    bool modifications = false;

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
                        y >= 285 &&
                        y <= 285 + 120;

                bool towerTwo = leftPressed &&
                        x >= this->width - this->menuWidth + 5 &&
                        x <= this->width - 10 &&
                        y >= 410 &&
                        y <= 410 + 120;

                bool demolishActivation = leftPressed &&
                        x >= this->width - this->menuWidth + 5 &&
                        x <= this->width - 10 &&
                        y >= 535 &&
                        y <= 535 + 50;

                bool playActivation = leftPressed &&
                        x >= this->width - this->menuWidth + 5 &&
                        x <= this->width - 10 &&
                        y >= this->height - 110 &&
                        y <= this->height - 60;

                bool pauseActivation = leftPressed &&
                        x >= this->width - this->menuWidth + 5 &&
                        x <= this->width - 10 &&
                        y >= this->height - 55 &&
                        y <= this->height - 5;

                int i = (int) rint(x / this->tileSize);
                int j = (int) rint(y / this->tileSize);

                bool towerPlacement = leftPressed && this->data != nullptr &&
                        i >= 0 && i < this->data->map_.size() &&
                        j >= 0 && j < this->data->map_[0].size();


                if(playActivation) {
                    modifications = true;
                    this->viewerData = std::make_unique<GameStatusData>(GameStatus::PLAY);
                } else if(pauseActivation) {
                    modifications = true;
                    this->viewerData = std::make_unique<GameStatusData>(GameStatus::PAUSE);
                }

                if(towerOne) {
                    this->towerOneActive = !this->towerOneActive;
                    this->towerTwoActive = false;
                    this->removeTowerActive = false;
                } else if(towerTwo) {
                    this->towerTwoActive = !this->towerTwoActive;
                    this->towerOneActive = false;
                    this->removeTowerActive = false;
                } else if(demolishActivation) {
                    this->removeTowerActive = !this->removeTowerActive;
                    this->towerOneActive = false;
                    this->towerTwoActive = false;
                }

                if(towerPlacement && (this->towerOneActive || this->towerTwoActive || this->removeTowerActive)) {
                    modifications = true;
                    Position<int> pos(i, j);
                    this->viewerData = std::make_unique<OperationTowerData>(
                        (this->removeTowerActive ? TowerOperation::REMOVE : TowerOperation::INSERT), 
                        pos, (this->towerOneActive ? TowerType::SIMPLE : TowerType::COMPLEX));
                }
                break;
        }
    }
    return modifications;
}

const std::unique_ptr<ViewerData>& MapDrawer::getViewerData() const {
    return viewerData;
}

bool MapDrawer::isQuit() {
    return this->quit;
}

bool MapDrawer::loadTextures() {
    std::string folderPath = this->folderPath + "/textures/";

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

    std::string fontPath = this->folderPath + "/fonts/sans.ttf";
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

    // Sell text
    surfaceMessage = TTF_RenderText_Solid(sans, "SELL", black);
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
        this->textures->emplace("demolish_text", message);
    }

    // Play text
    surfaceMessage = TTF_RenderText_Solid(sans, "PLAY", black);
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
        this->textures->emplace("play_text", message);
    }

    // Pause text
    surfaceMessage = TTF_RenderText_Solid(sans, "PAUSE", black);
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
        this->textures->emplace("pause_text", message);
    }

    // Lives text
    surfaceMessage = TTF_RenderText_Solid(sans, "LIVES", black);
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
        this->textures->emplace("lives_text", message);
    }

    // Monsters text
    surfaceMessage = TTF_RenderText_Solid(sans, "LEFT", black);
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
        this->textures->emplace("monsters_text", message);
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

    this->drawMenuScore();
    this->drawMenuMoney();
    this->drawMenuLives();
    this->drawMenuLeftMonsters();
    this->drawMenuTowerOne();
    this->drawMenuTowerTwo();
    this->drawMenuDemolishButton();
    this->drawMenuPlayButton();
    this->drawMenuPauseButton();
}

void MapDrawer::drawMenuScore() {
    int init_x = this->width - this->menuWidth + 10;
    int init_y = 5;

    SDL_Rect dest;

    // Score text
    dest.x = init_x;
    dest.y = init_y;
    dest.w = this->menuWidth - 20;
    dest.h = 25;

    SDL_RenderCopy(this->renderer, this->textures->find("score_text")->second, nullptr, &dest);

    // Score box
    dest.x = init_x - 5;
    dest.y = init_y + 30;
    dest.w = this->menuWidth - 10;
    dest.h = 35;

    SDL_SetRenderDrawColor(this->renderer, 0, 0, 0, 255);
    SDL_RenderFillRect(this->renderer, &dest);

    std::string fontPath = this->folderPath + "/fonts/sans.ttf";
    TTF_Font* sans = TTF_OpenFont(fontPath.c_str(), 24);
    SDL_Color red = {255, 0, 0};

    dest.x = init_x;
    dest.y = init_y + 35;
    dest.w = this->menuWidth - 20;
    dest.h = 25;

    std::stringstream ss;
    ss << std::setw(5) << std::setfill('0') << this->data->score_;
    std::string score_text = ss.str();

    // Score value text
    SDL_Surface* surfaceMessage = TTF_RenderText_Solid(sans, score_text.c_str(), red);
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

void MapDrawer::drawMenuMoney() {
    int init_x = this->width - this->menuWidth + 10;
    int init_y = 75;

    SDL_Rect dest;

    // Money text
    dest.x = init_x;
    dest.y = init_y;
    dest.w = this->menuWidth - 20;
    dest.h = 25;

    SDL_RenderCopy(this->renderer, this->textures->find("money_text")->second, nullptr, &dest);

    // Money box
    dest.x = init_x - 5;
    dest.y = init_y + 30;
    dest.w = this->menuWidth - 10;
    dest.h = 35;

    SDL_SetRenderDrawColor(this->renderer, 0, 0, 0, 255);
    SDL_RenderFillRect(this->renderer, &dest);

    std::string fontPath = this->folderPath + "/fonts/sans.ttf";
    TTF_Font* sans = TTF_OpenFont(fontPath.c_str(), 24);
    SDL_Color red = {255, 0, 0};

    dest.x = init_x;
    dest.y = init_y + 35;
    dest.w = this->menuWidth - 20;
    dest.h = 25;

    // Score text
    std::stringstream ss;
    ss << std::setw(5) << std::setfill('0') << this->data->player_currency_;
    std::string money_text = "$" + ss.str();

    SDL_Surface* surfaceMessage = TTF_RenderText_Solid(sans, money_text.c_str(), red);
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

void MapDrawer::drawMenuLives() {
    int init_x = this->width - this->menuWidth + 10;
    int init_y = 145;

    SDL_Rect dest;

    // Money text
    dest.x = init_x;
    dest.y = init_y;
    dest.w = this->menuWidth - 20;
    dest.h = 25;

    SDL_RenderCopy(this->renderer, this->textures->find("lives_text")->second, nullptr, &dest);

    // Money box
    dest.x = init_x - 5;
    dest.y = init_y + 30;
    dest.w = this->menuWidth - 10;
    dest.h = 35;

    SDL_SetRenderDrawColor(this->renderer, 0, 0, 0, 255);
    SDL_RenderFillRect(this->renderer, &dest);

    std::string fontPath = this->folderPath + "/fonts/sans.ttf";
    TTF_Font* sans = TTF_OpenFont(fontPath.c_str(), 24);
    SDL_Color red = {255, 0, 0};

    dest.x = init_x;
    dest.y = init_y + 35;
    dest.w = this->menuWidth - 20;
    dest.h = 25;

    // Score text
    std::stringstream ss;
    ss << std::setw(5) << std::setfill('0') << this->data->lives_;
    std::string money_text = ss.str();

    SDL_Surface* surfaceMessage = TTF_RenderText_Solid(sans, money_text.c_str(), red);
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

void MapDrawer::drawMenuLeftMonsters() {
    int init_x = this->width - this->menuWidth + 10;
    int init_y = 215;

    SDL_Rect dest;

    // Money text
    dest.x = init_x;
    dest.y = init_y;
    dest.w = this->menuWidth - 20;
    dest.h = 25;

    SDL_RenderCopy(this->renderer, this->textures->find("monsters_text")->second, nullptr, &dest);

    // Money box
    dest.x = init_x - 5;
    dest.y = init_y + 30;
    dest.w = this->menuWidth - 10;
    dest.h = 35;

    SDL_SetRenderDrawColor(this->renderer, 0, 0, 0, 255);
    SDL_RenderFillRect(this->renderer, &dest);

    std::string fontPath = this->folderPath + "/fonts/sans.ttf";
    TTF_Font* sans = TTF_OpenFont(fontPath.c_str(), 24);
    SDL_Color red = {255, 0, 0};

    dest.x = init_x;
    dest.y = init_y + 35;
    dest.w = this->menuWidth - 20;
    dest.h = 25;

    // Score text
    std::stringstream ss;
    ss << std::setw(5) << std::setfill('0') << this->data->monsters_left_level_;
    std::string money_text = ss.str();

    SDL_Surface* surfaceMessage = TTF_RenderText_Solid(sans, money_text.c_str(), red);
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
    int init_x = this->width - this->menuWidth + 5;
    int init_y = 285;

    SDL_Rect dest;

    // Tower one box
    dest.x = init_x;
    dest.y = init_y;
    dest.w = this->menuWidth - 10;
    dest.h = 120;

    if(this->towerOneActive)
        SDL_SetRenderDrawColor(this->renderer, 255, 0, 0, 255);
    else
        SDL_SetRenderDrawColor(this->renderer, 255, 255, 255, 255);

    SDL_RenderDrawRect(this->renderer, &dest);

    // Tower one icon
    dest.x = init_x + 5;
    dest.y = init_y + 5;
    dest.w = this->menuWidth - 20;
    dest.h = this->menuWidth - 20;

    SDL_RenderCopy(this->renderer, this->textures->find("tower_one")->second, nullptr, &dest);

    // Tower one price
    dest.x = init_x + 5;
    dest.y = init_y + this->menuWidth - 15;
    dest.w = this->menuWidth - 20;
    dest.h = 30;

    SDL_RenderCopy(this->renderer, this->textures->find("tower_one_price")->second, nullptr, &dest);
}

void MapDrawer::drawMenuTowerTwo() {
    int init_x = this->width - this->menuWidth + 5;
    int init_y = 410;

    SDL_Rect dest;

    // Tower two box
    dest.x = init_x;
    dest.y = init_y;
    dest.w = this->menuWidth - 10;
    dest.h = 120;

    if(this->towerTwoActive)
        SDL_SetRenderDrawColor(this->renderer, 255, 0, 0, 255);
    else
        SDL_SetRenderDrawColor(this->renderer, 255, 255, 255, 255);

    SDL_RenderDrawRect(this->renderer, &dest);

    // Tower two icon
    dest.x = init_x + 5;
    dest.y = init_y + 5;
    dest.w = this->menuWidth - 20;
    dest.h = this->menuWidth - 20;

    SDL_RenderCopy(this->renderer, this->textures->find("tower_two")->second, nullptr, &dest);

    // Tower two price
    dest.x = init_x + 5;
    dest.y = init_y + this->menuWidth - 15;
    dest.w = this->menuWidth - 20;
    dest.h = 30;

    SDL_RenderCopy(this->renderer, this->textures->find("tower_two_price")->second, nullptr, &dest);
}

void MapDrawer::drawMenuDemolishButton() {
    int init_x = this->width - this->menuWidth + 5;
    int init_y = 535;

    SDL_Rect dest;

    // Demolish box
    dest.x = init_x;
    dest.y = init_y;
    dest.w = this->menuWidth - 10;
    dest.h = 50;

    if(this->removeTowerActive)
        SDL_SetRenderDrawColor(this->renderer, 255, 0, 0, 255);
    else
        SDL_SetRenderDrawColor(this->renderer, 255, 255, 255, 255);

    SDL_RenderDrawRect(this->renderer, &dest);

    // Demolish text
    dest.x = init_x + 5;
    dest.y = init_y + 10;
    dest.w = this->menuWidth - 20;
    dest.h = 30;

    SDL_RenderCopy(this->renderer, this->textures->find("demolish_text")->second, nullptr, &dest);
}

void MapDrawer::drawMenuPlayButton() {
    int init_x = this->width - this->menuWidth + 5;
    int init_y = this->height - 110;

    SDL_Rect dest;

    // Tower two box
    dest.x = init_x;
    dest.y = init_y;
    dest.w = this->menuWidth - 10;
    dest.h = 50;

    if(viewerData->get_type() == ViewerRequest::GAME_STATUS &&
       dynamic_cast<GameStatusData*>(viewerData.get())->status_ == GameStatus::PLAY)
        SDL_SetRenderDrawColor(this->renderer, 255, 0, 0, 255);
    else
        SDL_SetRenderDrawColor(this->renderer, 255, 255, 255, 255);

    SDL_RenderDrawRect(this->renderer, &dest);

    // Tower two price
    dest.x = init_x + 5;
    dest.y = init_y + 10;
    dest.w = this->menuWidth - 20;
    dest.h = 30;

    SDL_RenderCopy(this->renderer, this->textures->find("play_text")->second, nullptr, &dest);
}

void MapDrawer::drawMenuPauseButton() {
    int init_x = this->width - this->menuWidth + 5;
    int init_y = this->height - 55;

    SDL_Rect dest;

    // Tower two box
    dest.x = init_x;
    dest.y = init_y;
    dest.w = this->menuWidth - 10;
    dest.h = 50;

    if(viewerData->get_type() == ViewerRequest::GAME_STATUS &&
       dynamic_cast<GameStatusData*>(viewerData.get())->status_ == GameStatus::PAUSE)
        SDL_SetRenderDrawColor(this->renderer, 255, 0, 0, 255);
    else
        SDL_SetRenderDrawColor(this->renderer, 255, 255, 255, 255);

    SDL_RenderDrawRect(this->renderer, &dest);

    // Tower two price
    dest.x = init_x + 5;
    dest.y = init_y + 10;
    dest.w = this->menuWidth - 20;
    dest.h = 30;

    SDL_RenderCopy(this->renderer, this->textures->find("pause_text")->second, nullptr, &dest);
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
    return path.substr(0, pos + 15);
}
