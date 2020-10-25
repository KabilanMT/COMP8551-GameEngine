#pragma once

#include <fstream>
#include <iostream>
#include <string>
#include <functional>

#include "entityx/entityx.h"
#include "../Components/CustomScript.h"
#include "../Events/Events.h"

using namespace entityx;

class CustomScriptSystem : public System<CustomScriptSystem>, public Receiver<CustomScriptSystem> {
public:
    void configure(EventManager& events) override {
        events.subscribe<SceneLoad>(*this);
    }

    void receive(const SceneLoad& sl) {
        //this method will be called when the scene loads

        // call start methods
        for (Entity e : sl.entities) {
            currEntity = &e;
            ComponentHandle<CustomScript> handle = e.component<CustomScript>();

            XMLElement* variablesContent = handle->getVariables();
            if (variablesContent != nullptr)
                getVariables(variablesContent->FirstChild(), handle);

            XMLElement* startContent = handle->getStart();
            if (startContent != nullptr)
                runCommands(startContent->FirstChild(), handle);
        }
    }

    void update(EntityManager& es, EventManager& events, TimeDelta dt) override 
    {
        auto entities = es.entities_with_components<CustomScript>();

        for (Entity e : entities) {
            currEntity = &e;
            ComponentHandle<CustomScript> handle = e.component<CustomScript>();
            XMLElement* updateContent = handle->getUpdate();

            if (updateContent != nullptr)
                runCommands(updateContent->FirstChild(), handle);
        }
    }

   private:
        Entity* currEntity;

        // Should be ran in start
        // TODO: Requires error checking and refactor, assumes that the first and second attrib is variable name and value
        void getVariables(XMLNode* variable, ComponentHandle<CustomScript> cScript) {
            while (variable != nullptr) {
                
                string name = variable->Value();
                const XMLAttribute* attr = variable->ToElement()->FirstAttribute();
                string var_name = attr->Value();
                string var_value = attr->Next()->Value(); 

                if (name.compare("int") == 0)
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
                        double valToCompare = stod(attributes.at("value"));

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

                        if (val)
                            runCommands(command->FirstChild(), cScript);   
                    }

                }

                if (name == "updateVar") {
                    cout << "Test" << endl;
                    string varName = attributes.at("name");
                    string varType = attributes.at("type");
                    string value = attributes.at("value");
                    cout << "Test" << endl;

                    updateVar(varName, varType, value, cScript);
                }

                if (name == "addVar") {
                    string varName = attributes.at("name");
                    string varType = attributes.at("type");
                    string value = attributes.at("value");

                    addVar(varName, varType, value, cScript);
                }

                if (name == "moveEntity") {
                    int x = stoi(attributes.at("x"), nullptr, 0);
                    int y = stoi(attributes.at("y"), nullptr, 0);
                    int z = stoi(attributes.at("z"), nullptr, 0);

                    moveEntity(x, y, z);
                }

                if (name == "removeEntity") {
                    removeEntity();
                }

                command = command->NextSibling();
            }
        }

        // CustomScript Functions
        // Should update as there should be more than just 4 vertices
        void moveEntity(int x, int y, int z) {
            // ComponentHandle<Translation> trans = currEntity->component<Translation>(); 
            // trans.get()->x += x;
            // trans.get()->y += y;
            // trans.get()->z += z;
        }

        void removeEntity() {
            // Flag Current entity for deletion
        }

        void setActiveObject(bool active) {
            
        }

        void loadScene() {

        }

        void updateVar(string varName, string varType, string value, ComponentHandle<CustomScript> cScript) {
            if (varType == "int")
                cScript.get()->ints.at(varName) = stoi(value, nullptr, 0);

            if (varType == "float")
                cScript.get()->floats.at(varName) = stof(value);

            if (varType == "double")
                cScript.get()->doubles.at(varName) = stod(value);
            
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

            if (varType == "double")
                cScript.get()->doubles.at(varName) += stod(value);

            if (varType == "string")
                cScript.get()->strings.at(varName) += value;
        }
}; 