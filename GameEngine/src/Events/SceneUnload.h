#pragma once

#include <entityx/entityx.h>
#include <string>

using namespace std;

struct SceneUnload : Event<SceneUnload> {
  SceneUnload(string sceneName) : sceneName(sceneName) {}

  string sceneName;
};