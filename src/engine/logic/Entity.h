#ifndef TOWERDEFENSE_ENTITY_H
#define TOWERDEFENSE_ENTITY_H

class EntityInterface {
public:
    virtual ~EntityInterface() {};
};

enum class EntityAction { ADD, REMOVE };
enum class EntityType { TOWER, USER_INTERACTION, MONSTER };

typedef struct EntityModification {
public:
    EntityModification(EntityInterface * ref, unsigned int identifier, EntityAction action, EntityType type) :
            entity_(ref), identifier_(identifier), action_(action), type_(type) {}
    EntityInterface* entity_;
    unsigned int identifier_;
    EntityAction action_;
    EntityType type_;
} EntityModification;

#endif //TOWERDEFENSE_ENTITY_H
