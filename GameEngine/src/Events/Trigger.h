
#pragma once

#include <entityx/entityx.h>

using namespace entityx;

struct Trigger : public Event<Trigger> {
    Trigger(Entity* gotTriggered, Entity* triggeringEntity) : gotTriggered(gotTriggered), triggeringEntity(triggeringEntity), triggered(true), triggerEntered(true), triggerExited(false){}
    Entity *gotTriggered, *triggeringEntity;
    bool triggered, triggerEntered, triggerExited;  //triggered means trigger is currently firing;  triggerEntered means trigger has already fired before;  triggerExited means objects are no longer colliding;

    bool triggerMatch(Entity* otherGotTriggered, Entity* otherTriggeringEntity){
        return (*otherGotTriggered == *gotTriggered && *otherTriggeringEntity == *triggeringEntity);
    }
};