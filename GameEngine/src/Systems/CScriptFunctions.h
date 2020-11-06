#pragma once

#include <entityx/entityx.h>

#include "../Components/Components.h"
#include "../Events/Events.h"
#include "../SceneManager.h"
#include "../logger.h"
#include "../engine.h"

namespace CScript 
{
    // Variables
    entityx::Entity* currEntity;    

    // Functions
    void moveEntity(float x, float y, float z) {
        if (!currEntity->has_component<Transform>()) {
            return;
        }
        ComponentHandle<Transform> handle = currEntity->component<Transform>();
        float newX = handle.get()->x + x;
        float newY = handle.get()->y + y;
        float newZ = handle.get()->z + z;
        Engine::getInstance().events.emit<MoveTo>(*currEntity, newX, newY, newZ);
    }

    void removeEntity() {
        currEntity->destroy();
    }

    void setActive(bool active) {
        if (!currEntity->has_component<Active>())
            return; 
        
        ComponentHandle<Active> _active = currEntity->component<Active>();
        _active.get()->setActiveStatus(active);
    }

    void loadScene(string sceneName) {
        SceneManager::getInstance().loadScene(sceneName);
    }

    void changeSprite(string texturePath) {
        if (!currEntity->has_component<TextureComp>())
            return;
        
        ComponentHandle<TextureComp> sprite = currEntity->component<TextureComp>();
        if (sprite.get()->filepath != texturePath.c_str()) {
            int n = texturePath.length();
            char *chararray= new char [n+1];
            strcpy(chararray, texturePath.c_str());

            sprite.get()->filepath = chararray;
        }
    }

    void matchEntityPos(string& value, ComponentHandle<CustomScript>& cScript) {

        if (!cScript.get()->containsVariable(value))
            return;

        entityx::Entity other = cScript.get()->entities.at(value);

        if (!other.has_component<Transform>() || !currEntity->has_component<Transform>())
            return;

        ComponentHandle<Transform> otherT = other.component<Transform>();
        ComponentHandle<Transform> thisT = currEntity->component<Transform>();

        otherT.get()->x = thisT.get()->x;
        otherT.get()->y = thisT.get()->y;
        otherT.get()->z = thisT.get()->z;
    }

    void log(string value) {
        Logger::getInstance() << value << "\n";
    }

    Entity* getCurrEntity() {
        return currEntity;
    }

    void setCurrEntity(Entity* e) {
        currEntity = e;
    }

    void updateVar(string varName, string varType, string value, ComponentHandle<CustomScript> cScript) {
        if (varType == "int")
            cScript.get()->ints.at(varName) = stoi(value, nullptr, 0);

        if (varType == "float")
            cScript.get()->floats.at(varName) = stof(value);

        if (varType == "double") {
            if (value == "deltaTime") {
                cScript.get()->doubles.at(varName) = cScript.get()->doubles.at("deltaTime");
            } else {
                cScript.get()->doubles.at(varName) = stod(value);
            }
        }
        
        if (varType == "string")
            cScript.get()->strings.at(varName) = value;

        if (varType == "bool") 
            cScript.get()->bools.at(varName) = (value == "true") ? true : false;

        if (varType == "entity") {
            auto entities = Engine::getInstance().entities.entities_with_components<Name>();

            if (value == "collisionObject-name")
                value = cScript.get()->strings.at("collisionObject-name");

            for (Entity e : entities) {
                ComponentHandle<Name> entityName = e.component<Name>();
                if (entityName.get()->getName().compare(value) == 0) {
                    cScript.get()->entities.at(varName) = e;
                    break;
                }
            }
        }
    }

    void addVar(string varName, string varType, string value, ComponentHandle<CustomScript> cScript) {
        if (varType == "int")
            cScript.get()->ints.at(varName) += stoi(value, nullptr, 0);

        if (varType == "float")
            cScript.get()->floats.at(varName) += stof(value);

        if (varType == "double") {
            if (value == "deltaTime") {
                cScript.get()->doubles.at(varName) += cScript.get()->doubles.at("deltaTime");
            } else {
                cScript.get()->doubles.at(varName) += stod(value);
            }
        }

        if (varType == "string")
            cScript.get()->strings.at(varName) += value;
    }

    void subVar(string varName, string varType, string value, ComponentHandle<CustomScript> cScript) {
        if (varType == "int")
            cScript.get()->ints.at(varName) -= stoi(value, nullptr, 0);

        if (varType == "float")
            cScript.get()->floats.at(varName) -= stof(value);

        if (varType == "double") {
            if (value == "deltaTime") {
                cScript.get()->doubles.at(varName) -= cScript.get()->doubles.at("deltaTime");
            } else {
                cScript.get()->doubles.at(varName) -= stod(value);
            }
        }
    }

    void multiVar(string varName, string varType, string value, ComponentHandle<CustomScript> cScript) {
        if (varType == "int")
            cScript.get()->ints.at(varName) *= stoi(value, nullptr, 0);

        if (varType == "float")
            cScript.get()->floats.at(varName) *= stof(value);

        if (varType == "double") {
            if (value == "deltaTime") {
                cScript.get()->doubles.at(varName) *= cScript.get()->doubles.at("deltaTime");
            } else {
                cScript.get()->doubles.at(varName) *= stod(value);
            }
        }
    }

    void divideVar(string varName, string varType, string value, ComponentHandle<CustomScript> cScript) {
        if (varType == "int")
            cScript.get()->ints.at(varName) /= stoi(value, nullptr, 0);

        if (varType == "float")
            cScript.get()->floats.at(varName) /= stof(value);

        if (varType == "double") {
            if (value == "deltaTime") {
                cScript.get()->doubles.at(varName) /= cScript.get()->doubles.at("deltaTime");
            } else {
                cScript.get()->doubles.at(varName) /= stod(value);
            }
        }
    }
}
