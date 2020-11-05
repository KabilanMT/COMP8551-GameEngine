#pragma once

#include <fstream>
#include <iostream>
#include <string>
#include <functional>

#include "entityx/entityx.h"
#include "../Components/CustomScript.h"
#include "../Events/Events.h"
#include "../SceneManager.h"
#include "Input.h"
#include <glfw/glfw3.h>
#include "../logger.h"

using namespace entityx;

class CustomScriptSystem : public System<CustomScriptSystem>, public Receiver<CustomScriptSystem> {
public:
    void configure(EventManager& events) override {
        events.subscribe<SceneLoad>(*this);
        events.subscribe<Trigger>(*this);
    }

    void receive(const SceneLoad& sl) {
        //this method will be called when the scene loads

        // call start methods
        for (Entity e : sl.entities) {
            currEntity = &e;

            // Error checking to avoid crashing
            if (!e.has_component<CustomScript>())
                continue;

            ComponentHandle<CustomScript> handle = e.component<CustomScript>();
            XMLElement* variablesContent = handle->getVariables();
            if (variablesContent != nullptr)
                getVariables(variablesContent->FirstChild(), handle);

            XMLElement* startContent = handle->getStart();
            if (startContent != nullptr)
                runCommands(startContent->FirstChild(), handle);
        }
    }

    void receive(const Trigger& tr) {
        if (tr.gotTriggered->has_component<CustomScript>()) {
            currEntity = tr.gotTriggered;
            ComponentHandle<CustomScript> handle = tr.gotTriggered->component<CustomScript>();

            handle->strings.at("collisionObject-tag") = tr.triggeringEntity->component<Name>().get()->getName();

            XMLElement* collisionContent = handle->getOnCollision();
            if (collisionContent != nullptr)
                runCommands(collisionContent->FirstChild(), handle);

            handle->resetReservedVariables();
        }
    }

    void update(EntityManager& es, EventManager& events, TimeDelta dt) override 
    {
        auto entities = es.entities_with_components<CustomScript>();

        // Example of how to check if a key is pressed:
        // bool isSpacePressed = Input::getInstance().isKeyPressed(GLFW_KEY_SPACE)

        // Full list of key_press codes are here
        // https://www.glfw.org/docs/3.3/group__keys.html

        // Example of how to check if a mouse button was pressed
        // bool isLeftMousePressed = Input::getInstance().isMousePressed(true) //left mouse button

        // Example to check the position of the cursor
        // int xpos = Input::getInstance().getCursorPosition(true) //xcoord
        // int ypos = Input::getInstance().getCursorPosition(false) //ycoord

        for (Entity e : entities) {
            currEntity = &e;

            // Error checking to avoid crashing 
            if (!e.has_component<Active>() && !e.has_component<CustomScript>())
                continue;

            // Skip if entity isn't active
            ComponentHandle<Active> active = e.component<Active>();
            if (!active.get()->getIfActive())
                continue; 

            // Get update tag from xml and run commands
            ComponentHandle<CustomScript> handle = e.component<CustomScript>();
            XMLElement* updateContent = handle->getUpdate();

            if (updateContent != nullptr) {
                handle.get()->doubles.at("deltaTime") = dt;
                runCommands(updateContent->FirstChild(), handle);
            }
        }
    }

   private:
        Entity* currEntity;

        // Should be ran in start
        // TODO: Requires error checking and refactor, assumes that the first and second attrib is variable name and value
        void getVariables(XMLNode* variable, ComponentHandle<CustomScript> cScript) {
            while (variable != nullptr) {

                if (variable->ToElement() == NULL) {
                    variable = variable->NextSibling();
                    continue;
                }
                
                string name = variable->Value();
                const XMLAttribute* attr = variable->ToElement()->FirstAttribute();
                string var_name = attr->Value();
                string var_value = attr->Next()->Value(); 

                if (cScript->containsVariable(var_name)) {
                    Logger::getInstance() << var_name << " is already defined or is a reserved variable. \n";
                    variable = variable->NextSibling();
                    continue;
                }

                if (name == "int")
                    cScript.get()->ints.insert(make_pair(var_name, stoi(var_value, nullptr, 0)));

                if (name == "float")
                    cScript.get()->floats.insert(make_pair(var_name, stof(var_value)));

                if (name == "double")
                    cScript.get()->doubles.insert(make_pair(var_name, stod(var_value)));
                
                if (name == "string")
                    cScript.get()->strings.insert(make_pair(var_name, var_value));

                if (name == "bool") 
                    cScript.get()->bools.insert(make_pair(var_name, (var_value == "true") ? true : false));

                if (name == "entity") {
                    auto entities = Engine::getInstance().entities.entities_with_components<Name>();

                    for (Entity e : entities) {
                        ComponentHandle<Name> entityName = e.component<Name>();
                        if (entityName.get()->getName().compare(var_value) == 0) {
                            cScript.get()->entities.insert(make_pair(var_value, e));
                            break;
                        }
                    }
                }

                variable = variable->NextSibling();
            }
        }

