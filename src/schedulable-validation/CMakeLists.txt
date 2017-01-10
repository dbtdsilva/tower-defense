IF(Xenomai_FOUND)
    ADD_DEFINITIONS(${Xenomai_DEFINITIONS})
    INCLUDE_DIRECTORIES(${Xenomai_INCLUDE_DIR})

    ADD_EXECUTABLE(wcet_monster
            wcet_monster.cpp
            ../engine/logic/WorldState.cpp ../engine/logic/WorldState.h
            ../engine/logic/tower/TowerInterface.h ../engine/logic/tower/TowerInterface.cpp
            ../engine/logic/tower/Tower.cpp ../engine/logic/tower/Tower.h
            ../engine/logic/tower/types/SimpleTower.h
            ../engine/logic/tower/types/ComplexTower.h
            ../engine/logic/tower/Bullet.h ../engine/logic/tower/Bullet.cpp
            ../engine/logic/monster/Monster.cpp ../engine/logic/monster/Monster.h
            ../engine/logic/monster/types/BasicMonster.h ../engine/logic/monster/types/InsaneMonster.h
            ../engine/logic/monster/MonsterInterface.cpp ../engine/logic/monster/MonsterInterface.h
            ../engine/logic/monster/MonsterEye.h
            ../engine/logic/interaction/UserInteractionInterface.cpp ../engine/logic/interaction/UserInteractionInterface.h
            ../engine/logic/interaction/UserInteraction.cpp ../engine/logic/interaction/UserInteraction.h
            ../engine/logic/Entity.h)

    ADD_EXECUTABLE(wcet_tower
            wcet_tower.cpp
            ../engine/logic/WorldState.cpp ../engine/logic/WorldState.h
            ../engine/logic/tower/TowerInterface.h ../engine/logic/tower/TowerInterface.cpp
            ../engine/logic/tower/Tower.cpp ../engine/logic/tower/Tower.h
            ../engine/logic/tower/types/SimpleTower.h
            ../engine/logic/tower/types/ComplexTower.h
            ../engine/logic/tower/Bullet.h ../engine/logic/tower/Bullet.cpp
            ../engine/logic/monster/Monster.cpp ../engine/logic/monster/Monster.h
            ../engine/logic/monster/types/BasicMonster.h ../engine/logic/monster/types/InsaneMonster.h
            ../engine/logic/monster/MonsterInterface.cpp ../engine/logic/monster/MonsterInterface.h
            ../engine/logic/monster/MonsterEye.h
            ../engine/logic/interaction/UserInteractionInterface.cpp ../engine/logic/interaction/UserInteractionInterface.h
            ../engine/logic/interaction/UserInteraction.cpp ../engine/logic/interaction/UserInteraction.h
            ../engine/logic/Entity.h)

    ADD_DEPENDENCIES(wcet_monster cereal)
    TARGET_LINK_LIBRARIES(wcet_monster ${Xenomai_LIBRARIES_NATIVE})
    SET_TARGET_PROPERTIES(wcet_monster PROPERTIES COMPILE_FLAGS "-std=c++14 -g -O3")

    ADD_DEPENDENCIES(wcet_tower cereal)
    TARGET_LINK_LIBRARIES(wcet_tower ${Xenomai_LIBRARIES_NATIVE})
    SET_TARGET_PROPERTIES(wcet_tower PROPERTIES COMPILE_FLAGS "-std=c++14 -g -O3")


ENDIF()
