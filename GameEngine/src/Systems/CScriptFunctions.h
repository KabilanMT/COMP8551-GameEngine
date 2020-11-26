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

    const std::unordered_map<std::string, int> possibleInputs {
        {"KEY_W", GLFW_KEY_W}, 
        {"KEY_S", GLFW_KEY_S},
        {"KEY_A", GLFW_KEY_A},
        {"KEY_D", GLFW_KEY_D},
        {"KEY_Q", GLFW_KEY_Q},
        {"KEY_E", GLFW_KEY_E},
        {"KEY_R", GLFW_KEY_R},
        {"KEY_F", GLFW_KEY_F},
        {"KEY_I", GLFW_KEY_I},
        {"KEY_SPACE", GLFW_KEY_SPACE}
    }; 

    /**
     * Initializes any reserved variables.
     */
    void initVariables() {
        doubles.insert(make_pair("deltaTime", 0.0f));
    }

    bool containsGlobalVariable(string varName) {
        if (ints.find(varName) != ints.end() || floats.find(varName) != floats.end()
            || doubles.find(varName) != doubles.end() || strings.find(varName) != strings.end()
            || bools.find(varName) != bools.end() || entities.find(varName) != entities.end())
            return true; 
        
        return false;
    }

    int getValidKeyPress(string key) {
        return possibleInputs.at(key);
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
        if (strcmp(sprite->filepath, texturePath.c_str())) {
            // The texturePath to char *
            int n = texturePath.length();
            char *chararray= new char [n+1];
            strcpy(chararray, texturePath.c_str());

            // Change texture path to new texture path
            sprite.get()->filepath = chararray;
            sprite->deInit();
        }
    }

    void setAudioSource(string& audioPath) {
        if (!currEntity->valid()) {
            return;
        }

        if(!currEntity->has_component<AudioSource>())
            return;

        ComponentHandle<AudioSource> handle = currEntity->component<AudioSource>();
        if (handle->name != audioPath) {
            handle->cleanUp();
            handle->name = audioPath;
            handle->initSound();
        }
    }

    void setPosition(string& xVariable, string& yVariable, ComponentHandle<CustomScript>& cScript) {
        if (!currEntity->valid() || !cScript.valid() || !currEntity->has_component<Transform>()) {
            return;
        }

        ComponentHandle<Transform> handle = currEntity->component<Transform>();

        if (!handle.valid()) {
            return;
        }

        if (!cScript->containsVariable(xVariable) || !cScript->containsVariable(yVariable))
            return;

        handle->x = cScript->floats.at(xVariable);
        handle->y = cScript->floats.at(yVariable);
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

        entityx::Entity other;

        if (cScript.get()->containsVariable(value))
            other = cScript.get()->entities.at(value);
        else if (containsGlobalVariable(value))
            other = entities.at(value);
        else
            return;

        if (!other.valid() || !other.has_component<Transform>() || !currEntity->valid() || !currEntity->has_component<Transform>())
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

    void distanceTo(string entityVariableName, string floatVariableName, ComponentHandle<CustomScript> cScript) {
        if (!currEntity->valid())
            return;

        if ((!cScript->containsVariable(entityVariableName) && !containsGlobalVariable(entityVariableName)) 
            || (!cScript->containsVariable(floatVariableName) && !containsGlobalVariable(floatVariableName))) {
            Logger::getInstance() << "distanceTo: entityVariableName or floatVariableName do not exist";
            return;
        }

        entityx::Entity other;

        if (cScript.get()->containsVariable(entityVariableName))
            other = cScript.get()->entities.at(entityVariableName);
        else if (containsGlobalVariable(entityVariableName))
            other = entities.at(entityVariableName);

        if (!other.has_component<Transform>() || !currEntity->has_component<Transform>())
            return;

        ComponentHandle<Transform> otherT = other.component<Transform>();
        ComponentHandle<Transform> thisT = currEntity->component<Transform>();

        float distance = pow((otherT.get()->x - thisT.get()->x), 2) + pow((otherT.get()->y - thisT.get()->y), 2) + pow((otherT.get()->z - thisT.get()->z), 2);
        distance = sqrt(distance);

        if (cScript.get()->containsVariable(floatVariableName))
            cScript->floats.at(floatVariableName) = distance;
        else if (containsGlobalVariable(floatVariableName))
            cScript->floats.at(floatVariableName) = distance;
    }
     
    void vectorTo(string entityVariableName, string floatVariableXName, string floatVariableYName, ComponentHandle<CustomScript> cScript) {
        if (!currEntity->valid())
            return;

        if ((!cScript->containsVariable(entityVariableName) && !containsGlobalVariable(entityVariableName)) 
            || (!cScript->containsVariable(floatVariableXName) && !containsGlobalVariable(floatVariableXName)) 
            || (!cScript->containsVariable(floatVariableYName) && !containsGlobalVariable(floatVariableYName))) {
            Logger::getInstance() << "vectorTo: entityVariableName, floatVariableXName or floatVariableYName do not exist";
            return;
        }

        entityx::Entity other;

        if (cScript.get()->containsVariable(entityVariableName))
            other = cScript.get()->entities.at(entityVariableName);
        else if (containsGlobalVariable(entityVariableName))
            other = entities.at(entityVariableName);

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

        if (containsGlobalVariable(nameOfXFloatVariable))
            x = floats.at(nameOfXFloatVariable);
        else if (cScript->containsVariable(nameOfXFloatVariable))
            x = cScript->floats.at(nameOfXFloatVariable);

        if (containsGlobalVariable(nameOfYFloatVariable))
            y = floats.at(nameOfYFloatVariable);
        else if (cScript->containsVariable(nameOfYFloatVariable))
            y = cScript->floats.at(nameOfYFloatVariable);

        if (containsGlobalVariable(nameOfZFloatVariable))
            z = floats.at(nameOfZFloatVariable);
        else if (cScript->containsVariable(nameOfZFloatVariable))
            z = cScript->floats.at(nameOfZFloatVariable);

        moveEntity(x, y, z, applyDt, doubles.at("deltaTime"));
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

        if (!cScript.get()->containsVariable(varName))
            return;

        if (varType == "int")
            cScript.get()->ints.at(varName) = stoi(value, nullptr, 0);

        if (varType == "float")
            cScript.get()->floats.at(varName) = stof(value);

        if (varType == "double") {
            if (value == "deltaTime") {
                cScript.get()->doubles.at(varName) = doubles.at("deltaTime");
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

            if (value == "") {
                Entity e;
                cScript.get()->entities.at(varName) = e;
                return;
            }

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

    void setEqual(string varName, string varType, string value, string isGlobal, ComponentHandle<CustomScript> cScript) {
        if (!currEntity->valid() || !cScript.valid()) {
            return;
        }

        if (!cScript.get()->containsVariable(varName))
            return;
        bool global = isGlobal == "true" ? true : false;
        if (!global) {
            if (!cScript.get()->containsVariable(value)) {
                return;
            }

            if (varType == "int")
                cScript.get()->ints.at(varName) = cScript.get()->ints.at(value);

            if (varType == "float")
                cScript.get()->floats.at(varName) = cScript.get()->floats.at(value);

            if (varType == "double")
                cScript.get()->doubles.at(varName) = cScript.get()->doubles.at(value);
            
            if (varType == "string")
                cScript.get()->strings.at(varName) = cScript.get()->strings.at(value);;

            if (varType == "bool") 
                cScript.get()->bools.at(varName) = cScript.get()->bools.at(value);

            if (varType == "entity")
                cScript.get()->entities.at(varName) = cScript.get()->entities.at(value);
        } else {
            if (!containsGlobalVariable(value)) {
                return;
            }

            if (varType == "int")
                cScript.get()->ints.at(varName) = ints.at(value);

            if (varType == "float")
                cScript.get()->floats.at(varName) = floats.at(value);

            if (varType == "double")
                cScript.get()->doubles.at(varName) = doubles.at(value);
            
            if (varType == "string")
                cScript.get()->strings.at(varName) = strings.at(value);;

            if (varType == "bool") 
                cScript.get()->bools.at(varName) = bools.at(value);

            if (varType == "entity")
                cScript.get()->entities.at(varName) = entities.at(value);
        }
    }

    void flipBool(string varName, ComponentHandle<CustomScript> cScript) {
        if (!currEntity->valid() || !cScript.valid()) {
            return;
        }
                
        if (!cScript.get()->containsVariable(varName))
            return;
        
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
        if (!currEntity->valid() || !cScript.valid()) {
            return;
        }
        
        if (!cScript.get()->containsVariable(varName))
            return;

        ComponentHandle<Name> ename = currEntity->component<Name>();

        if (varType == "int")
            cScript.get()->ints.at(varName) += stoi(value, nullptr, 0);

        if (varType == "float")
            cScript.get()->floats.at(varName) += stof(value);

        if (varType == "double") {
            if (value == "deltaTime") {
                cScript.get()->doubles.at(varName) += doubles.at("deltaTime");
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
        
        if (!cScript.get()->containsVariable(varName))
            return;

        if (varType == "int")
            cScript.get()->ints.at(varName) -= stoi(value, nullptr, 0);

        if (varType == "float")
            cScript.get()->floats.at(varName) -= stof(value);

        if (varType == "double") {
            if (value == "deltaTime") {
                cScript.get()->doubles.at(varName) -= doubles.at("deltaTime");
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
        
        if (!cScript.get()->containsVariable(varName))
            return;

        if (varType == "int")
            cScript.get()->ints.at(varName) *= stoi(value, nullptr, 0);

        if (varType == "float")
            cScript.get()->floats.at(varName) *= stof(value);

        if (varType == "double") {
            if (value == "deltaTime") {
                cScript.get()->doubles.at(varName) *= doubles.at("deltaTime");
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

        if (!cScript.get()->containsVariable(varName))
            return;

        if (varType == "int")
            cScript.get()->ints.at(varName) /= stoi(value, nullptr, 0);

        if (varType == "float")
            cScript.get()->floats.at(varName) /= stof(value);

        if (varType == "double") {
            if (value == "deltaTime") {
                cScript.get()->doubles.at(varName) /= doubles.at("deltaTime");
            } else {
                cScript.get()->doubles.at(varName) /= stod(value);
            }
        }
    }    
    
    /**
     * Modulus a stored variable's value to a new value.
     * PARAM: varName variable's name
     * PARAM: varType the data type of that variable. Must be type of int.
     * PARAM: value the new value
     * PARAM: cScript handle to current entity's CustomScript handle  
     */
    void modVar(string varName, string varType, string value, ComponentHandle<CustomScript> cScript) {
        if (!currEntity->valid() || !cScript.valid()) {
            return;
        }

        if (!cScript->containsVariable(varName))
            return;
        
        if (varType == "int")
            cScript->ints.at(varName) %= stoi(value, nullptr, 0);
    }

    /**
     * Updates a stored variable's value to a new value.
     * PARAM: varName variable's name
     * PARAM: varType the data type of that variable. Must be type of int, float, double, string, bool, entity.
     * PARAM: value the new value
     * PARAM: cScript handle to current entity's CustomScript handle  
     */
    void updateGlobalVar(string varName, string varType, string value, ComponentHandle<CustomScript> cScript) {
        if (!currEntity->valid() || !cScript.valid()) {
            return;
        }

        if (!containsGlobalVariable(varName))
            return;

        if (varType == "int")
            ints.at(varName) = stoi(value, nullptr, 0);

        if (varType == "float")
            floats.at(varName) = stof(value);

        if (varType == "double") {
            if (value == "deltaTime") {
                doubles.at(varName) = doubles.at("deltaTime");
            } else {
                doubles.at(varName) = stod(value);
            }
        }
        
        if (varType == "string")
            strings.at(varName) = value;

        if (varType == "bool") 
            bools.at(varName) = (value == "true") ? true : false;

        if (varType == "entity") {
            auto engineEntities = Engine::getInstance().entities.entities_with_components<Name>();

            for (Entity e : engineEntities) {
                ComponentHandle<Name> entityName = e.component<Name>();
                if (entityName.get()->getName().compare(value) == 0) {
                    entities.at(varName) = e;
                    break;
                }
            }
        }
    }

    void setGlobalEqual(string varName, string varType, string value, string isGlobal, ComponentHandle<CustomScript> cScript) {
        if (!currEntity->valid() || !cScript.valid()) {
            return;
        }

        if (!containsGlobalVariable(varName))
            return;
        bool global = isGlobal == "true" ? true : false;
        if (!global) {
            if (!cScript->containsVariable(value)) {
                return;
            }
            if (varType == "int")
                ints.at(varName) = cScript.get()->ints.at(value);

            if (varType == "float")
                floats.at(varName) = cScript.get()->floats.at(value);

            if (varType == "double")
                doubles.at(varName) = cScript.get()->doubles.at(value);
            
            if (varType == "string")
                strings.at(varName) = cScript.get()->strings.at(value);;

            if (varType == "bool") 
                bools.at(varName) = cScript.get()->bools.at(value);

            if (varType == "entity")
                entities.at(varName) = cScript.get()->entities.at(value);
        } else {
            if (!containsGlobalVariable(value)) {
                return;
            }

            if (varType == "int")
                ints.at(varName) = ints.at(value);

            if (varType == "float")
                floats.at(varName) = floats.at(value);

            if (varType == "double")
                doubles.at(varName) = doubles.at(value);
            
            if (varType == "string")
                strings.at(varName) = strings.at(value);;

            if (varType == "bool") 
                bools.at(varName) = bools.at(value);

            if (varType == "entity")
                entities.at(varName) = entities.at(value);
        }
    }

    void flipGlobalBool(string varName, ComponentHandle<CustomScript> cScript) {
        if (!currEntity->valid() || !cScript.valid()) {
            return;
        }

        if (!containsGlobalVariable(varName))
            return;

        bool val = bools.at(varName);
        if (val == true) {
            bools.at(varName) = false;
        } else {
            bools.at(varName) = true;
        }
    }

    /**
     * Add a stored variable's value to a new value.
     * PARAM: varName variable's name
     * PARAM: varType the data type of that variable. Must be type of int, float, double, string, bool, entity.
     * PARAM: value the new value
     * PARAM: cScript handle to current entity's CustomScript handle  
     */
    void addGlobalVar(string varName, string varType, string value, ComponentHandle<CustomScript> cScript) {
        if (!currEntity->valid() || !cScript.valid()) {
            return;
        }

        if (!containsGlobalVariable(varName))
            return;

        ComponentHandle<Name> ename = currEntity->component<Name>();

        if (varType == "int")
            ints.at(varName) += stoi(value, nullptr, 0);

        if (varType == "float")
            floats.at(varName) += stof(value);

        if (varType == "double") {
            if (value == "deltaTime") {
                doubles.at(varName) += doubles.at("deltaTime");
            } else {
                doubles.at(varName) += stod(value);
            }
        }

        if (varType == "string")
            strings.at(varName) += value;
    }

    /**
     * Subtract a stored variable's value to a new value.
     * PARAM: varName variable's name
     * PARAM: varType the data type of that variable. Must be type of int, float, double, string, bool, entity.
     * PARAM: value the new value
     * PARAM: cScript handle to current entity's CustomScript handle  
     */
    void subGlobalVar(string varName, string varType, string value, ComponentHandle<CustomScript> cScript) {
        if (!currEntity->valid() || !cScript.valid()) {
            return;
        }

        if (!containsGlobalVariable(varName))
            return;

        if (varType == "int")
            ints.at(varName) -= stoi(value, nullptr, 0);

        if (varType == "float")
            floats.at(varName) -= stof(value);

        if (varType == "double") {
            if (value == "deltaTime") {
                doubles.at(varName) -= doubles.at("deltaTime");
            } else {
                doubles.at(varName) -= stod(value);
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
    void multiGlobalVar(string varName, string varType, string value, ComponentHandle<CustomScript> cScript) {
        if (!currEntity->valid() || !cScript.valid()) {
            return;
        }

        if (!containsGlobalVariable(varName))
            return;

        if (varType == "int")
            ints.at(varName) *= stoi(value, nullptr, 0);

        if (varType == "float")
            floats.at(varName) *= stof(value);

        if (varType == "double") {
            if (value == "deltaTime") {
                doubles.at(varName) *= cScript.get()->doubles.at("deltaTime");
            } else {
                doubles.at(varName) *= stod(value);
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
    void divideGlobalVar(string varName, string varType, string value, ComponentHandle<CustomScript> cScript) {
        if (!currEntity->valid() || !cScript.valid()) {
            return;
        }

        if (!containsGlobalVariable(varName))
            return;
        
        if (varType == "int")
            ints.at(varName) /= stoi(value, nullptr, 0);

        if (varType == "float")
            floats.at(varName) /= stof(value);

        if (varType == "double") {
            if (value == "deltaTime") {
                doubles.at(varName) /= doubles.at("deltaTime");
            } else {
                doubles.at(varName) /= stod(value);
            }
        }
    }

    /**
     * Modulus a stored variable's value to a new value.
     * PARAM: varName variable's name
     * PARAM: varType the data type of that variable. Must be type of int.
     * PARAM: value the new value
     * PARAM: cScript handle to current entity's CustomScript handle  
     */
    void modGlobalVar(string varName, string varType, string value, ComponentHandle<CustomScript> cScript) {
        if (!currEntity->valid() || !cScript.valid()) {
            return;
        }

        if (!containsGlobalVariable(varName))
            return;
        
        if (varType == "int")
            ints.at(varName) %= stoi(value, nullptr, 0);
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
