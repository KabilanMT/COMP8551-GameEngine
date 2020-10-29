#pragma once

#include <entityx/entityx.h>
#include <string>

using namespace std;

struct SceneLoad : Event<SceneLoad> {
  SceneLoad(string sceneName) : sceneName(sceneName) {}

  string sceneName;
};