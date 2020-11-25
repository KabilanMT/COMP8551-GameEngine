#pragma once
#include <gtest/gtest.h>
#include <entityx/entityx.h>
#include <vector>

#include "../Components/Components.h"
#include "../Systems/Input.h"
#include "../engine.h"
#include <glfw/glfw3.h>

using namespace std;
using namespace entityx;

TEST (IsKeyPressed, NoKeyPressed) {
    EXPECT_FALSE(Input::getInstance().isKeyPressed(GLFW_KEY_A));
}

TEST (IsKeyPressed, DifferentKeyIsPressed) {
    Input::getInstance().pressKey(GLFW_KEY_A);
    EXPECT_FALSE(Input::getInstance().isKeyPressed(GLFW_KEY_W));
}

TEST (IsKeyPressed, KeyIsPressed) {
    Input::getInstance().pressKey(GLFW_KEY_A);
    EXPECT_TRUE(Input::getInstance().isKeyPressed(GLFW_KEY_A));
}

TEST (IsMousePressed, LeftMousePressed) {
    Input::getInstance().clear();
    Input::getInstance().pressMouseButton(GLFW_MOUSE_BUTTON_LEFT);
    EXPECT_TRUE(Input::getInstance().isMousePressed(true));
}

TEST (IsMousePressed, NotLeftMousePressed) {
    Input::getInstance().clear();
    Input::getInstance().pressMouseButton(GLFW_MOUSE_BUTTON_RIGHT);
    EXPECT_FALSE(Input::getInstance().isMousePressed(true));
}

TEST (IsMousePressed, RightMousePressed) {
    Input::getInstance().clear();
    Input::getInstance().pressMouseButton(GLFW_MOUSE_BUTTON_RIGHT);
    EXPECT_TRUE(Input::getInstance().isMousePressed(false));
}

TEST (IsMousePressed, NotRightMousePressed) {
    Input::getInstance().clear();
    Input::getInstance().pressMouseButton(GLFW_MOUSE_BUTTON_LEFT);
    EXPECT_FALSE(Input::getInstance().isMousePressed(false));
}

TEST (IsMousePressed, NoMousePressed) {
    Input::getInstance().clear();
    EXPECT_FALSE(Input::getInstance().isMousePressed(true));
    EXPECT_FALSE(Input::getInstance().isMousePressed(false));
}

TEST (GetCursorPosition, PosX) {
    Input::getInstance().cursorPositionCallback(1, 3);
    EXPECT_EQ(Input::getInstance().getCursorPosition(true), 1);    
}

TEST (GetCursorPosition, PosY) {
    Input::getInstance().cursorPositionCallback(1, 3);
    EXPECT_EQ(Input::getInstance().getCursorPosition(false), 3);    
}
