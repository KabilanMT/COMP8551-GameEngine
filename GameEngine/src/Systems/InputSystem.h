#pragma once

#include "entityx/entityx.h"
#include <glad/glad.h>
#include <glfw/glfw3.h>
#include "../engine.h"
#include "Input.h"
#include "../logger.h"
#include<unordered_set>
#include <vector>

using namespace entityx;
/*
Input system is updating key press, mouse button press, mouse position, scroll offset 
and state of cursor enter the window.
*/
class InputSystem : public System<InputSystem> {

public:

    /*
     update input of key, mouse button, mouse position, scroll offset, cursor enter window
     parameter: EntityManager&, EventManager&, TimeDelta    
    */
    void update(EntityManager& es, EventManager& events, TimeDelta dt)
    {
        
        if (!hasWindow) {
            window = Engine::getInstance().window;

            glfwSetKeyCallback(window, [](GLFWwindow* window, int key, int scancode, int action, int mods)
                {
                    auto state = glfwGetKey(window, key);
                    if (state == GLFW_PRESS || state == GLFW_REPEAT) {
                       Input::getInstance().pressedChar.push_back(key);
                    }

                    Input::getInstance().pressKey(key);
                });

            glfwSetCursorPosCallback(window, [](GLFWwindow* window, double xpos, double ypos) {
                Input::getInstance().cursorPositionCallback(xpos, ypos);
                });

            glfwSetCursorEnterCallback(window, [](GLFWwindow* window, int entered) {
                Input::getInstance().cursorEnterCallback(entered);
                });

            glfwSetMouseButtonCallback(window, [](GLFWwindow* window, int button, int action, int mods) {
                auto state = glfwGetMouseButton(window, button);
                if (state == GLFW_PRESS || state == GLFW_REPEAT) {
                    Input::getInstance().pressedChar.push_back(button);
                }
                Input::getInstance().pressMouseButton(button);
                });

            glfwSetScrollCallback(window, [](GLFWwindow* window, double xoffset, double yoffset) {
                Input::getInstance().scrollCallback(xoffset, yoffset);
                
                });

            hasWindow = true;
           
        }
        Input::getInstance().pressedChar.clear();

        double xpos, ypos;
        glfwGetCursorPos(window, &xpos, &ypos);

        //Poll for and process events
        glfwPollEvents();
    }

    GLFWwindow* window;
    bool hasWindow = false; 
};