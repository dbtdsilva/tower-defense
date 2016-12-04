CMAKE_MINIMUM_REQUIRED(VERSION 2.8.11)

PROJECT(towerdefense)

SET(CMAKE_C_COMPILER "gcc-5")
SET(CMAKE_CXX_COMPILER "g++-5")

SET(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}/build")
#SET(CMAKE_INCLUDE_CURRENT_DIR ON)
SET(CMAKE_AUTOMOC ON)
SET(CMAKE_CXX_STANDARD 14)
SET(CMAKE_CXX_STANDARD_REQUIRED ON)
SET(CMAKE_CXX_EXTENSIONS OFF)

FIND_PACKAGE(SDL2 REQUIRED)
INCLUDE_DIRECTORIES(${SDL2_INCLUDE_DIRS})

ADD_EXECUTABLE(towerdefense
        main.cpp
        logic/WorldState.cpp logic/WorldState.h
        logic/tower/Tower.cpp logic/tower/Tower.h
        logic/tower/types/SimpleTower.cpp logic/tower/types/SimpleTower.h
        logic/monster/Monster.cpp logic/monster/Monster.h
        logic/helpers/Position.h
        logic/tower/TowerAgent.cpp logic/tower/TowerAgent.h
        logic/WorldUserInteractions.cpp logic/WorldUserInteractions.h
        logic/tower/Bullet.cpp logic/tower/Bullet.h
        logic/monster/MonsterAgent.cpp logic/monster/MonsterAgent.h
        logic/monster/types/BasicMonster.cpp logic/monster/types/BasicMonster.h)
TARGET_LINK_LIBRARIES(towerdefense ${SDL2_LIBRARIES})
SET_TARGET_PROPERTIES(towerdefense PROPERTIES COMPILE_FLAGS "-std=c++14")

ADD_DEFINITIONS(-DDEBUG)
