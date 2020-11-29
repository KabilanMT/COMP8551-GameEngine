#include "SceneManager.h"
#include "logger.h"
#include "engine.h"
#include "Events/Events.h"

#include "Components/CustomScript.h"

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
            vector<Entity> persistentEntities;
            if (isSceneLoaded) {
                Engine::getInstance().events.emit<SceneUnload>(sceneLoaded);
                // Engine::getInstance().entities.reset(); //This will clear all entities, we need to clear all except persistant ones
                for (Entity entity : Engine::getInstance().entities.entities_for_debugging()) {
                    if (!entity.has_component<Persistent>()) {
                        entity.destroy();
                    } else {
                        persistentEntities.push_back(entity);
                    }
                }
            }

            Engine::getInstance().events.emit<ScenePreLoad>(scene->getName());
            scene->load(persistentEntities);
            Logger::getInstance() << "Loaded scene \"" << scene->getName() << "\"\n";
            Engine::getInstance().events.emit<SceneLoad>(scene->getName(), Engine::getInstance().entities.entities_with_components<CustomScript>());
            isSceneLoaded = true;
            return;
        }
    }

    Logger::getInstance() << "SceneManager::loadScene failed: Scene " << sceneName << " not found.\n";
}

void SceneManager::addScene(string sceneName, string tmxFile) {
    scenes.push_back(new Scene(sceneName, tmxFile));
}