#pragma once

#define _USE_MATH_DEFINES

#include "entityx/entityx.h"
#include <vector>
#include <algorithm>
#include "../Components/Components.h"
#include "PhysicsFunctions.h"
#include "../logger.h"
#include <string>
#include <math.h>
#include <glm/glm.hpp>
#include <glm/gtx/rotate_vector.hpp>
#include <future>
#include <thread>

using namespace entityx;
using namespace Physics;

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

bool narrowphaseResults(int id, EntityPair& possibleColl){
    ComponentHandle<Transform> c1T = possibleColl.a.component<Transform>();
    ComponentHandle<Transform> c2T = possibleColl.b.component<Transform>();

    if (possibleColl.aType == Box) {
        ComponentHandle<BoxCollider> c1 = possibleColl.a.component<BoxCollider>();
        if (possibleColl.bType == Box) {
            ComponentHandle<BoxCollider> c2 = possibleColl.b.component<BoxCollider>();
            return CheckCollision(c1, c2, c1T, c2T);
        } else if (possibleColl.bType == Circle) {
            ComponentHandle<CircleCollider> c2 = possibleColl.b.component<CircleCollider>();
            return CheckCollision(c1, c2, c1T, c2T);
        } else if (possibleColl.bType == Capsule) {
            ComponentHandle<CapsuleCollider> c2 = possibleColl.b.component<CapsuleCollider>();
            return CheckCollision(c1, c2, c1T, c2T);
        }
    } else if (possibleColl.aType == Circle) {
        ComponentHandle<CircleCollider> c1 = possibleColl.a.component<CircleCollider>();
        if (possibleColl.bType == Box) {
            ComponentHandle<BoxCollider> c2 = possibleColl.b.component<BoxCollider>();
            return CheckCollision(c1, c2, c1T, c2T);
        } else if (possibleColl.bType == Circle) {
            ComponentHandle<CircleCollider> c2 = possibleColl.b.component<CircleCollider>();
            return CheckCollision(c1, c2, c1T, c2T);
        } else if (possibleColl.bType == Capsule) {
            ComponentHandle<CapsuleCollider> c2 = possibleColl.b.component<CapsuleCollider>();
            return CheckCollision(c1, c2, c1T, c2T);
        }
    } else if (possibleColl.aType == Capsule) {
        ComponentHandle<CapsuleCollider> c1 = possibleColl.a.component<CapsuleCollider>();
        if (possibleColl.bType == Box) {
            ComponentHandle<BoxCollider> c2 = possibleColl.b.component<BoxCollider>();
            return CheckCollision(c1, c2, c1T, c2T);
        } else if (possibleColl.bType == Circle) {
            ComponentHandle<CircleCollider> c2 = possibleColl.b.component<CircleCollider>();
            return CheckCollision(c1, c2, c1T, c2T);
        } else if (possibleColl.bType == Capsule) {
            ComponentHandle<CapsuleCollider> c2 = possibleColl.b.component<CapsuleCollider>();
            return CheckCollision(c1, c2, c1T, c2T);
        }
    }
    return false;
}


class PhysicsSystem : public System<PhysicsSystem>, public Receiver<PhysicsSystem> {
    public:
        void configure(EventManager& events) override {
            events.subscribe<MoveTo>(*this);
        }

        void receive(const MoveTo& mt) {
            RigidbodyMoveTo(Engine::getInstance().entities, mt.e, mt.x, mt.y);
            /*
            for (Entity e : Engine::getInstance().entities.entities_with_components<Name>()) {
                ComponentHandle<Name> handle = e.component<Name>();
                if (handle.get()->getName() == "Enemy1") {
                    ComponentHandle<Transform> pT = mt.e.component<Transform>();
                    ComponentHandle<Transform> eT = e.component<Transform>();
                    Logger::getInstance() << "Player: " << pT.get()->x << ", " << pT.get()->y << "\n";
                    Logger::getInstance() << "Enemy: " << eT.get()->x << ", " << eT.get()->y << "\n";
                    break;
                }
            }
            */
        }

