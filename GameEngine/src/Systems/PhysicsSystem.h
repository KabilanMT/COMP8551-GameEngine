#pragma once

#define _USE_MATH_DEFINES

#include "entityx/entityx.h"
#include <vector>
#include <algorithm>
#include "../Components/Components.h"
// #include "PhysicsFunctions.h"
#include "../logger.h"
#include <string>
#include <math.h>

using namespace entityx;

enum ColliderType {
    Box,
    Circle,
    Capsule
};

struct SASObject {
    SASObject(float val_, Entity& e_, bool isBegin_, ColliderType type_) : val(val_), e(e_), isBegin(isBegin_), type(type_) {}
    bool operator<(SASObject& right) {
        return val < right.val;
    }

    float val;
    Entity e;
    bool isBegin;
    ColliderType type;
};

struct EntityPair {
    EntityPair(Entity& a_, ColliderType aType_, Entity& b_, ColliderType bType_) : a(a_), b(b_), aType(aType_), bType(bType_) {}
    
    Entity a;
    ColliderType aType;
    Entity b;
    ColliderType bType;
};

class PhysicsSystem : public System<PhysicsSystem> {
    public:
        void update(EntityManager& es, EventManager& events, TimeDelta dt) override {
            //Step 1: Apply rigidbody movement (velocity)
            //TODO

            //Step 2: Detect collisions
            std::vector<EntityPair> pairs = broadphase(es); //returns pairs of possible collisions
            std::vector<EntityPair> collidingPairs = narrowphase(pairs); //should return pairs of entities that are colliding

            for (int i = 0; i < collidingPairs.size(); ++i) {
                Logger::getInstance() << collidingPairs.at(i).a.id().id() << " colliding with " << collidingPairs.at(i).b.id().id() << "\n";
            }
            //Logger::getInstance() << "physics!\n";
            //Step 3: apply physics to all entities and resolve all collisions from pairs
            for(int i = 0; i < collidingPairs.size(); ++i) {
                PerformCollisionCalculations(collidingPairs.at(i));
            }
            auto entities = es.entities_with_components<Rigidbody_2D>();

            for(Entity e : entities){
                //Update thrust by getting force value from input
                float thrust = 0.0f;

                //Update velocities and accelerations
                UpdateVelocityAndAcceleration(e, dt, thrust);
            }
        }
    private:
        std::vector<EntityPair> narrowphase(std::vector<EntityPair> possibleColl) {
            std::vector<EntityPair> collisions;
            for (EntityPair ep : possibleColl) {
                //get each entity's colliders and transforms
                bool isColliding = false;
                ComponentHandle<Transform> c1T = ep.a.component<Transform>();
                ComponentHandle<Transform> c2T = ep.b.component<Transform>();
                if (ep.aType == Box) {
                    ComponentHandle<BoxCollider> c1 = ep.a.component<BoxCollider>();
                    if (ep.bType == Box) {
                        ComponentHandle<BoxCollider> c2 = ep.b.component<BoxCollider>();
                        isColliding = CheckCollision(c1, c2, c1T, c2T);
                    } else if (ep.bType == Circle) {
                        ComponentHandle<CircleCollider> c2 = ep.b.component<CircleCollider>();
                        isColliding = CheckCollision(c1, c2, c1T, c2T);
                    } else if (ep.bType == Capsule) {
                        ComponentHandle<CapsuleCollider> c2 = ep.b.component<CapsuleCollider>();
                        isColliding = CheckCollision(c1, c2, c1T, c2T);
                    }
                } else if (ep.aType == Circle) {
                    ComponentHandle<CircleCollider> c1 = ep.a.component<CircleCollider>();
                    if (ep.bType == Box) {
                        ComponentHandle<BoxCollider> c2 = ep.b.component<BoxCollider>();
                        isColliding = CheckCollision(c1, c2, c1T, c2T);
                    } else if (ep.bType == Circle) {
                        ComponentHandle<CircleCollider> c2 = ep.b.component<CircleCollider>();
                        isColliding = CheckCollision(c1, c2, c1T, c2T);
                    } else if (ep.bType == Capsule) {
                        ComponentHandle<CapsuleCollider> c2 = ep.b.component<CapsuleCollider>();
                        isColliding = CheckCollision(c1, c2, c1T, c2T);
                    }
                } else if (ep.aType == Capsule) {
                    ComponentHandle<CapsuleCollider> c1 = ep.a.component<CapsuleCollider>();
                    if (ep.bType == Box) {
                        ComponentHandle<BoxCollider> c2 = ep.b.component<BoxCollider>();
                        isColliding = CheckCollision(c1, c2, c1T, c2T);
                    } else if (ep.bType == Circle) {
                        ComponentHandle<CircleCollider> c2 = ep.b.component<CircleCollider>();
                        isColliding = CheckCollision(c1, c2, c1T, c2T);
                    } else if (ep.bType == Capsule) {
                        ComponentHandle<CapsuleCollider> c2 = ep.b.component<CapsuleCollider>();
                        isColliding = CheckCollision(c1, c2, c1T, c2T);
                    }
                }
                // ComponentHandle<BoxCollider> c1 = ep.a.component<BoxCollider>();
                // ComponentHandle<BoxCollider> c2 = ep.b.component<BoxCollider>();

                // //check if the colliders are colliding based on their type/shape
                // bool isColliding = CheckCollision(c1, c2, c1T, c2T);
                if (isColliding) {
                    collisions.push_back(ep);
                }
            }

            return collisions;
        }
        #pragma region //Collision algorithms
        //Box - Box
        bool CheckCollision(ComponentHandle<BoxCollider>& c1, ComponentHandle<BoxCollider>& c2, ComponentHandle<Transform> c1T, ComponentHandle<Transform> c2T) {
            return DetectAABB(c1->x + c1T->x, c1->y + c1T->y, c1->bbWidth, c1->bbHeight, c2->x + c2T->x, c2->y + c2T->y, c2->bbWidth, c2->bbHeight);
        }

