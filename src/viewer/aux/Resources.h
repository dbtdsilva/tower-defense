#ifndef TOWERDEFENSE_VIEWER_RESOURCES_H
#define TOWERDEFENSE_VIEWER_RESOURCES_H

#include <iostream>
#include <string>
#include <SDL2/SDL_filesystem.h>
#include <SDL2/SDL_quit.h>

std::string getResourcePath(const std::string &subDir = ""){
    static std::string baseRes;
    size_t pos;

    if (baseRes.empty()){
        char *basePath = SDL_GetBasePath();
        if (basePath){
            baseRes = basePath;
            SDL_free(basePath);
        }
        else {
            std::cerr << "Error getting resource path: " << SDL_GetError() << std::endl;
            return "";
        }

        pos = baseRes.rfind("src");
    }

    return baseRes.substr(0, pos);
}

#endif