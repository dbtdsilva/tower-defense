IF(Xenomai_FOUND)
  ADD_DEFINITIONS(${Xenomai_DEFINITIONS})
  INCLUDE_DIRECTORIES(${Xenomai_INCLUDE_DIR})

  ADD_EXECUTABLE(towerdefense
          main.cpp
          logic/WorldState.cpp logic/WorldState.h
          logic/tower/TowerInterface.h logic/tower/TowerInterface.cpp
          logic/tower/Tower.cpp logic/tower/Tower.h
          logic/tower/types/SimpleTower.h
          logic/tower/types/ComplexTower.h
          logic/tower/Bullet.h logic/tower/Bullet.cpp
          logic/monster/Monster.cpp logic/monster/Monster.h
          logic/monster/types/BasicMonster.h logic/monster/types/InsaneMonster.h
          logic/monster/MonsterInterface.cpp logic/monster/MonsterInterface.h
          logic/monster/MonsterEye.h
          logic/interaction/UserInteractionInterface.cpp logic/interaction/UserInteractionInterface.h
          logic/interaction/UserInteraction.cpp logic/interaction/UserInteraction.h
          logic/Entity.h)

  ADD_DEPENDENCIES(towerdefense cereal)
  TARGET_LINK_LIBRARIES(towerdefense ${Xenomai_LIBRARIES_NATIVE})
  SET_TARGET_PROPERTIES(towerdefense PROPERTIES COMPILE_FLAGS "-std=c++14 -g")
ENDIF()
