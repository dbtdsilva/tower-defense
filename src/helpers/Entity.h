#ifndef TOWERDEFENSE_ENTITY_H
#define TOWERDEFENSE_ENTITY_H

class EntityInterface {
public:
    virtual ~EntityInterface() {};
};

enum class EntityAction { ADD, REMOVE };

typedef struct EntityModification {
public:
    EntityModification(EntityInterface * ref, unsigned int identifier, EntityAction action) :
            entity_(ref), identifier_(identifier), action_(action) {}
    EntityInterface* entity_;
    unsigned int identifier_;
    EntityAction action_;
} EntityModification;

#endif //TOWERDEFENSE_ENTITY_H
