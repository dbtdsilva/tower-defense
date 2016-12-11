CMAKE_MINIMUM_REQUIRED(VERSION 2.8.11)

PROJECT(towerdefense)

INCLUDE(FindPkgConfig)

SET(CMAKE_C_COMPILER "gcc-5")
SET(CMAKE_CXX_COMPILER "g++-5")

SET(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}/build")
#SET(CMAKE_INCLUDE_CURRENT_DIR ON)
SET(CMAKE_AUTOMOC ON)
SET(CMAKE_CXX_STANDARD 14)
SET(CMAKE_CXX_STANDARD_REQUIRED ON)
SET(CMAKE_CXX_EXTENSIONS OFF)

PKG_SEARCH_MODULE(SDL2 REQUIRED sdl2)
PKG_SEARCH_MODULE(SDL2IMAGE REQUIRED SDL2_image >= 2.0.0)
PKG_SEARCH_MODULE(SDL2TTF REQUIRED SDL2_ttf >= 2.0.0)

INCLUDE_DIRECTORIES(${SDL2_INCLUDE_DIR} ${SDL2IMAGE_INCLUDE_DIR} ${SDL2TTF_INCLUDE_DIR})

ADD_EXECUTABLE(towerdefense
        main.cpp
        logic/WorldState.cpp logic/WorldState.h
        logic/helpers/Position.h
        logic/tower/TowerInterface.h logic/tower/TowerInterface.cpp
        logic/tower/Tower.cpp logic/tower/Tower.h
        logic/tower/types/SimpleTower.h
        logic/monster/Monster.cpp logic/monster/Monster.h
        logic/monster/types/BasicMonster.h
        logic/monster/MonsterInterface.cpp logic/monster/MonsterInterface.h
        logic/interaction/UserInteractionInterface.cpp logic/interaction/UserInteractionInterface.h
        logic/interaction/UserInteraction.cpp logic/interaction/UserInteraction.h logic/helpers/Definitions.h)

ADD_EXECUTABLE(towerdefense_viewer
        main_viewer.cpp
        viewer/MapDrawer.h viewer/MapDrawer.cpp
        viewer/aux/Cleanup.h
        viewer/aux/Resources.h
        viewer/map/Map.h viewer/map/Map.cpp
        viewer/objects/Bullet.h viewer/objects/Bullet.cpp
        viewer/objects/Monster.h viewer/objects/Monster.cpp
        viewer/objects/Tower.h viewer/objects/Tower.cpp)

TARGET_LINK_LIBRARIES(towerdefense_viewer ${SDL2_LIBRARIES} ${SDL2IMAGE_LIBRARIES} ${SDL2TTF_LIBRARIES})
SET_TARGET_PROPERTIES(towerdefense PROPERTIES COMPILE_FLAGS "-std=c++14")
SET_TARGET_PROPERTIES(towerdefense_viewer PROPERTIES COMPILE_FLAGS "-std=c++14")

ADD_DEFINITIONS(-DDEBUG)
