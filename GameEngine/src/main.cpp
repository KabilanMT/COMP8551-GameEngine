#include "engine.h"
#include "SceneManager.h"

typedef void (*f_update)(); 

int main()
{
    //create scene
    SceneManager::getInstance().addScene("first scene", "maps/map8.tmx");
    SceneManager::getInstance().addScene("second scene", "maps/Scene2.tmx");
    SceneManager::getInstance().addScene("third scene", "maps/Scene2.tmx");
    // SceneManager::getInstance().addScene("first scene", "maps/test.tmx");
    // SceneManager::getInstance().addScene("second scene", "maps/test.tmx");
    // SceneManager::getInstance().addScene("third scene", "maps/test.tmx");
    //Game loop
    //Engine start
    Engine::getInstance().start();

    return 0;
}