        //Circle - Circle
        bool CheckCollision(ComponentHandle<CircleCollider>& c1, ComponentHandle<CircleCollider>& c2, ComponentHandle<Transform> c1T, ComponentHandle<Transform> c2T) {
            //TODO
            Logger::getInstance() << "CheckCollision Circle-Circle\n"; // remove this during implementation
            return false;
        }

        //Capsule - Capsule
        bool CheckCollision(ComponentHandle<CapsuleCollider>& c1, ComponentHandle<CapsuleCollider>& c2, ComponentHandle<Transform> c1T, ComponentHandle<Transform> c2T) {
            //TODO
            Logger::getInstance() << "CheckCollision Capsule-Capsule\n"; // remove this during implementation
            return false;
        }

        //Box - Circle
        bool CheckCollision(ComponentHandle<BoxCollider>& c1, ComponentHandle<CircleCollider>& c2, ComponentHandle<Transform> c1T, ComponentHandle<Transform> c2T) {
            //TODO
            Logger::getInstance() << "CheckCollision Box-Circle\n"; // remove this during implementation
            return false;
        }

        //Circle - Box (Calls Box - Circle)
        bool CheckCollision(ComponentHandle<CircleCollider>& c1, ComponentHandle<BoxCollider>& c2, ComponentHandle<Transform> c1T, ComponentHandle<Transform> c2T) {
            return CheckCollision(c2, c1, c2T, c1T);
        }

        //Box - Capsule
        bool CheckCollision(ComponentHandle<BoxCollider>& c1, ComponentHandle<CapsuleCollider>& c2, ComponentHandle<Transform> c1T, ComponentHandle<Transform> c2T) {
            //TODO
            Logger::getInstance() << "CheckCollision Box-Capsule\n"; // remove this during implementation
            return false;
        }

        //Capsule - Box (Calls Box - Capsule)
        bool CheckCollision(ComponentHandle<CapsuleCollider>& c1, ComponentHandle<BoxCollider>& c2, ComponentHandle<Transform> c1T, ComponentHandle<Transform> c2T) {
            return CheckCollision(c2, c1, c2T, c1T);
        }

