#pragma once

#include <entityx/entityx.h>
#include <string>

using namespace std;

struct ScenePreLoad : Event<ScenePreLoad> {
  ScenePreLoad(string sceneName) : sceneName(sceneName) {}

  string sceneName;
};