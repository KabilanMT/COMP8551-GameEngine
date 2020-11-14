#pragma once


#include <string>
#include <vector>
#include "../logger.h"
#include <cstring>
#include <fstream>
#include <iostream>
#include <functional>

#include "../Components/Components.h"
#include "tinyxml2.h"

using namespace tinyxml2;

struct CustomScript
{
    public:
        CustomScript(string xmlFile) {
            doc = new TinyXMLDocument();
            XMLError xmlerr = doc->LoadFile(xmlFile.c_str());

            if (xmlerr != XML_SUCCESS)
            {
                Logger::getInstance() << "Could not read game object XML file!\n";
                return;
            }

            root = doc->FirstChildElement("root");

            // Reserved variables
            strings.insert(make_pair("collisionObject-tag", ""));
            strings.insert(make_pair("collisionObject-name", ""));
        }

        XMLElement* getStart() {
            return root->FirstChildElement("start");
        }

        XMLElement* getUpdate() {
            return root->FirstChildElement("update");
        }

        XMLElement* getVariables() {
            return root->FirstChildElement("variables");
        }

        XMLElement* getOnEvent() {
            return root->FirstChildElement("onEvent");
        }

        XMLElement* getOnCollision() {
            return root->FirstChildElement("onCollision");
        }

        XMLElement* getCustomFunction(std::string functionName) {
            return root->FirstChildElement(functionName.c_str());
        }

        int getValidKeyPress(string key) {
            return possibleInputs.at(key);
        }

        void resetReservedVariables() {
            strings.at("collisionObject-tag") = "";
            strings.at("collisionObject-name") = "";
        }

        bool containsVariable(string varName) {
            if (ints.find(varName) != ints.end() || floats.find(varName) != floats.end()
                || doubles.find(varName) != doubles.end() || strings.find(varName) != strings.end()
                || bools.find(varName) != bools.end() || entities.find(varName) != entities.end())
                return true; 
            
            return false;
        }

        ~CustomScript() {
            delete doc;
        }

        // XML Variables
        TinyXMLDocument* doc;
        XMLElement* root;
        string xml;

        // User Generated Variables <varible name, variable value>
        unordered_map<std::string, int> ints;
        unordered_map<std::string, float> floats;
        unordered_map<std::string, double> doubles;
        unordered_map<std::string, std::string> strings;
        unordered_map<std::string, bool> bools;
        unordered_map<std::string, entityx::Entity> entities;

    private:
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
};
