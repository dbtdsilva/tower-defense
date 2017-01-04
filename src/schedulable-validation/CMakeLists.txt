IF(Xenomai_FOUND)
    ADD_DEFINITIONS(${Xenomai_DEFINITIONS})
    INCLUDE_DIRECTORIES(${Xenomai_INCLUDE_DIR})

    ADD_EXECUTABLE(schedulability_tests
            main.cpp
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

    ADD_DEPENDENCIES(schedulability_tests cereal)
    TARGET_LINK_LIBRARIES(schedulability_tests ${Xenomai_LIBRARIES_NATIVE})
    SET_TARGET_PROPERTIES(schedulability_tests PROPERTIES COMPILE_FLAGS "-std=c++14 -g -O3")
ENDIF()
