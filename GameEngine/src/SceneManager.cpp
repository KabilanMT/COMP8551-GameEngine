#include "SceneManager.h"
#include "logger.h"
#include "engine.h"
#include "Events/Events.h"

SceneManager::SceneManager() {

}

void SceneManager::start() {
    Scene* first = scenes.at(0);
    loadScene(first->getName());
}

void SceneManager::loadScene(string sceneName) {

    //find scene with name
    Scene* scene;
    for (int i = 0; i < scenes.size(); ++i) {
        scene = scenes.at(i);
        if (scene->getName() == sceneName) {
            if (isSceneLoaded) {
                Engine::getInstance().events.emit<SceneUnload>(sceneLoaded);
                auto es = Engine::getInstance().entities.entities_with_components<AudioSource>();
                Engine::getInstance().entities.reset();
            }

            Engine::getInstance().events.emit<ScenePreLoad>(scene->getName());
            scene->load();
            Logger::getInstance() << "Loaded scene \"" << scene->getName() << "\"\n";
            Engine::getInstance().events.emit<SceneLoad>(scene->getName());
            isSceneLoaded = true;
            return;
        }
    }

    Logger::getInstance() << "SceneManager::loadScene failed: Scene not found.\n";
}

void SceneManager::addScene(string sceneName, string tmxFile) {
    scenes.push_back(new Scene(sceneName, tmxFile));
}