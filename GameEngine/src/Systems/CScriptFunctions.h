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

    /**
     * Setter function for setting the current entity that we are using the custom script on.  
     */
    void setCurrEntity(Entity* e) {
        currEntity = e;
    } 

    /**
     * Moves the current entity to relative position by x, y and/or z. It does this by calculating the new coordinates
     * and passing it into the physics system to handle movement and collisions.
     * PARAM: x direction
     * PARAM: y direction
     * PARAM: z direction  
     */
    void moveEntity(float x, float y, float z, string applyDt, double dt) 
    {
        if (!currEntity->has_component<Transform>()) {
            return;
        }
        bool isApplyDt = applyDt == "true" ? true : false;
        // Calculate new coordinates
        ComponentHandle<Transform> handle = currEntity->component<Transform>();
        float actualX = x;
        float actualY = y;
        float actualZ = z;
        if (isApplyDt) {
            actualX = x * dt * 10.0;
            actualY = y * dt * 10.0;
            actualZ = z * dt * 10.0;
        }
        float newX = handle.get()->x + actualX;
        float newY = handle.get()->y + actualY;
        float newZ = handle.get()->z + actualZ;

        // Emit event to send to physics system
        Entity* temp = currEntity;
        Engine::getInstance().events.emit<MoveTo>(*currEntity, newX, newY, newZ);
        setCurrEntity(temp);
    }

    /**
     * Removes the entity from the current scene.  
     */
    void removeEntity() 
    {
        currEntity->destroy();
    }

    /**
     * Sets the entity's active.
     * PARAM: active the state to set the active to.  
     */
    void setActive(bool active) 
    {
        if (!currEntity->has_component<Active>())
            return; 
        
        ComponentHandle<Active> activeHandle = currEntity->component<Active>();
        activeHandle.get()->setActiveStatus(active);
    }

    /**
     * Loads a scene.
     * PARAM: sceneName name of the scene to load.
     */
    void loadScene(string sceneName) 
    {
        SceneManager::getInstance().loadScene(sceneName);
    }

    /**
     * Changes sprite to new sprite.
     * PARAM: texturePath path to new sprite texture.  
     */
    void changeSprite(string texturePath) 
    {
        if (!currEntity->has_component<TextureComp>())
            return;
        
        ComponentHandle<TextureComp> sprite = currEntity->component<TextureComp>();
        if (sprite.get()->filepath != texturePath.c_str()) {
            // The texturePath to char *
            int n = texturePath.length();
            char *chararray= new char [n+1];
            strcpy(chararray, texturePath.c_str());

            // Change texture path to new texture path
            sprite.get()->filepath = chararray;
        }
    }

    /**
     * Matches the specified entity's position with the current entity's position.
     * PARAM: value name of the specified entity.
     * PARAM: cScript handle to current entity's CustomScript handle  
     */
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

    /**
     * Logs the value to a log file.
     * PARAM: value string to be logged the log file.
     */
    void log(string value) {
        Logger::getInstance() << value << "\n";
    }

    /**
     * Updates a stored variable's value to a new value.
     * PARAM: varName variable's name
     * PARAM: varType the data type of that variable. Must be type of int, float, double, string, bool, entity.
     * PARAM: value the new value
     * PARAM: cScript handle to current entity's CustomScript handle  
     */
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

    void flipBool(string varName, ComponentHandle<CustomScript> cScript) {
        bool val = cScript->bools.at(varName);
        if (val == true) {
            cScript->bools.at(varName) = false;
        } else {
            cScript->bools.at(varName) = true;
        }
    }

    /**
     * Add a stored variable's value to a new value.
     * PARAM: varName variable's name
     * PARAM: varType the data type of that variable. Must be type of int, float, double, string, bool, entity.
     * PARAM: value the new value
     * PARAM: cScript handle to current entity's CustomScript handle  
     */
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

    /**
     * Subtract a stored variable's value to a new value.
     * PARAM: varName variable's name
     * PARAM: varType the data type of that variable. Must be type of int, float, double, string, bool, entity.
     * PARAM: value the new value
     * PARAM: cScript handle to current entity's CustomScript handle  
     */
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

    /**
     * Multiple a stored variable's value to a new value.
     * PARAM: varName variable's name
     * PARAM: varType the data type of that variable. Must be type of int, float, double, string, bool, entity.
     * PARAM: value the new value
     * PARAM: cScript handle to current entity's CustomScript handle  
     */
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

    /**
     * Divide a stored variable's value to a new value.
     * PARAM: varName variable's name
     * PARAM: varType the data type of that variable. Must be type of int, float, double, string, bool, entity.
     * PARAM: value the new value
     * PARAM: cScript handle to current entity's CustomScript handle  
     */
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

    /**
     * Getter function for getting the current entity that we are using the custom script on.  
     * RETURN: The current entity
     */
    Entity* getCurrEntity() {
        return currEntity;
    }
}