        void runCommands(XMLNode* command, ComponentHandle<CustomScript> cScript) {
            while (command != NULL) {
                
                if (command->ToElement() == NULL) {
                    command = command->NextSibling();
                    continue;
                }
                
                // Setup
                string name = command->Value();
                const XMLAttribute* attr = command->ToElement()->FirstAttribute();
                unordered_map<string, string> attributes;

                // Get Attributes
                while (attr != NULL) {
                    attributes.insert(make_pair(attr->Name(), attr->Value()));
                    attr = attr->Next();
                }

                // Do commands
                if (name == "ifVar" && attributes.find("name") != attributes.end() && attributes.find("value") != attributes.end()) {
                    string type = attributes.at("type");

                    if (type == "int" && cScript.get()->ints.find(attributes.at("name")) != cScript.get()->ints.end()) {
                        int val = cScript.get()->ints.at(attributes.at("name"));
                        int valToCompare = stoi(attributes.at("value"), nullptr, 0);

                        if (val == valToCompare)
                            runCommands(command->FirstChild(), cScript);
                    }

                    if (type == "float" && cScript.get()->floats.find(attributes.at("name")) != cScript.get()->floats.end()) {
                        float val = cScript.get()->floats.at(attributes.at("name"));
                        float valToCompare = stof(attributes.at("value"));

                        if (val == valToCompare)
                            runCommands(command->FirstChild(), cScript);
                    }

                    if (type == "double" && cScript.get()->doubles.find(attributes.at("name")) != cScript.get()->doubles.end()) {
                        double val = cScript.get()->doubles.at(attributes.at("name"));
                        double valToCompare = 0;
                        if (attributes.at("value") == "deltaTime") {
                            valToCompare = cScript.get()->doubles.at("deltaTime");
                        } else {
                            valToCompare = stod(attributes.at("value"));
                        }

                        if (val == valToCompare)
                            runCommands(command->FirstChild(), cScript);
                    }
                    
                    if (type == "string" && cScript.get()->strings.find(attributes.at("name")) != cScript.get()->strings.end()) {
                        string val = cScript.get()->strings.at(attributes.at("name"));
                        string valToCompare = attributes.at("value");

                        if (val.compare(valToCompare) == 0)
                            runCommands(command->FirstChild(), cScript);
                    }

                    if (type == "bool" && cScript.get()->bools.find(attributes.at("name")) != cScript.get()->bools.end()) { 
                        bool val = cScript.get()->bools.at(attributes.at("name"));

                        if (val == (attributes.at("value") == "true" ? true : false))
                            runCommands(command->FirstChild(), cScript);   
                    }
                }

                if (name == "ifVarGreater" && attributes.find("name") != attributes.end() && attributes.find("value") != attributes.end()) {
                    string type = attributes.at("type");

                    if (type == "int" && cScript.get()->ints.find(attributes.at("name")) != cScript.get()->ints.end()) {
                        int val = cScript.get()->ints.at(attributes.at("name"));
                        int valToCompare = stoi(attributes.at("value"), nullptr, 0);

                        if (val > valToCompare)
                            runCommands(command->FirstChild(), cScript);
                    }

                    if (type == "float" && cScript.get()->floats.find(attributes.at("name")) != cScript.get()->floats.end()) {
                        float val = cScript.get()->floats.at(attributes.at("name"));
                        float valToCompare = stof(attributes.at("value"));

                        if (val > valToCompare)
                            runCommands(command->FirstChild(), cScript);
                    }

                    if (type == "double" && cScript.get()->doubles.find(attributes.at("name")) != cScript.get()->doubles.end()) {
                        double val = cScript.get()->doubles.at(attributes.at("name"));
                        double valToCompare = 0;
                        if (attributes.at("value") == "deltaTime") {
                            valToCompare = cScript.get()->doubles.at("deltaTime");
                        } else {
                            valToCompare = stod(attributes.at("value"));
                        }

                        if (val > valToCompare)
                            runCommands(command->FirstChild(), cScript);
                    }
                }

                if (name == "updateVar") {
                    string varName = attributes.at("name");
                    string varType = attributes.at("type");
                    string value = attributes.at("value");

                    updateVar(varName, varType, value, cScript);
                }

                if (name == "addVar") {
                    string varName = attributes.at("name");
                    string varType = attributes.at("type");
                    string value = attributes.at("value");

                    addVar(varName, varType, value, cScript);
                }

                if (name == "moveEntity") {
                    float x = stof(attributes.at("x"));
                    float y = stof(attributes.at("y"));
                    float z = stof(attributes.at("z"));

                    moveEntity(x, y, z);
                }

                if (name == "removeEntity")
                    removeEntity();

                if (name == "keyPress") {
                    string value = attributes.at("value");
                    int keyValue = cScript->getValidKeyPress(value);
                    
                    if (Input::getInstance().isKeyPressed(keyValue))
                        runCommands(command->FirstChild(), cScript);
                }

                if (name == "changeSprite") {
                    string filepath = attributes.at("name");

                    changeSprite(filepath);
                }

                 if (name == "callFunction") {
                    string functionName = attributes.at("name");

                    if (!currEntity->has_component<CustomScript>()) {
                        command = command->NextSibling();
                        continue;
                    }

                    ComponentHandle<CustomScript> handle = currEntity->component<CustomScript>();
                    XMLElement* customFunction = handle->getCustomFunction(functionName);

                    if (customFunction != nullptr)
                        runCommands(customFunction->FirstChild(), handle);
                }

                if (name == "onEntity") {
                    string entityName = attributes.at("name");
                    Entity* temp = currEntity;

                    if (cScript.get()->containsVariable(entityName)) {
                        currEntity = &cScript.get()->entities.at(entityName);
                        runCommands(command->FirstChild(), cScript);
                    } else {
                        cout << endl << "onEntity: entity " << entityName << " not found" << endl;
                    }

                    currEntity = temp;
                }

                if (name == "setActive") {
                    string value = attributes.at("value");
                    
                    setActive(value == "true" ? true : false);
                }

                if (name == "matchEntityPos") {
                    string value = attributes.at("name");

                    matchEntityPos(value, cScript);
                }

                command = command->NextSibling();
            }
        }

        // CustomScript Functions
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
            // Flag Current entity for deletion
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

        // *********************************
        // Variable functions
        // *********************************
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
}; 