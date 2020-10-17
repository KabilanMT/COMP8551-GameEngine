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
            ComponentHandle<CustomScript> handle = e.component<CustomScript>();
            XMLElement* updateContent = handle->getUpdate();
            runCommands(updateContent->FirstChild(), handle);
        }
    }

   private:
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