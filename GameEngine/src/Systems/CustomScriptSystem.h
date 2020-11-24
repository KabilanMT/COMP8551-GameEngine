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
#include "CScriptFunctions.h"

using namespace entityx;

class CustomScriptSystem : public System<CustomScriptSystem>, public Receiver<CustomScriptSystem> {
public:
    void configure(EventManager& events) override {
        events.subscribe<SceneLoad>(*this);
        events.subscribe<Trigger>(*this);
    }

    void receive(const SceneLoad& sl) {
        // init reserved global variables
        CScript::initVariables();

        // call start methods
        for (Entity e : sl.entities) {
            CScript::setCurrEntity(&e);

            if (!e.has_component<CustomScript>())
                continue;

            ComponentHandle<CustomScript> handle = e.component<CustomScript>();

            // Gets variables specified in the entity's custom script 
            XMLElement* variablesContent = handle->getVariables();
            if (variablesContent != nullptr)
                getVariables(variablesContent->FirstChild(), handle);

            // Runs commands in the start tag in the entity's custom script
            if (handle->hasStarted)
                continue;

            XMLElement* startContent = handle->getStart();
            if (startContent != nullptr)
                runCommands(startContent->FirstChild(), handle);
            handle->hasStarted = true;
        }
    }

    void receive(const Trigger& tr) {
        if (tr.gotTriggered->valid() && tr.gotTriggered->has_component<CustomScript>()) {
            CScript::setCurrEntity(tr.gotTriggered);

            ComponentHandle<CustomScript> handle = tr.gotTriggered->component<CustomScript>();

            if (tr.triggeringEntity->has_component<Tag>()) {
                handle->strings.at("collisionObject-tag") = tr.triggeringEntity->component<Tag>().get()->getTag();
            } else {
                handle->strings.at("collisionObject-tag") = "";
            }
            
            if (tr.triggeringEntity->has_component<Name>()) {
                handle->strings.at("collisionObject-name") = tr.triggeringEntity->component<Name>().get()->getName();
            } else {
                handle->strings.at("collisionObject-name") = "";
            }

            XMLElement* collisionContent = handle->getOnCollision();
            if (collisionContent != nullptr)
                runCommands(collisionContent->FirstChild(), handle);
            if (handle.valid()) {
                handle->resetReservedVariables();
            }
        }
    }

