#include "engine.h"
#include "SceneManager.h"

typedef void (*f_update)(); 

int main()
{
    //create scene
    SceneManager::getInstance().addScene("first scene", "maps/map8.tmx");
    SceneManager::getInstance().addScene("second scene", "maps/Scene3.tmx");
    SceneManager::getInstance().addScene("third scene", "maps/Scene3.tmx");
    SceneManager::getInstance().addScene("third scene", "maps/map8.tmx");
    SceneManager::getInstance().addScene("third scene", "maps/map12.tmx");
    SceneManager::getInstance().addScene("third scene", "maps/map13.tmx");

    //Game loop
    //Engine start
    Engine::getInstance().start();

    return 0;
}
