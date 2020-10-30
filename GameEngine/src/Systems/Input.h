#pragma once
#include <vector>


/*

Input class is used for the input of game. You can define your own input function by using key,
mouse button press/repeat, mouse position and mouse scroll offset position
*/
class Input {
public:
    vector<int> pressedChar;

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

   /*
     Define your own key pressed call back, this method is a demo of key press template
     parameter: integer for key input
   */
   inline void pressKey(int key) {
       if (isKeyPressed(key)) {
           char letter = static_cast<char> (key);
           std::cout << letter << " is pressed" << std::endl;
       }
   }

   /*
     define your own mouse button pressed call back. this method is a demo of
      mouse button press template
      parameter: int for mouse button
   */
   
   inline void pressMouseButton(int button) {
       if (button == GLFW_MOUSE_BUTTON_RIGHT) {
           std::cout << "Right button pressed" << std::endl;
       }

       if (button == GLFW_MOUSE_BUTTON_LEFT) {
           std::cout << "Left button presse" << std::endl;
       }
   }

   /*
     define your own cursor position call back. This method is the demo of cursor position template
     parameter: double for x position, double for y position  
   */
   
   inline void cursorPositionCallback(double xpos, double ypos) {
       std::cout << "x position: " << xpos << "y position: " << ypos << std::endl;
   }


   /*
     define your own cursor enter input call back. This is the template you can use
     parameter: int for cursor enter the window
   */
   
   inline void cursorEnterCallback(int entered) {
       std::cout << "Entered window" << std::endl;
   }

   /*
    define your own scroll call back. This is the template you can use
    parameter: double x offset, double y offset
   */
   inline void scrollCallback(double xoffset, double yoffset) {
       std::cout << "scroll x offset: " << xoffset << " yoffset: " << yoffset << std::endl;
   }

private:
    Input() {
        pressedChar = vector<int>(256);
    }

};