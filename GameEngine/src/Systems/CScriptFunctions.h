#pragma once

#include <entityx/entityx.h>
#include <cmath>

#include "../Components/Components.h"
#include "../Events/Events.h"
#include "../SceneManager.h"
#include "../logger.h"
#include "../engine.h"

namespace CScript 
{
    // Variables
    entityx::Entity* currEntity;
    unordered_map<std::string, int> ints;
    unordered_map<std::string, float> floats;
    unordered_map<std::string, double> doubles;
    unordered_map<std::string, std::string> strings;
    unordered_map<std::string, bool> bools;
    unordered_map<std::string, entityx::Entity> entities;

    /**
     * Initializes any reserved variables.
     */
    void initVariables() {
        doubles.insert(make_pair("deltaTime", 0.0f));
    }

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
        if (!currEntity->valid()) {
            return;
        }
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
        if (!currEntity->valid()) {
            return;
        }
        Entity& e = *currEntity;
        currEntity = nullptr;
        e.destroy();
    }

    /**
     * Sets the entity's active.
     * PARAM: active the state to set the active to.  
     */
    void setActive(bool active) 
    {
        if (!currEntity->valid()) {
            return;
        }
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
        if (!currEntity->valid()) {
            return;
        }
        Engine::getInstance().loadScene(sceneName);
    }

    /**
     * Changes sprite to new sprite.
     * PARAM: texturePath path to new sprite texture.  
     */
    void changeSprite(string texturePath) 
    {
        if (!currEntity->valid()) {
            return;
        }

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
        if (!currEntity->valid() || !cScript.valid()) {
            return;
        }
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
        if (!currEntity->valid() || !cScript.valid()) {
            return;
        }
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
        if (!currEntity->valid() || !cScript.valid()) {
            return;
        }
        bool val = cScript->bools.at(varName);
        if (val == true) {
            cScript->bools.at(varName) = false;
        } else {
            cScript->bools.at(varName) = true;
        }
    }

    void distanceTo(string entityVariableName, string floatVariableName, ComponentHandle<CustomScript> cScript) {
        if (!currEntity->valid())
            return;

        if (!cScript->containsVariable(entityVariableName) || !cScript->containsVariable(floatVariableName)) {
            Logger::getInstance() << "distanceTo: entityVariableName or floatVariableName do not exist";
            return;
        }

        Entity other = cScript->entities.at(entityVariableName);

        if (!other.has_component<Transform>() || !currEntity->has_component<Transform>())
            return;

        ComponentHandle<Transform> otherT = other.component<Transform>();
        ComponentHandle<Transform> thisT = currEntity->component<Transform>();

        float distance = pow((otherT.get()->x - thisT.get()->x), 2) + pow((otherT.get()->y - thisT.get()->y), 2) + pow((otherT.get()->z - thisT.get()->z), 2);
        distance = sqrt(distance);

        cScript->floats.at(floatVariableName) = distance;
    }
     
    void vectorTo(string entityVariableName, string floatVariableXName, string floatVariableYName, ComponentHandle<CustomScript> cScript) {
        if (!currEntity->valid())
            return;

        if (!cScript->containsVariable(entityVariableName) || !cScript->containsVariable(floatVariableXName) || !cScript->containsVariable(floatVariableYName)) {
            Logger::getInstance() << "vectorTo: entityVariableName, floatVariableXName or floatVariableYName do not exist";
            return;
        }

        Entity other = cScript->entities.at(entityVariableName);

        if (!other.has_component<Transform>() || !currEntity->has_component<Transform>())
            return;

        ComponentHandle<Transform> otherT = other.component<Transform>();
        ComponentHandle<Transform> thisT = currEntity->component<Transform>();

        float x = otherT.get()->x - thisT.get()->x;
        float y = otherT.get()->y - thisT.get()->y;

        float mag = sqrt(x * x + y * y);

        if (mag > 0) {
            x /= mag;
            y /= mag;
        }

        cScript.get()->floats.at(floatVariableXName) = x;
        cScript.get()->floats.at(floatVariableYName) = y;
    }

    void moveEntityByVars(string nameOfXFloatVariable, string nameOfYFloatVariable, string nameOfZFloatVariable, string applyDt, ComponentHandle<CustomScript> cScript) {
        if (!currEntity->valid())
            return;

        float x = 0;
        float y = 0;
        float z = 0;

        if (cScript->containsVariable(nameOfXFloatVariable))
            x = cScript->floats.at(nameOfXFloatVariable);

        if (cScript->containsVariable(nameOfYFloatVariable))
            y = cScript->floats.at(nameOfYFloatVariable);

        if (cScript->containsVariable(nameOfZFloatVariable))
            z = cScript->floats.at(nameOfZFloatVariable);

        moveEntity(x, y, z, applyDt, cScript.get()->doubles.at("deltaTime"));
    }

    /**
     * Add a stored variable's value to a new value.
     * PARAM: varName variable's name
     * PARAM: varType the data type of that variable. Must be type of int, float, double, string, bool, entity.
     * PARAM: value the new value
     * PARAM: cScript handle to current entity's CustomScript handle  
     */
    void addVar(string varName, string varType, string value, ComponentHandle<CustomScript> cScript) {
        if (!currEntity->valid() || !cScript.valid()) {
            return;
        }

        ComponentHandle<Name> ename = currEntity->component<Name>();

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
        if (!currEntity->valid() || !cScript.valid()) {
            return;
        }
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
        if (!currEntity->valid() || !cScript.valid()) {
            return;
        }
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
        if (!currEntity->valid() || !cScript.valid()) {
            return;
        }
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

    void playAudio() {
        Engine::getInstance().events.emit<AudioCommand>(*currEntity, AudioCommandType::PLAY);
    }

    void pauseAudio() {
        Engine::getInstance().events.emit<AudioCommand>(*currEntity, AudioCommandType::PAUSE);
    }

    void stopAudio() {
        Engine::getInstance().events.emit<AudioCommand>(*currEntity, AudioCommandType::STOP);
    }

    /**
     * Getter function for getting the current entity that we are using the custom script on.  
     * RETURN: The current entity
     */
    Entity* getCurrEntity() {
        return currEntity;
    }
}
