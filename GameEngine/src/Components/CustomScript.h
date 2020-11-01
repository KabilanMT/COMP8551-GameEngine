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

        int getValidKeyPress(string key) {
            if (key == "KEY_W")
                return GLFW_KEY_W;

            if (key == "KEY_S")
                return GLFW_KEY_S;

            if (key == "KEY_A")
                return GLFW_KEY_A;

            if (key == "KEY_D")
                return GLFW_KEY_D;
            
            if (key == "KEY_E")
                return GLFW_KEY_E;

            if (key == "KEY_SPACE")
                return GLFW_KEY_SPACE;

            return 0;
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

        // Reserved Variables
        
};
