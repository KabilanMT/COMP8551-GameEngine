#pragma once

#include <fstream>
#include <iostream>
#include <string>
#include <windows.h>
#include <functional>

#include "entityx/entityx.h"
#include "../Components/Components.h"
#include "../Events/Events.h"

using namespace entityx;

class CustomScriptSystem : public System<CustomScriptSystem>, public Receiver<CustomScriptSystem> {
public:
    void configure(EventManager& events) override {
        events.subscribe<SceneLoad>(*this);
    }

    void receive(const SceneLoad& sl) {
        //this method will be called when the scene loads

        //call start methods
    }

    void update(EntityManager& es, EventManager& events, TimeDelta dt) override 
    {
        auto entities = es.entities_with_components<CustomScript>();

        for (Entity e : entities) {
            currEntity = &e;
            ComponentHandle<CustomScript> handle = e.component<CustomScript>();
            XMLElement* updateContent = handle->getUpdate();
            runCommands(updateContent->FirstChild(), handle);
        }
    }

   private:
        Entity* currEntity;

        // Should be ran in start
        // TODO: Requires error checking and refactor, assumes that the first and second attrib is variable name and value
        void getVariables(XMLNode* variable, ComponentHandle<CustomScript> cScript) {
            while (variable != NULL) {
                
                string name = variable->Value();
                const XMLAttribute* attr = variable->ToElement()->FirstAttribute();
                string var_name = attr->Value();
                string var_value = attr->Next()->Value(); 

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

                variable = variable->NextSibling();
            }
        }

        void runCommands(XMLNode* command, ComponentHandle<CustomScript> cScript) {
            while (command != NULL) {
                
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
                if (name == "moveEntity") {
                    float x = stof(attributes.at("x"));
                    float y = stof(attributes.at("y"));

                    moveEntity(x, y);
                }

                if (name == "removeEntity") {
                    removeEntity();
                }

                command = command->NextSibling();
            }
        }

        // CustomScript Functions
        // Should update as there should be more than just 4 vertices
        void moveEntity(float x, float y) {
            ComponentHandle<Position> position = currEntity->component<Position>(); 
            position.get()->v0x += x;
            position.get()->v0y += y;

            position.get()->v1x += x;
            position.get()->v1y += y;

            position.get()->v2x += x;
            position.get()->v2y += y;

            position.get()->v3x += x;
            position.get()->v3y += y;
        }

        void removeEntity() {
            // Flag Current entity for deletion
        }

        void setActiveObject(bool active) {
            
        }

        void loadScene() {

        }

        void updateVar(string varName, string varType, ComponentHandle<CustomScript> cScript) {
            if (varType == "int")
                cScript.get()->ints.at(varName);

            if (varType == "float")
                cScript.get()->floats.at(varName);

            if (varType == "double")
                cScript.get()->doubles.at(varName);
            
            if (varType == "string")
                cScript.get()->strings.at(varName);

            if (varType == "bool") 
                cScript.get()->bools.at(varName);
        }

        void addVar(string varName, string varType, string value) {
            
        }
}; 