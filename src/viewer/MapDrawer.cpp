#include <iostream>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_ttf.h>
#include "MapDrawer.h"
#include "aux/Cleanup.h"

MapDrawer::MapDrawer(int width, int height, Map *map) {
    this->width = width;
    this->height = height;
    this->tileSize = 80;
    this->menuWidth = 100;
    this->map = map;

    this->textures = new std::map<std::string, SDL_Texture*>();
    this->monsters = new std::vector<Monster*>();
    this->towers = new std::vector<Tower*>();
    this->bullets = new std::vector<Bullet*>();

    this->initStatus = init();

    if(initStatus)
        this->initStatus &= loadTextures();

    if(!initStatus)
        unloadTextures();
}

MapDrawer::~MapDrawer() {
    delete this->map;
    this->map = nullptr;

    delete this->monsters;
    this->monsters = nullptr;

    delete this->towers;
    this->towers = nullptr;

    if(initStatus) {
        unloadTextures();

        delete this->textures;
        this->textures = nullptr;

        cleanup(this->renderer, this->window);
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

    this->window = SDL_CreateWindow("Hello World!", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, this->width, this->height, SDL_WINDOW_SHOWN);
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

bool MapDrawer::loadTextures() {
    std::string folderPath = "/home/ed1000/Documents/tower-defense/textures/";

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

    folderPath = "/home/ed1000/Documents/tower-defense/fonts/";
    std::string fontPath = folderPath + "sans.ttf";
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

void MapDrawer::updateMonster(Monster* monster, UpdateAction action) {}

void MapDrawer::updateTower(Tower* tower, UpdateAction action) {}

void MapDrawer::updateBullet(Bullet *bullet, UpdateAction action) {}

void MapDrawer::drawMap() {
    SDL_RenderClear(this->renderer);
    SDL_SetRenderDrawColor(this->renderer, 255, 255, 255, 255);

    for(int x = 0; x < this->width; x += this->tileSize)
        for(int y = 0; y < this->height; y += this->tileSize) {
            if(y % (this->tileSize * 2) == 0)
                drawField(x, y);
            else
                drawRoadStraight(x, y);
        }

    for(int x = 0; x < this->width; x += this->tileSize)
        for(int y = 0; y < this->height; y += this->tileSize * 2)
            drawInsertPosition(x, y, NORMAL);

    for(int x = 0; x < this->width; x += this->tileSize)
        for(int y = 0; y < this->height; y += this->tileSize * 2) {
            if (x % (this->tileSize * 2) == 0)
                drawTower(x, y, 45, ONE_CANNON);
            else
                drawTower(x, y, -45, TWO_CANNON);
        }

    for(int x = 0; x < this->width; x += this->tileSize)
        for(int y = this->tileSize; y < this->height; y += this->tileSize * 2) {
            if (x % (this->tileSize * 2) == 0)
                drawMonster(x, y, 0, MONSTER_1);
            else
                drawMonster(x, y, -180, MONSTER_2);
        }

    for(int x = this->tileSize / 2; x < this->width; x += this->tileSize)
        for(int y = this->tileSize / 2; y < this->height; y += this->tileSize) {
            drawBullet(x, y, BULLET_1);
        }

    drawMenu();

    SDL_RenderPresent(this->renderer);
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

    // Tower one box
    dest.x = this->width - this->menuWidth + 5;
    dest.y = 210;
    dest.w = this->menuWidth - 10;
    dest.h = 120;

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

    // Tower two box
    dest.x = this->width - this->menuWidth + 5;
    dest.y = 340;
    dest.w = this->menuWidth - 10;
    dest.h = 120;

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

void MapDrawer::drawScore() {
    std::string fontPath = "/home/ed1000/Documents/tower-defense/fonts/sans.ttf";
    TTF_Font* sans = TTF_OpenFont(fontPath.c_str(), 24);
    SDL_Color red = {255, 0, 0};

    SDL_Rect dest;

    dest.x = this->width - this->menuWidth + 10;
    dest.y = 60;
    dest.w = this->menuWidth - 20;
    dest.h = 30;

    // Score value text
    SDL_Surface* surfaceMessage = TTF_RenderText_Solid(sans, "0000", red);
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
    std::string fontPath = "/home/ed1000/Documents/tower-defense/fonts/sans.ttf";
    TTF_Font* sans = TTF_OpenFont(fontPath.c_str(), 24);
    SDL_Color red = {255, 0, 0};

    SDL_Rect dest;

    dest.x = this->width - this->menuWidth + 10;
    dest.y = 160;
    dest.w = this->menuWidth - 20;
    dest.h = 30;

    // Score text
    SDL_Surface* surfaceMessage = TTF_RenderText_Solid(sans, "$000", red);
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

void MapDrawer::drawField(int x, int y) {
    SDL_Rect dest;

    dest.x = x;
    dest.y = y;
    dest.w = this->tileSize;
    dest.h = this->tileSize;

    SDL_RenderCopy(this->renderer, this->textures->find("field")->second, nullptr, &dest);
}

void MapDrawer::drawRoadStraight(int x, int y) {
    SDL_Rect upper_left;
    SDL_Rect upper_right;
    SDL_Rect lower_left;
    SDL_Rect lower_right;

    upper_left.x = x;
    upper_left.y = y;
    upper_left.h = this->tileSize / 2;
    upper_left.w = this->tileSize / 2;

    upper_right.x = x + this->tileSize / 2;
    upper_right.y = y;
    upper_right.h = this->tileSize / 2;
    upper_right.w = this->tileSize / 2;

    lower_left.x = x;
    lower_left.y = y + this->tileSize / 2;
    lower_left.h = this->tileSize / 2;
    lower_left.w = this->tileSize / 2;

    lower_right.x = x + this->tileSize / 2;
    lower_right.y = y + this->tileSize / 2;
    lower_right.h = this->tileSize / 2;
    lower_right.w = this->tileSize / 2;

    SDL_Texture* texture = this->textures->find("full_border")->second;

    SDL_RenderCopyEx(this->renderer, texture, nullptr, &upper_left, 0, nullptr, SDL_FLIP_VERTICAL);
    SDL_RenderCopyEx(this->renderer, texture, nullptr, &upper_right, 0, nullptr, SDL_FLIP_VERTICAL);
    SDL_RenderCopy(this->renderer, texture, nullptr, &lower_left);
    SDL_RenderCopy(this->renderer, texture, nullptr, &lower_right);
}

void MapDrawer::drawRoadCorner(int x, int y, corner_side corner) {
    SDL_Rect upper_left;
    SDL_Rect upper_right;
    SDL_Rect lower_left;
    SDL_Rect lower_right;

    SDL_Texture* small_corner = this->textures->find("small_corner_border")->second;
    SDL_Texture* big_corner = this->textures->find("big_corner_border")->second;
    SDL_Texture* full_border = this->textures->find("full_border")->second;

    upper_left.x = x;
    upper_left.y = y;
    upper_left.h = this->tileSize / 2;
    upper_left.w = this->tileSize / 2;

    upper_right.x = x + this->tileSize / 2;
    upper_right.y = y;
    upper_right.h = this->tileSize / 2;
    upper_right.w = this->tileSize / 2;

    lower_left.x = x;
    lower_left.y = y + this->tileSize / 2;
    lower_left.h = this->tileSize / 2;
    lower_left.w = this->tileSize / 2;

    lower_right.x = x + this->tileSize / 2;
    lower_right.y = y + this->tileSize / 2;
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

    dest.x = x;
    dest.y = y;
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

    dest.x = x;
    dest.y = y;
    dest.w = this->tileSize;
    dest.h = this->tileSize;

    switch (tower) {
        case ONE_CANNON:
            SDL_RenderCopyEx(this->renderer, this->textures->find("tower_one")->second, nullptr, &dest, angle, nullptr,
                             SDL_FLIP_NONE);
            break;
        case TWO_CANNON:
            SDL_RenderCopyEx(this->renderer, this->textures->find("tower_two")->second, nullptr, &dest, angle, nullptr,
                             SDL_FLIP_NONE);
            break;
    }
}

void MapDrawer::drawMonster(int x, int y, int angle, monster_type monster) {
    SDL_Rect dest;

    dest.x = x;
    dest.y = y;
    dest.w = this->tileSize;
    dest.h = this->tileSize;

    switch (monster) {
        case MONSTER_1:
            SDL_RenderCopyEx(this->renderer, this->textures->find("monster_one")->second, nullptr, &dest, angle, nullptr,
                             SDL_FLIP_NONE);
            break;
        case MONSTER_2:
            SDL_RenderCopyEx(this->renderer, this->textures->find("monster_two")->second, nullptr, &dest, angle, nullptr,
                             SDL_FLIP_NONE);
            break;
    }
}

void MapDrawer::drawBullet(int x, int y, bullet_type bullet) {
    SDL_Rect dest;

    dest.x = x;
    dest.y = y;
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