        //Circle - Capsule
        bool CheckCollision(ComponentHandle<CircleCollider>& c1, ComponentHandle<CapsuleCollider>& c2, ComponentHandle<Transform> c1T, ComponentHandle<Transform> c2T) {
            //TODO
            Logger::getInstance() << "CheckCollision Circle-Capsule\n"; // remove this during implementation
            return false;
        }

        //Capsule - Circle (Calls Circle - Capsule)
        bool CheckCollision(ComponentHandle<CapsuleCollider>& c1, ComponentHandle<CircleCollider>& c2, ComponentHandle<Transform> c1T, ComponentHandle<Transform> c2T) {
            return CheckCollision(c2, c1, c2T, c1T);
        }

        bool DetectAABB(float x1, float y1, float width1, float height1,
            float x2, float y2, float width2, float height2){
            if (x1 <= x2 + width2 && x1 + width1 >= x2 && y1 <= y2 + height2 &&
            y1 + height1 >= y2){
                return true;
            }
            return false;
        }

        void PerformCollisionCalculations(EntityPair collision) {
            ComponentHandle<Transform> aTrans = collision.a.component<Transform>();
            ComponentHandle<Transform> bTrans = collision.b.component<Transform>();
            ComponentHandle<Rigidbody_2D> aRB = collision.a.component<Rigidbody_2D>();
            ComponentHandle<Rigidbody_2D> bRB = collision.b.component<Rigidbody_2D>();

            float aMass = aRB->mass;
            float bMass = bRB->mass;

            float xDist = (bTrans->x + bRB->cmX) - (aTrans->x + aRB->cmX);
            float yDist = (bTrans->y + bRB->cmY) - (aTrans->y + aRB->cmY);

            float phi = atan2f(yDist, xDist);

            float vAI = sqrtf(powf(aRB->velocityX, 2.0f) + powf(aRB->velocityY, 2.0f));
            float iThetaA = atan2f(aRB->velocityY, aRB->velocityX);

            float vBI = sqrtf(powf(bRB->velocityX, 2.0f) + powf(bRB->velocityY, 2.0f));
            float iThetaB = atan2f(bRB->velocityY, bRB->velocityX);

            float vAFX = ((vAI * cosf(iThetaA - phi) * (aMass - bMass) + 2 * bMass * vBI * cosf(iThetaB - phi)) / (aMass + bMass)) * 
            cosf(phi) + vAI * sinf(iThetaA - phi) * cosf(phi + M_PI_2);

            float vAFY = ((vAI * cosf(iThetaA - phi) * (aMass - bMass) + 2 * bMass * vBI * cosf(iThetaB - phi)) / (aMass + bMass)) * 
            sinf(phi) + vAI * sinf(iThetaA - phi) * sinf(phi + M_PI_2);

            float vBFX = ((vBI * cosf(iThetaB - phi) * (bMass - aMass) + 2 * aMass * vAI * cosf(iThetaA - phi)) / (aMass + bMass)) * 
            cosf(phi) + vBI * sinf(iThetaB - phi) * cosf(phi + M_PI_2);

            float vBFY = ((vBI * cosf(iThetaB - phi) * (bMass - aMass) + 2 * aMass * vAI * cosf(iThetaA - phi)) / (aMass + bMass)) * 
            sinf(phi) + vBI * sinf(iThetaB - phi) * sinf(phi + M_PI_2);

            aRB->velocityX = vAFX;
            aRB->velocityY = vAFY;
            bRB->velocityX = vBFX;
            bRB->velocityY = vBFY;

            return;
        }

        void UpdateVelocityAndAcceleration(Entity e, TimeDelta dt, float thrust = 0.0f){
            ComponentHandle<Rigidbody_2D> rb = e.component<Rigidbody_2D>();
            float tau = rb->mass / rb->linDrag;
            float t = (float)dt;
            //update velocity
            rb->velocityX = (1.0f/rb->linDrag)*(thrust - expf(-1 * rb->linDrag * t / rb->mass) * (thrust - rb->linDrag * rb->velocityX));
            rb->velocityY = (1.0f/rb->linDrag)*(thrust - expf(-1 * rb->linDrag * t / rb->mass) * (thrust - rb->linDrag * rb->velocityY));
            //update acceleration
            rb->accelerationX = (thrust - rb->linDrag * rb->velocityX) / rb->mass;
            rb->accelerationY = (thrust - rb->linDrag * rb->velocityY) / rb->mass;

            return;
        }

