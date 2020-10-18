#pragma once

#include <fstream>
#include <experimental/filesystem>
#include <iostream>
#include <string>
#include <windows.h>

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
            // ComponentHandle<CustomScript> handle = e.component<CustomScript>();
            // XMLElement* updateContent = handle->getUpdate();
            // runCommands(updateContent->FirstChild(), handle);
        }
    }

   private:
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
                //do commands

                string name = command->Value();
                Logger::getInstance() << name << "\n";
                const XMLAttribute* attr = command->ToElement()->FirstAttribute();
                vector<string> attribNames;
                vector<string> attribVals;

                while (attr != NULL) {
                    attribNames.push_back(attr->Name());
                    attribVals.push_back(attr->Value());
                    attr = attr->Next();
                }

                command = command->NextSibling();
            }
        }
}; 