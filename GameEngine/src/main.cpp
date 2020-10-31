#include "engine.h"
#include "SceneManager.h"

typedef void (*f_update)(); 

int main()
{
    //create scene
    SceneManager::getInstance().addScene("first scene", "maps/test.tmx");
    SceneManager::getInstance().addScene("second scene", "maps/Scene1.tmx");
    SceneManager::getInstance().addScene("third scene", "maps/Scene2.tmx");
    //Game loop
    //Engine start
    Engine::getInstance().start();

    return 0;
}
