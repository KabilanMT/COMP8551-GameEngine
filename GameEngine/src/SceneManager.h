#pragma once

#include <string>
#include <vector>
#include "Scene.h"

using namespace std;
class SceneManager {
    public:
        static SceneManager& getInstance();
         //remove other constructors
        SceneManager(SceneManager const&) = delete;
        void operator=(SceneManager const&) = delete;

        void addScene(string sceneName, string tmxFile);
        void loadScene(string sceneName);
        void start();

    private:
        SceneManager();
        bool isSceneLoaded = false;
        string sceneLoaded;

        vector<Scene*> scenes;
};

inline SceneManager& SceneManager::getInstance() {
    static SceneManager instance;
    return instance;
}