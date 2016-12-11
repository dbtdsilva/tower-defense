PKG_SEARCH_MODULE(SDL2 REQUIRED sdl2)
PKG_SEARCH_MODULE(SDL2IMAGE REQUIRED SDL2_image >= 2.0.0)
PKG_SEARCH_MODULE(SDL2TTF REQUIRED SDL2_ttf >= 2.0.0)

INCLUDE_DIRECTORIES(${SDL2_INCLUDE_DIR} ${SDL2IMAGE_INCLUDE_DIR} ${SDL2TTF_INCLUDE_DIR})

ADD_EXECUTABLE(towerdefense
        main.cpp
        logic/WorldState.cpp logic/WorldState.h
        logic/tower/TowerInterface.h logic/tower/TowerInterface.cpp
        logic/tower/Tower.cpp logic/tower/Tower.h
        logic/tower/types/SimpleTower.h
        logic/tower/Bullet.h
        logic/monster/Monster.cpp logic/monster/Monster.h
        logic/monster/types/BasicMonster.h
        logic/monster/MonsterInterface.cpp logic/monster/MonsterInterface.h
        logic/monster/MonsterEye.h
        logic/interaction/UserInteractionInterface.cpp logic/interaction/UserInteractionInterface.h
        logic/interaction/UserInteraction.cpp logic/interaction/UserInteraction.h
        helpers/Definitions.h helpers/Entity.h helpers/Position.h
        serializer/WorldDataSerializer.h logic/tower/Bullet.cpp)

ADD_EXECUTABLE(towerdefense_viewer
        main_viewer.cpp
        viewer/MapDrawer.h viewer/MapDrawer.cpp
        viewer/aux/Cleanup.h
        viewer/map/Map.h viewer/map/Map.cpp
        viewer/objects/Bullet.h viewer/objects/Bullet.cpp
        viewer/objects/Monster.h viewer/objects/Monster.cpp
        viewer/objects/Tower.h viewer/objects/Tower.cpp
        viewer/objects/Tower.h viewer/objects/Tower.cpp
        viewer/MapDrawer.h viewer/MapDrawer.cpp)

ADD_DEFINITIONS(-Xlinker -rpath -Xlinker /usr/xenomai/lib)

ADD_DEPENDENCIES(towerdefense cereal)
SET_TARGET_PROPERTIES(towerdefense PROPERTIES COMPILE_FLAGS "-std=c++14")

TARGET_LINK_LIBRARIES(towerdefense_viewer ${SDL2_LIBRARIES} ${SDL2IMAGE_LIBRARIES} ${SDL2TTF_LIBRARIES})
ADD_DEPENDENCIES(towerdefense_viewer cereal)
SET_TARGET_PROPERTIES(towerdefense_viewer PROPERTIES COMPILE_FLAGS "-std=c++14")