        void update(EntityManager& es, EventManager& events, TimeDelta dt) override {
            //Step 1: Apply rigidbody movement (velocity)
            auto physicsEntities = es.entities_with_components<Rigidbody_2D>();

            for(Entity e : physicsEntities){
                //Update position:
                ComponentHandle<Rigidbody_2D> rb = e.component<Rigidbody_2D>();
                ComponentHandle<Transform> transform = e.component<Transform>();
                float t = (float)dt;
                transform->x = rb->velocityX * t + 0.5 * rb->accelerationX * powf(t, 2);
                transform->y = rb->velocityY * t + 0.5 * rb->accelerationY * powf(t, 2);
            }

            //Step 2: Detect collisions
            std::vector<EntityPair> pairs = broadphase(es); //returns pairs of possible collisions

            //Logger::getInstance() << "broadphase(update) collisions: " << pairs.size() << "\n";
            std::vector<EntityPair> collidingPairs = narrowphase(pairs); //should return pairs of entities that are colliding

            //Logger::getInstance() << "physics!\n";
            //Step 3: apply physics to all entities and resolve all collisions from pairs
            for(int i = 0; i < collidingPairs.size(); ++i) {
                if (!collidingPairs.at(i).a.valid() || !collidingPairs.at(i).b.valid()) {
                    continue;
                }

                if(collidingPairs.at(i).a.has_component<Rigidbody_2D>() && collidingPairs.at(i).b.has_component<Rigidbody_2D>())
                    PerformCollisionCalculations(collidingPairs.at(i));
                else if(collidingPairs.at(i).a.has_component<Rigidbody_2D>())
                    PerformPhysicsWithOneRigidBody(collidingPairs.at(i).a, collidingPairs.at(i).b, events);
                else if(collidingPairs.at(i).b.has_component<Rigidbody_2D>())
                    PerformPhysicsWithOneRigidBody(collidingPairs.at(i).b, collidingPairs.at(i).a, events);
                Trigger aTrigger = Trigger(&collidingPairs.at(i).a, &collidingPairs.at(i).b);
                Trigger bTrigger = Trigger(&collidingPairs.at(i).b, &collidingPairs.at(i).a);
                events.emit(aTrigger);
                events.emit(bTrigger);

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
            std::vector<std::future<bool>> results(possibleColl.size());
            std::vector<std::thread> narrowJobs;
            
            //thread pool
            for(int i = 0; i < possibleColl.size(); i++){
                results.at(i) = Engine::getInstance().jobPool.push(narrowphaseResults, possibleColl.at(i));
            }

            while(Engine::getInstance().jobPool.n_idle() != Engine::getInstance().jobPool.size()){};

            for(int i = 0; i < results.size(); i++){
                if(results.at(i).get() == true)
                    collisions.push_back(possibleColl.at(i));
            }
            return collisions;
        }

        std::vector<EntityPair> broadphase(EntityManager& es) {
            //might need performance boost - right now it's sorting every frame,
            //could keep it sorted between frames but would need a way to track new/moving colliders

            //sort and sweep
            //get all colliders
            auto boxEntities = es.entities_with_components<BoxCollider>();
            auto circleEntities = es.entities_with_components<CircleCollider>();
            auto capsuleEntities = es.entities_with_components<CapsuleCollider>();

            /*
            ComponentHandle<CircleCollider> circleHandle;
            ComponentHandle<BoxCollider> boxHandle;
            ComponentHandle<CapsuleCollider> capHandle;
            std::vector<ComponentHandle<CircleCollider>> circColliders;
            std::vector<ComponentHandle<BoxCollider>> boxColliders;
            std::vector<ComponentHandle<CapsuleCollider>> capColliders;
            for(Entity e : circleEntities){
                circleHandle = e.component<CircleCollider>();
                circColliders.push_back(circleHandle);
            }
            for(Entity e : boxEntities){
                boxHandle = e.component<BoxCollider>();
                boxColliders.push_back(boxHandle);
            }
            for(Entity e : capsuleEntities){
                capHandle = e.component<CapsuleCollider>();
                capColliders.push_back(capHandle);
            }
            Logger::getInstance() << "Num circle colliders: " << circColliders.size() << "\n";
            Logger::getInstance() << "Num box colliders: " << boxColliders.size() << "\n";
            Logger::getInstance() << "Num cap colliders: " << capColliders.size() << "\n";
            */

            // colliders require b and e attributes (along x) - type float
            //get each collider's b & e
            //and put all b & e in a list
            std::vector<SASObject> sas;
            for (Entity e : boxEntities) {
                if (e.has_component<Active>() && !e.component<Active>()->getIfActive())
                    continue;
                ComponentHandle<BoxCollider> handle = e.component<BoxCollider>();
                ComponentHandle<Transform> handleT = e.component<Transform>();
                sas.emplace_back(SASObject(handle->b + handleT->x, e, true, Box));
                sas.emplace_back(SASObject(handle->e + handleT->x, e, false, Box));
            }
            for (Entity e : circleEntities) {
                if (e.has_component<Active>() && !e.component<Active>()->getIfActive())
                    continue;
                ComponentHandle<CircleCollider> handle = e.component<CircleCollider>();
                ComponentHandle<Transform> handleT = e.component<Transform>();
                sas.emplace_back(SASObject(handle->b + handleT->x, e, true, Circle));
                sas.emplace_back(SASObject(handle->e + handleT->x, e, false, Circle));
            }
            for (Entity e : capsuleEntities) {
                if (e.has_component<Active>() && !e.component<Active>()->getIfActive())
                    continue;
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
                                    if (Physics::DetectAABB(c1->x + c1T->x, c1->y + c1T->y, c1->bbWidth, c1->bbHeight, c2->x + c2T->x, c2->y + c2T->y, c2->bbWidth, c2->bbHeight)) {
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
                                    //Logger::getInstance() << "Circle AABB inputs: " << c1->x + c1T->x << ", " << c1->y + c1T->y << ", " << c1->bbWidth << ", " << c1->bbHeight << ", " << c2->x + c2T->x << ", " << c2->y + c2T->y << ", " << c2->bbWidth << ", " << c2->bbHeight << "\n";
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
            //Logger::getInstance() << "Size of possible collides: " << possibleCollides.size() << "\n";
            return possibleCollides;
        }

        #pragma region //Collision algorithms

        void PerformCollisionCalculations(EntityPair& collision) {

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
        }

        //Updates the move on the rigidbody with detection of collisions along the way
        void RigidbodyMoveTo(EntityManager& es, Entity& e, float x, float y){
            int checkInterval = 10;
            ComponentHandle<Transform>transform = e.component<Transform>();
            float xStep = (x - transform->x) / checkInterval;
            float yStep = (y - transform->y) / checkInterval;

            for(int i = 0; i <= checkInterval; i++){
                //Step 1:  Perform step
                transform->x += xStep;
                transform->y += yStep;
                //Step 2: Detect collisions
                std::vector<EntityPair> pairs = broadphase(es); //returns pairs of possible collisions
                std::vector<EntityPair> collideWithMe;  //Container for all the collisions involving the entity
                std::vector<EntityPair> narrowPhaseCollisions;
                for(EntityPair p : pairs){
                    bool isATrigger = false;
                    bool isBTrigger = false;
                    if (p.aType == Circle) {
                        ComponentHandle<CircleCollider> aHandle = p.a.component<CircleCollider>();
                        isATrigger = aHandle->isTrigger;
                    } else if (p.aType == Box) {
                        ComponentHandle<BoxCollider> aHandle = p.a.component<BoxCollider>();
                        isATrigger = aHandle->isTrigger;
                    } else if (p.aType == Capsule) {
                        ComponentHandle<CapsuleCollider> aHandle = p.a.component<CapsuleCollider>();
                        isATrigger = aHandle->isTrigger;
                    }
                    if (p.bType == Circle) {
                        ComponentHandle<CircleCollider> bHandle = p.b.component<CircleCollider>();
                        isBTrigger = bHandle->isTrigger;
                    } else if (p.bType == Box) {
                        ComponentHandle<BoxCollider> bHandle = p.b.component<BoxCollider>();
                        isBTrigger = bHandle->isTrigger;
                    } else if (p.bType == Capsule) {
                        ComponentHandle<CapsuleCollider> bHandle = p.b.component<CapsuleCollider>();
                        isBTrigger = bHandle->isTrigger;
                    }
                    if ((p.a == e || p.b == e) && !isATrigger && !isBTrigger)
                        collideWithMe.push_back(p);
                }

                //Logger::getInstance() << "broadphase collisions: " << collideWithMe.size() << "\n";

                if(collideWithMe.size() > 0)
                    narrowPhaseCollisions = narrowphase(collideWithMe);

                //Logger::getInstance() << "Narrowphase collisions: " << narrowPhaseCollisions.size() << "\n";

                if(narrowPhaseCollisions.size() > 0){
                    //Reverse step:
                    for (int i = 0; i < narrowPhaseCollisions.size(); ++i) {
                        Trigger aTrigger = Trigger(&narrowPhaseCollisions.at(i).a, &narrowPhaseCollisions.at(i).b);
                        Trigger bTrigger = Trigger(&narrowPhaseCollisions.at(i).b, &narrowPhaseCollisions.at(i).a);
                        Engine::getInstance().events.emit(aTrigger);
                        Engine::getInstance().events.emit(bTrigger);
                    }
                    transform->x -= xStep;
                    transform->y -= yStep;
                    return;     //Leave function after backstep instead of continuing
                }
            }
        }

        void UpdateVelocityAndAcceleration(Entity& e, TimeDelta dt, float thrust = 0.0f){
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

        void PerformPhysicsWithOneRigidBody(Entity& hasRigidbody, Entity& noRigidbody, EventManager& events){
            
            ComponentHandle<Transform> hRBTrans = hasRigidbody.component<Transform>();
            ComponentHandle<Transform> nRBTrans = noRigidbody.component<Transform>();
            ComponentHandle<Rigidbody_2D> hRB = hasRigidbody.component<Rigidbody_2D>();
            ComponentHandle<CircleCollider> hRBCC;
            ComponentHandle<BoxCollider> hRBBC;
            ComponentHandle<CapsuleCollider> hRBCapC;
            glm::vec2 rbVelocity = glm::vec2(hRB->velocityX, hRB->velocityY);

            //Figure out what kind of collider the rigidbody has:
            if(hasRigidbody.has_component<CircleCollider>()){
                hRBCC = hasRigidbody.component<CircleCollider>();
                if(hRBCC->isTrigger){
                    return;
                }
            }
            if(hasRigidbody.has_component<BoxCollider>()){
                hRBBC = hasRigidbody.component<BoxCollider>();
                if(hRBBC->isTrigger){
                    return;
                }
            }
            if(hasRigidbody.has_component<CapsuleCollider>()){
                hRBCapC = hasRigidbody.component<CapsuleCollider>();
                if(hRBCapC->isTrigger){
                    return;
                }
            }
            
            if(noRigidbody.has_component<CircleCollider>()){
                ComponentHandle<CircleCollider> nRBCollider = noRigidbody.component<CircleCollider>();
                //Check for trigger:
                if(nRBCollider->isTrigger){
                    return;
                }

                //Find the vector of the angle between the center of the two bodies
                glm::vec2 directionFromNoRBToRB = glm::vec2(hRBTrans->x + hRB->cmX - nRBTrans->x + nRBCollider->x, 
                    hRBTrans->y + hRB->cmY - nRBTrans->y + nRBCollider->y);

                //Calculate the reflection of the velocity vector:
                glm::vec2 newVelocity = rbVelocity - (2 * glm::dot(rbVelocity, directionFromNoRBToRB)) / 
                    glm::dot(directionFromNoRBToRB, directionFromNoRBToRB) * directionFromNoRBToRB;

                hRB->velocityX = newVelocity.x;
                hRB->velocityY = newVelocity.y;

            } else {
                //Collider bounding box points setup:
                glm::vec2 topLeft;
                glm::vec2 topRight;
                glm::vec2 bottomLeft;
                glm::vec2 bottomRight;
                if(noRigidbody.has_component<BoxCollider>()){
                    ComponentHandle<BoxCollider> nRBCollider = noRigidbody.component<BoxCollider>();
                    //Check for trigger:
                    if(nRBCollider->isTrigger){
                        return;
                    }
                    float theta = nRBTrans->angle * M_PI / 180.0f;
                    topLeft = glm::rotate(glm::vec2(0 - nRBCollider->bbWidth / 2, nRBCollider->bbHeight / 2), theta);
                    topLeft = glm::vec2(topLeft.x + nRBTrans->x + nRBCollider->x, topLeft.y + nRBTrans->y + nRBCollider->y);
                    topRight = glm::rotate(glm::vec2(nRBCollider->bbWidth / 2, nRBCollider->bbHeight / 2), theta);
                    topRight = glm::vec2(topRight.x + nRBTrans->x + nRBCollider->x, topRight.y + nRBTrans->y + nRBCollider->y);
                    bottomLeft = glm::rotate(glm::vec2(0 - nRBCollider->bbWidth / 2, 0 - nRBCollider->bbHeight / 2), theta);
                    bottomLeft = glm::vec2(bottomLeft.x + nRBTrans->x + nRBCollider->x, bottomLeft.y + nRBTrans->y + nRBCollider->y);
                    bottomRight = glm::rotate(glm::vec2(nRBCollider->bbWidth / 2, 0 - nRBCollider->bbHeight / 2), theta);
                    bottomRight = glm::vec2(bottomRight.x + nRBTrans->x + nRBCollider->x, bottomRight.y + nRBTrans->y + nRBCollider->y);
                }
                else if(noRigidbody.has_component<CapsuleCollider>()){
                    ComponentHandle<CapsuleCollider> nRBCollider = noRigidbody.component<CapsuleCollider>();
                    //Check for trigger:
                    if(nRBCollider->isTrigger){
                        return;
                    }
                    float theta = nRBTrans->angle * M_PI / 180.0f;
                    topLeft = glm::rotate(glm::vec2(0 - nRBCollider->bbWidth / 2, nRBCollider->bbHeight / 2), theta);
                    topLeft = glm::vec2(topLeft.x + nRBTrans->x + nRBCollider->x, topLeft.y + nRBTrans->y + nRBCollider->y);
                    topRight = glm::rotate(glm::vec2(nRBCollider->bbWidth / 2, nRBCollider->bbHeight / 2), theta);
                    topRight = glm::vec2(topRight.x + nRBTrans->x + nRBCollider->x, topRight.y + nRBTrans->y + nRBCollider->y);
                    bottomLeft = glm::rotate(glm::vec2(0 - nRBCollider->bbWidth / 2, 0 - nRBCollider->bbHeight / 2), theta);
                    bottomLeft = glm::vec2(bottomLeft.x + nRBTrans->x + nRBCollider->x, bottomLeft.y + nRBTrans->y + nRBCollider->y);
                    bottomRight = glm::rotate(glm::vec2(nRBCollider->bbWidth / 2, 0 - nRBCollider->bbHeight / 2), theta);
                    bottomRight = glm::vec2(bottomRight.x + nRBTrans->x + nRBCollider->x, bottomRight.y + nRBTrans->y + nRBCollider->y);
                }
                //Center of mass of the rigidbody
                glm::vec2 rbCoM = glm::vec2(hRBTrans->x + hRB->cmX, hRB->cmY - nRBTrans->y);

                //Vectors of the edges of the bounding box:            
                //Test the tip against the rect points:
                glm::vec2 tip2TL = topLeft - rbCoM;
                glm::vec2 tip2TR = topRight - rbCoM;
                glm::vec2 tip2BL = bottomLeft - rbCoM;
                glm::vec2 tip2BR = bottomRight - rbCoM;

                //Closest points on the rect to the CoM
                glm::vec2 closestPoint;
                glm::vec2 nextClosest;

                float minDistance = glm::length(tip2TL);
                closestPoint = topLeft;
                float nextMin = glm::length(tip2TR);
                nextClosest = topRight;

                if(glm::length(tip2TL) < glm::length(tip2TR)){
                    float minDistance = glm::length(tip2TL);
                    closestPoint = topLeft;
                    float nextMin = glm::length(tip2TR);
                    nextClosest = topRight;
                } else {
                    float minDistance = glm::length(tip2TR);
                    closestPoint = topRight;
                    float nextMin = glm::length(tip2TL);
                    nextClosest = topLeft;
                }

                if(glm::length(tip2BL) < minDistance){
                    nextMin = minDistance;
                    nextClosest = closestPoint;
                    minDistance = glm::length(tip2BL);
                    closestPoint = bottomLeft;
                } else if(glm::length(tip2BL) < nextMin){
                    nextMin = glm::length(tip2BL);
                    nextClosest = bottomLeft;
                }

                if(glm::length(tip2BR) < minDistance){
                    nextMin = minDistance;
                    nextClosest = closestPoint;
                    minDistance = glm::length(tip2BR);
                    closestPoint = bottomRight;
                } else if(glm::length(tip2BR) < nextMin){
                    nextMin = glm::length(tip2BR);
                    nextClosest = bottomRight;
                }

                glm::vec2 bestPointOnRect = ClosestPointOnLineSegment(closestPoint, nextClosest, rbCoM);
                glm::vec2 reflectNorm = rbCoM - bestPointOnRect;

                //Calculate the reflection of the velocity vector:
                glm::vec2 newVelocity = rbVelocity - 2 * glm::dot(rbVelocity, reflectNorm) / glm::dot(reflectNorm, reflectNorm) * reflectNorm;

                hRB->velocityX = newVelocity.x;
                hRB->velocityY = newVelocity.y;
            }
        }
        #pragma endregion //collision algorithms

};