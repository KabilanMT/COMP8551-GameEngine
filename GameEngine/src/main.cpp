#include "engine.h"
#include "SceneManager.h"

typedef void (*f_update)(); 

int main()
{
    //create scene
    //Scene 2 is first for testing, scene 1 should be first
    SceneManager::getInstance().addScene("Scene 2", "../TempleAtonement/src/maps/map02.tmx");
    SceneManager::getInstance().addScene("Scene 1", "../TempleAtonement/src/maps/map01.tmx");
    SceneManager::getInstance().addScene("Scene 3", "../TempleAtonement/src/maps/map03.tmx");
    SceneManager::getInstance().addScene("Scene 4", "../TempleAtonement/src/maps/map04.tmx");
    SceneManager::getInstance().addScene("Scene 5", "../TempleAtonement/src/maps/map05.tmx");
    SceneManager::getInstance().addScene("Scene 6", "../TempleAtonement/src/maps/map06.tmx");
    SceneManager::getInstance().addScene("Scene 7", "../TempleAtonement/src/maps/map07.tmx");
    SceneManager::getInstance().addScene("Scene 8", "../TempleAtonement/src/maps/map08.tmx");
    SceneManager::getInstance().addScene("Scene 9", "../TempleAtonement/src/maps/map09.tmx");
    SceneManager::getInstance().addScene("Scene 10", "../TempleAtonement/src/maps/map10.tmx");
    SceneManager::getInstance().addScene("Scene 11", "../TempleAtonement/src/maps/map11.tmx");
    SceneManager::getInstance().addScene("Scene 12", "../TempleAtonement/src/maps/map12.tmx");
    SceneManager::getInstance().addScene("Scene 13", "../TempleAtonement/src/maps/map13.tmx");

    //Game loop
    //Engine start
    Engine::getInstance().start();

    return 0;
}
