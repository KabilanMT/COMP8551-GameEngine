#include "engine.h"
#include "SceneManager.h"

typedef void (*f_update)(); 

int main()
{
    //create scene
    SceneManager::getInstance().addScene("first scene", "maps/test.tmx");
    //Game loop
    //Engine start
    Engine::getInstance().start();

    return 0;
}
