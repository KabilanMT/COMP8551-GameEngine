#pragma once
#include <vector>
#include <string>
#include <functional>

/*

Input class is used for the input of game. You can define your own input function by using key,
mouse button press/repeat, mouse position and mouse scroll offset position

Example of how to check if a key is pressed:
- bool isSpacePressed = Input::getInstance().isKeyPressed(GLFW_KEY_SPACE)

Full list of key_press codes are here
https://www.glfw.org/docs/3.3/group__keys.html

Example of how to check if a mouse button was pressed
- bool isLeftMousePressed = Input::getInstance().isMousePressed(true) //left mouse button

Example to check the position of the cursor
- int xpos = Input::getInstance().getCursorPosition(true) //xcoord
- int ypos = Input::getInstance().getCursorPosition(false) //ycoord
*/
using namespace std;
class Input {
public:
    Input(Input const&) = delete;
    void operator=(Input const&) = delete;

    /*
      get singleton instance of class Input
    */
    static Input&  getInstance() {
        static Input instance;
        return instance;
    }

    /* check if the key is pressed
        parameter: integer for key input
        return bool
    */
    inline bool isKeyPressed(int key) {
        for (int i = 0; i < pressedChar.size(); ++i) {
            if (pressedChar.at(i) == key) {
                return true;
            }
        }
        return false;
    }

    inline bool isMousePressed(bool isLeft) {
        if (isLeft) {
            return leftMousePressed;
        } else {
            return rightMousePressed;
        }
    }

    inline int getCursorPosition(bool isX) {
        if (isX) {
            return mousePosX;
        } else {
            return mousePosY;
        }
    }

    inline void pressKey(int key) {
        pressedChar.push_back(key);
    }

    inline void releaseKey(int key) {
        for (auto i = pressedChar.begin(); i != pressedChar.end(); ++i) {
            if (*i == key) {
                pressedChar.erase(i);
                return;
            }
        }
    }

    inline void pressMouseButton(int button) {
        if (button == GLFW_MOUSE_BUTTON_RIGHT) {
            rightMousePressed = true;
        }

        if (button == GLFW_MOUSE_BUTTON_LEFT) {
            leftMousePressed = true;
        }
    }

    inline void cursorPositionCallback(double xpos, double ypos) {
        mousePosX = xpos;
        mousePosY = ypos;
    }

    inline void cursorEnterCallback(int entered) {
        //std::cout << "Entered window" << std::endl;
    }

    inline void scrollCallback(double xoffset, double yoffset) {
        //std::cout << "scroll x offset: " << xoffset << " yoffset: " << yoffset << std::endl;
    }

    inline void clear() {
        leftMousePressed = false;
        rightMousePressed = false;
    }

private:
    Input() {}

    vector<int> pressedChar;
    bool leftMousePressed = false;
    bool rightMousePressed = false;
    int mousePosX = 0;
    int mousePosY = 0;

};