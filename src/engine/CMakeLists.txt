set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_CURRENT_SOURCE_DIR}/cmake/Modules/")

SET(Xenomai_INCLUDE_DIR "xenomai/install_output/include")
FIND_PACKAGE(Xenomai REQUIRED)

ADD_DEFINITIONS(${Xenomai_DEFINITIONS})
INCLUDE_DIRECTORIES(${Xenomai_INCLUDE_DIR})

ADD_EXECUTABLE(towerdefense
        main.cpp
        logic/WorldState.cpp logic/WorldState.h
        logic/tower/TowerInterface.h logic/tower/TowerInterface.cpp
        logic/tower/Tower.cpp logic/tower/Tower.h
        logic/tower/types/SimpleTower.h
        logic/tower/Bullet.h logic/tower/Bullet.cpp
        logic/monster/Monster.cpp logic/monster/Monster.h
        logic/monster/types/BasicMonster.h
        logic/monster/MonsterInterface.cpp logic/monster/MonsterInterface.h
        logic/monster/MonsterEye.h
        logic/interaction/UserInteractionInterface.cpp logic/interaction/UserInteractionInterface.h
        logic/interaction/UserInteraction.cpp logic/interaction/UserInteraction.h
        logic/Entity.h)

ADD_DEPENDENCIES(towerdefense cereal)
SET_TARGET_PROPERTIES(towerdefense PROPERTIES COMPILE_FLAGS "-std=c++14")

ADD_EXECUTABLE(abc periodic_task_4.c)
TARGET_LINK_LIBRARIES(abc ${Xenomai_LIBRARIES_NATIVE} ${Xenomai_LDFLAGS_NATIVE})
#ADD_DEPENDENCIES(abc xenomai)