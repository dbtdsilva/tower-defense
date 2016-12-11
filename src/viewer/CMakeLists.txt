PKG_SEARCH_MODULE(SDL2 REQUIRED sdl2)
PKG_SEARCH_MODULE(SDL2IMAGE REQUIRED SDL2_image >= 2.0.0)
PKG_SEARCH_MODULE(SDL2TTF REQUIRED SDL2_ttf >= 2.0.0)

INCLUDE_DIRECTORIES(${SDL2_INCLUDE_DIR} ${SDL2IMAGE_INCLUDE_DIR} ${SDL2TTF_INCLUDE_DIR})

ADD_EXECUTABLE(towerdefense_viewer
        main_viewer.cpp
        MapDrawer.h MapDrawer.cpp
        aux/Cleanup.h
        map/Map.h map/Map.cpp
        objects/Bullet.h objects/Bullet.cpp
        objects/Monster.h objects/Monster.cpp
        objects/Tower.h objects/Tower.cpp
        objects/Tower.h objects/Tower.cpp
        MapDrawer.h MapDrawer.cpp)

TARGET_LINK_LIBRARIES(towerdefense_viewer ${SDL2_LIBRARIES} ${SDL2IMAGE_LIBRARIES} ${SDL2TTF_LIBRARIES})
ADD_DEPENDENCIES(towerdefense_viewer cereal)
SET_TARGET_PROPERTIES(towerdefense_viewer PROPERTIES COMPILE_FLAGS "-std=c++14")