    void update(EntityManager& es, EventManager& events, TimeDelta dt) override 
    {
        auto entities = es.entities_with_components<CustomScript>();

        for (Entity e : entities) {
            CScript::setCurrEntity(&e);

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
                CScript::doubles.at("deltaTime") = dt;
                runCommands(updateContent->FirstChild(), handle);
            }
        }
    }

   private:
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

                if (CScript::containsGlobalVariable(var_name)) {
                    Logger::getInstance() << var_name << " is already defined or is a reserved global variable. \n";
                    variable = variable->NextSibling();
                    continue;
                }

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
                    bool entityFound = false;

                    for (Entity e : entities) {
                        ComponentHandle<Name> entityName = e.component<Name>();
                        if (entityName.get()->getName().compare(var_value) == 0) {
                            cScript.get()->entities.insert(make_pair(var_name, e));
                            entityFound = true;
                            break;
                        }
                    }

                    if (!entityFound)
                        cScript.get()->entities[var_name];
                }

                if (name == "globalInt")
                    CScript::ints.insert(make_pair(var_name, stoi(var_value, nullptr, 0)));

                if (name == "globalFloat")
                    CScript::floats.insert(make_pair(var_name, stof(var_value)));

                if (name == "globalDouble")
                    CScript::doubles.insert(make_pair(var_name, stod(var_value)));
                
                if (name == "globalString")
                    CScript::strings.insert(make_pair(var_name, var_value));

                if (name == "globalBool") 
                    CScript::bools.insert(make_pair(var_name, (var_value == "true") ? true : false));

                if (name == "globalEntity") {
                    auto entities = Engine::getInstance().entities.entities_with_components<Name>();
                    bool entityFound = false;

                    for (Entity e : entities) {
                        ComponentHandle<Name> entityName = e.component<Name>();
                        if (entityName.get()->getName().compare(var_value) == 0) {
                            CScript::entities.insert(make_pair(var_name, e));
                            entityFound = true;
                            break;
                        }
                    }

                    if (!entityFound)
                        CScript::entities[var_name];
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

                    if (!cScript.valid()) {
                        break;
                    }

                    if (!cScript->containsVariable(attributes.at("name")))
                        return;

                    if (type == "int") {
                        int val = cScript.get()->ints.at(attributes.at("name"));
                        int valToCompare = stoi(attributes.at("value"), nullptr, 0);

                        if (val == valToCompare)
                            runCommands(command->FirstChild(), cScript);
                    }

                    if (type == "float") {
                        float val = cScript.get()->floats.at(attributes.at("name"));
                        float valToCompare = stof(attributes.at("value"));

                        if (val == valToCompare)
                            runCommands(command->FirstChild(), cScript);
                    }

                    if (type == "double") {
                        double val = cScript.get()->doubles.at(attributes.at("name"));
                        double valToCompare = 0;

                        if (attributes.at("value") == "deltaTime") {
                            valToCompare = CScript::doubles.at("deltaTime");
                        } else {
                            valToCompare = stod(attributes.at("value"));
                        }

                        if (val == valToCompare)
                            runCommands(command->FirstChild(), cScript);
                    }
                    
                    if (type == "string") {
                        string val = cScript.get()->strings.at(attributes.at("name"));
                        string valToCompare = attributes.at("value");

                        if (val == "collisionObject-name" || val == "collisionObject-tag") {
                            val = cScript->strings.at(val);
                        }
                        if (valToCompare == "collisionObject-name" || valToCompare == "collisionObject-tag") {
                            valToCompare = cScript->strings.at(valToCompare);
                        }

                        if (val.compare(valToCompare) == 0)
                            runCommands(command->FirstChild(), cScript);
                    }

                    if (type == "bool") { 
                        bool val = cScript.get()->bools.at(attributes.at("name"));

                        if (val == (attributes.at("value") == "true" ? true : false))
                            runCommands(command->FirstChild(), cScript);   
                    }
                }

                if (name == "ifGlobalVar" && attributes.find("name") != attributes.end() && attributes.find("value") != attributes.end()) {
                    string type = attributes.at("type");

                    if (!cScript.valid()) {
                        break;
                    }

                    if (!CScript::containsGlobalVariable(attributes.at("name")))
                        return;
                    
                    if (type == "int") {
                        int val = CScript::ints.at(attributes.at("name"));
                        int valToCompare = stoi(attributes.at("value"), nullptr, 0);

                        if (val == valToCompare)
                            runCommands(command->FirstChild(), cScript);
                    }

                    if (type == "float") {
                        float val = CScript::floats.at(attributes.at("name"));
                        float valToCompare = stof(attributes.at("value"));

                        if (val == valToCompare)
                            runCommands(command->FirstChild(), cScript);
                    }

                    if (type == "double") {
                        double val = CScript::doubles.at(attributes.at("name"));
                        double valToCompare = 0;

                        if (attributes.at("value") == "deltaTime") {
                            valToCompare = CScript::doubles.at("deltaTime");
                        } else {
                            valToCompare = stod(attributes.at("value"));
                        }

                        if (val == valToCompare)
                            runCommands(command->FirstChild(), cScript);
                    }
                    
                    if (type == "string") {
                        string val = CScript::strings.at(attributes.at("name"));
                        string valToCompare = attributes.at("value");

                        if (val.compare(valToCompare) == 0)
                            runCommands(command->FirstChild(), cScript);
                    }

                    if (type == "bool") { 
                        bool val = CScript::bools.at(attributes.at("name"));

                        if (val == (attributes.at("value") == "true" ? true : false))
                            runCommands(command->FirstChild(), cScript);   
                    }
                }

                if (name == "ifVarGreater" && attributes.find("name") != attributes.end() && attributes.find("value") != attributes.end()) {
                    string type = attributes.at("type");

                    if (!cScript.valid()) {
                        break;
                    }

                    if (!cScript->containsVariable(attributes.at("name")))
                        return;

                    if (type == "int") {
                        int val = cScript.get()->ints.at(attributes.at("name"));
                        int valToCompare = stoi(attributes.at("value"), nullptr, 0);

                        if (val > valToCompare)
                            runCommands(command->FirstChild(), cScript);
                    }

                    if (type == "float") {
                        float val = cScript.get()->floats.at(attributes.at("name"));
                        float valToCompare = stof(attributes.at("value"));

                        if (val > valToCompare)
                            runCommands(command->FirstChild(), cScript);
                    }

                    if (type == "double") {
                        double val = cScript.get()->doubles.at(attributes.at("name"));
                        double valToCompare = 0;
                        if (attributes.at("value") == "deltaTime") {
                            valToCompare = CScript::doubles.at("deltaTime");
                        } else {
                            valToCompare = stod(attributes.at("value"));
                        }

                        if (val > valToCompare)
                            runCommands(command->FirstChild(), cScript);
                    }
                }

                if (name == "ifGlobalVarGreater" && attributes.find("name") != attributes.end() && attributes.find("value") != attributes.end()) {
                    string type = attributes.at("type");

                    if (!cScript.valid()) {
                        break;
                    }

                    if (!CScript::containsGlobalVariable(attributes.at("name")))
                        return;

                    if (type == "int") {
                        int val = CScript::ints.at(attributes.at("name"));
                        int valToCompare = stoi(attributes.at("value"), nullptr, 0);

                        if (val > valToCompare)
                            runCommands(command->FirstChild(), cScript);
                    }

                    if (type == "float") {
                        float val = CScript::floats.at(attributes.at("name"));
                        float valToCompare = stof(attributes.at("value"));

                        if (val > valToCompare)
                            runCommands(command->FirstChild(), cScript);
                    }

                    if (type == "double") {
                        double val = CScript::doubles.at(attributes.at("name"));
                        double valToCompare = 0;

                        if (attributes.at("value") == "deltaTime") {
                            valToCompare = CScript::doubles.at("deltaTime");
                        } else {
                            valToCompare = stod(attributes.at("value"));
                        }

                        if (val > valToCompare)
                            runCommands(command->FirstChild(), cScript);
                    }
                }

                if (name == "ifVarLess" && attributes.find("name") != attributes.end() && attributes.find("value") != attributes.end()) {
                    string type = attributes.at("type");

                    if (!cScript.valid()) {
                        break;
                    }

                    if (!cScript->containsVariable(attributes.at("name")))
                        return;

                    if (type == "int") {
                        int val = cScript.get()->ints.at(attributes.at("name"));
                        int valToCompare = stoi(attributes.at("value"), nullptr, 0);

                        if (val < valToCompare)
                            runCommands(command->FirstChild(), cScript);
                    }

                    if (type == "float") {
                        float val = cScript.get()->floats.at(attributes.at("name"));
                        float valToCompare = stof(attributes.at("value"));

                        if (val < valToCompare)
                            runCommands(command->FirstChild(), cScript);
                    }

                    if (type == "double") {
                        double val = cScript.get()->doubles.at(attributes.at("name"));
                        double valToCompare = 0;
                        if (attributes.at("value") == "deltaTime") {
                            valToCompare = CScript::doubles.at("deltaTime");
                        } else {
                            valToCompare = stod(attributes.at("value"));
                        }

                        if (val < valToCompare)
                            runCommands(command->FirstChild(), cScript);
                    }
                }

                if (name == "ifGlobalVarLess" && attributes.find("name") != attributes.end() && attributes.find("value") != attributes.end()) {
                    string type = attributes.at("type");

                    if (!cScript.valid()) {
                        break;
                    }

                    if (!CScript::containsGlobalVariable(attributes.at("name")))
                        return;

                    if (type == "int") {
                        int val = CScript::ints.at(attributes.at("name"));
                        int valToCompare = stoi(attributes.at("value"), nullptr, 0);

                        if (val < valToCompare)
                            runCommands(command->FirstChild(), cScript);
                    }

                    if (type == "float") {
                        float val = CScript::floats.at(attributes.at("name"));
                        float valToCompare = stof(attributes.at("value"));

                        if (val < valToCompare)
                            runCommands(command->FirstChild(), cScript);
                    }

                    if (type == "double") {
                        double val = CScript::doubles.at(attributes.at("name"));
                        double valToCompare = 0;

                        if (attributes.at("value") == "deltaTime") {
                            valToCompare = CScript::doubles.at("deltaTime");
                        } else {
                            valToCompare = stod(attributes.at("value"));
                        }

                        if (val < valToCompare)
                            runCommands(command->FirstChild(), cScript);
                    }
                }

                if (name == "callFunction") {
                    if (!cScript.valid()) {
                        break;
                    }
                    string functionName = attributes.at("name");

                    if (!CScript::getCurrEntity()->has_component<CustomScript>()) {
                        command = command->NextSibling();
                        continue;
                    }

                    ComponentHandle<CustomScript> handle = CScript::getCurrEntity()->component<CustomScript>();
                    XMLElement* customFunction = handle->getCustomFunction(functionName);

                    if (customFunction != nullptr)
                        runCommands(customFunction->FirstChild(), handle);
                }

                if (name == "onEntity") {
                    if (!cScript.valid()) {
                        break;
                    }

                    string entityName = attributes.at("name");
                    Entity* temp = CScript::getCurrEntity();

                    if (cScript.get()->containsVariable(entityName) && cScript.get()->entities.at(entityName).id().id() != 0) {

                        CScript::setCurrEntity(&cScript.get()->entities.at(entityName));
                        runCommands(command->FirstChild(), cScript);

                    } else if (CScript::containsGlobalVariable(entityName) && CScript::entities.at(entityName).id().id() != 0) {    

                        CScript::setCurrEntity(&CScript::entities.at(entityName));
                        runCommands(command->FirstChild(), cScript);
                    
                    } else {
                        Logger::getInstance() << "onEntity: entity " << entityName << " not found\n";
                    }

                    CScript::setCurrEntity(temp);
                }

                if (name == "keyPress") {
                    if (!cScript.valid()) {
                        break;
                    }
                    string value = attributes.at("value");
                    int keyValue = CScript::getValidKeyPress(value);
                    
                    if (Input::getInstance().isKeyPressed(keyValue))
                        runCommands(command->FirstChild(), cScript);
                }

                if (name == "changeSprite") 
                    CScript::changeSprite(attributes.at("name"));

                if (name == "updateVar")
                    CScript::updateVar(attributes.at("name"), attributes.at("type"), attributes.at("value"), cScript);

                if (name == "setEqual")
                    CScript::setEqual(attributes.at("name"), attributes.at("type"), attributes.at("value"), attributes.at("isGlobal"), cScript);

                if (name == "addVar") 
                    CScript::addVar(attributes.at("name"), attributes.at("type"), attributes.at("value"), cScript);

                if (name == "multiVar") 
                    CScript::multiVar(attributes.at("name"), attributes.at("type"), attributes.at("value"), cScript);
                if (name == "subVar") 
                    CScript::subVar(attributes.at("name"), attributes.at("type"), attributes.at("value"), cScript);

                if (name == "divideVar") 
                    CScript::divideVar(attributes.at("name"), attributes.at("type"), attributes.at("value"), cScript);

                if (name == "modVar") 
                    CScript::modVar(attributes.at("name"), attributes.at("type"), attributes.at("value"), cScript);

                if (name == "updateGlobalVar")
                    CScript::updateGlobalVar(attributes.at("name"), attributes.at("type"), attributes.at("value"), cScript);

                if (name == "setGlobalEqual")
                    CScript::setGlobalEqual(attributes.at("name"), attributes.at("type"), attributes.at("value"), attributes.at("isGlobal"), cScript);

                if (name == "addGlobalVar") 
                    CScript::addGlobalVar(attributes.at("name"), attributes.at("type"), attributes.at("value"), cScript);

                if (name == "subGlobalVar") 
                    CScript::subGlobalVar(attributes.at("name"), attributes.at("type"), attributes.at("value"), cScript);

                if (name == "multiGlobalVar") 
                    CScript::multiGlobalVar(attributes.at("name"), attributes.at("type"), attributes.at("value"), cScript);

                if (name == "divideGlobalVar") 
                    CScript::divideGlobalVar(attributes.at("name"), attributes.at("type"), attributes.at("value"), cScript);

                if (name == "modGlobalVar") 
                    CScript::modGlobalVar(attributes.at("name"), attributes.at("type"), attributes.at("value"), cScript);

                if (name == "moveEntity")
                    CScript::moveEntity(stof(attributes.at("x")), stof(attributes.at("y")), stof(attributes.at("z")), attributes.at("applyDt"), CScript::doubles.at("deltaTime"));

                if (name == "moveEntityByVars")
                    CScript::moveEntityByVars(attributes.at("x"), attributes.at("y"), attributes.at("z"), attributes.at("applyDt"), cScript);

                if (name == "distanceTo")
                    CScript::distanceTo(attributes.at("name"), attributes.at("var"), cScript);

                if (name == "vectorTo")
                    CScript::vectorTo(attributes.at("name"), attributes.at("x"), attributes.at("y"), cScript);

                if (name == "removeEntity") {
                    CScript::removeEntity();
                    break;
                }

                if (name == "setActive") 
                    CScript::setActive(attributes.at("value") == "true" ? true : false);

                if (name == "matchEntityPos") 
                    CScript::matchEntityPos(attributes.at("name"), cScript);

                if (name == "log")
                    CScript::log(attributes.at("value"));
                    
                if (name == "loadScene") {
                    CScript::loadScene(attributes.at("name"));
                    break;
                }
                
                if (name == "flipBool")
                    CScript::flipBool(attributes.at("name"), cScript);

                if (name == "playAudio")
                    CScript::playAudio();
                    
                if (name == "pauseAudio")
                    CScript::pauseAudio();
                    
                if (name == "stopAudio")
                    CScript::stopAudio();

                if (name == "setAudioSource")
                    CScript::setAudioSource(attributes.at("value"));

                if (name == "setPos")
                    CScript::setPosition(attributes.at("x"), attributes.at("y"), cScript);

                command = command->NextSibling();
            }
        }

};