        #pragma endregion //collision algorithms

        std::vector<EntityPair> broadphase(EntityManager& es) {
            //might need performance boost - right now it's sorting every frame,
            //could keep it sorted between frames but would need a way to track new/moving colliders

            //sort and sweep
            //get all colliders
            auto boxEntities = es.entities_with_components<BoxCollider>();
            auto circleEntities = es.entities_with_components<CircleCollider>();
            auto capsuleEntities = es.entities_with_components<CapsuleCollider>();

            // colliders require b and e attributes (along x) - type float
            //get each collider's b & e
            //and put all b & e in a list
            std::vector<SASObject> sas;
            for (Entity e : boxEntities) {
                ComponentHandle<BoxCollider> handle = e.component<BoxCollider>();
                ComponentHandle<Transform> handleT = e.component<Transform>();
                sas.emplace_back(SASObject(handle->b + handleT->x, e, true, Box));
                sas.emplace_back(SASObject(handle->e + handleT->x, e, false, Box));
            }
            for (Entity e : circleEntities) {
                ComponentHandle<CircleCollider> handle = e.component<CircleCollider>();
                ComponentHandle<Transform> handleT = e.component<Transform>();
                sas.emplace_back(SASObject(handle->b + handleT->x, e, true, Circle));
                sas.emplace_back(SASObject(handle->e + handleT->x, e, false, Circle));
            }
            for (Entity e : capsuleEntities) {
                ComponentHandle<CapsuleCollider> handle = e.component<CapsuleCollider>();
                ComponentHandle<Transform> handleT = e.component<Transform>();
                sas.emplace_back(SASObject(handle->b + handleT->x, e, true, Capsule));
                sas.emplace_back(SASObject(handle->e + handleT->x, e, false, Capsule));
            }
            //pretty sure that each entity can only have one component of each type, i.e. no duplicates.
            //If this is the case it doesnt make sense for an entity to be able to have two different types of components either
            //solution: Generic Collider component that is either modified, or has children types
            //for now just assume only has box collider

            //sort the list
            std::sort(sas.begin(), sas.end());
            //sweep
            std::vector<SASObject> active;
            std::vector<EntityPair> possibleCollides;
            for (auto it = sas.begin(); it != sas.end(); ++it) {
                if ((*it).isBegin) {
                    //add to active
                    active.push_back(*it);
                } else {
                    //remove match from active
                    //when removing an SASObject, perform AABB check between this obj and all active ones
                    std::vector<SASObject>::iterator oToRemove; // to remember where to remove
                    for (auto it2 = active.begin(); it2 != active.end(); ++it2) {
                        if ((*it).e == (*it2).e) {
                            //they have the same entity, begin and end are *it and *it2
                            //remove begin
                            oToRemove = it2; //might be a reference issue here
                        } else {
                            Entity e1 = (*it).e;
                            Entity e2 = (*it2).e;
                            ComponentHandle<Transform> c1T = e1.component<Transform>();
                            ComponentHandle<Transform> c2T = e2.component<Transform>();
                            //Get each components' types and test AABB
                            if ((*it).type == Box) {
                                ComponentHandle<BoxCollider> c1 = e1.component<BoxCollider>();
                                if ((*it2).type == Box) {
                                    ComponentHandle<BoxCollider> c2 = e2.component<BoxCollider>();
                                    if (DetectAABB(c1->x + c1T->x, c1->y + c1T->y, c1->bbWidth, c1->bbHeight, c2->x + c2T->x, c2->y + c2T->y, c2->bbWidth, c2->bbHeight)) {
                                        possibleCollides.emplace_back(EntityPair((*it).e, Box, (*it2).e, Box));
                                    }
                                } else if ((*it2).type == Circle) {
                                    ComponentHandle<CircleCollider> c2 = e2.component<CircleCollider>();
                                    if (DetectAABB(c1->x + c1T->x, c1->y + c1T->y, c1->bbWidth, c1->bbHeight, c2->x + c2T->x, c2->y + c2T->y, c2->bbWidth, c2->bbHeight)) {
                                        possibleCollides.emplace_back(EntityPair((*it).e, Box, (*it2).e, Circle));
                                    }
                                } else if ((*it2).type == Capsule) {
                                    ComponentHandle<CapsuleCollider> c2 = e2.component<CapsuleCollider>();
                                    if (DetectAABB(c1->x + c1T->x, c1->y + c1T->y, c1->bbWidth, c1->bbHeight, c2->x + c2T->x, c2->y + c2T->y, c2->bbWidth, c2->bbHeight)) {
                                        possibleCollides.emplace_back(EntityPair((*it).e, Box, (*it2).e, Capsule));
                                    }
                                }
                            } else if ((*it).type == Circle) {
                                ComponentHandle<CircleCollider> c1 = e1.component<CircleCollider>();
                                if ((*it2).type == Box) {
                                    ComponentHandle<BoxCollider> c2 = e2.component<BoxCollider>();
                                    if (DetectAABB(c1->x + c1T->x, c1->y + c1T->y, c1->bbWidth, c1->bbHeight, c2->x + c2T->x, c2->y + c2T->y, c2->bbWidth, c2->bbHeight)) {
                                        possibleCollides.emplace_back(EntityPair((*it).e, Circle, (*it2).e, Box));
                                    }
                                } else if ((*it2).type == Circle) {
                                    ComponentHandle<CircleCollider> c2 = e2.component<CircleCollider>();
                                    if (DetectAABB(c1->x + c1T->x, c1->y + c1T->y, c1->bbWidth, c1->bbHeight, c2->x + c2T->x, c2->y + c2T->y, c2->bbWidth, c2->bbHeight)) {
                                        possibleCollides.emplace_back(EntityPair((*it).e, Circle, (*it2).e, Circle));
                                    }
                                } else if ((*it2).type == Capsule) {
                                    ComponentHandle<CapsuleCollider> c2 = e2.component<CapsuleCollider>();
                                    if (DetectAABB(c1->x + c1T->x, c1->y + c1T->y, c1->bbWidth, c1->bbHeight, c2->x + c2T->x, c2->y + c2T->y, c2->bbWidth, c2->bbHeight)) {
                                        possibleCollides.emplace_back(EntityPair((*it).e, Circle, (*it2).e, Capsule));
                                    }
                                }
                            } else if ((*it).type == Capsule) {
                                ComponentHandle<CapsuleCollider> c1 = e1.component<CapsuleCollider>();
                                if ((*it2).type == Box) {
                                    ComponentHandle<BoxCollider> c2 = e2.component<BoxCollider>();
                                    if (DetectAABB(c1->x + c1T->x, c1->y + c1T->y, c1->bbWidth, c1->bbHeight, c2->x + c2T->x, c2->y + c2T->y, c2->bbWidth, c2->bbHeight)) {
                                        possibleCollides.emplace_back(EntityPair((*it).e, Capsule, (*it2).e, Box));
                                    }
                                } else if ((*it2).type == Circle) {
                                    ComponentHandle<CircleCollider> c2 = e2.component<CircleCollider>();
                                    if (DetectAABB(c1->x + c1T->x, c1->y + c1T->y, c1->bbWidth, c1->bbHeight, c2->x + c2T->x, c2->y + c2T->y, c2->bbWidth, c2->bbHeight)) {
                                        possibleCollides.emplace_back(EntityPair((*it).e, Capsule, (*it2).e, Circle));
                                    }
                                } else if ((*it2).type == Capsule) {
                                    ComponentHandle<CapsuleCollider> c2 = e2.component<CapsuleCollider>();
                                    if (DetectAABB(c1->x + c1T->x, c1->y + c1T->y, c1->bbWidth, c1->bbHeight, c2->x + c2T->x, c2->y + c2T->y, c2->bbWidth, c2->bbHeight)) {
                                        possibleCollides.emplace_back(EntityPair((*it).e, Capsule, (*it2).e, Capsule));
                                    }
                                }
                            }
                        }
                    }
                    active.erase(oToRemove);
                }
            }
            return possibleCollides;
        }
};