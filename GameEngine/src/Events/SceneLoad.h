#pragma once

#include "../Components/CustomScript.h"
#include <entityx/entityx.h>
#include <string>

using namespace std;

struct SceneLoad : Event<SceneLoad> {
  SceneLoad(string sceneName, entityx::EntityManager::View<CustomScript> entities) : sceneName(sceneName), entities(entities) {}

  string sceneName;
  entityx::EntityManager::View<CustomScript> entities;
};