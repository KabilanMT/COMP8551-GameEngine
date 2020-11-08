#pragma once

#include "entityx/entityx.h"

using namespace std;

struct MoveTo : entityx::Event<MoveTo> {
  MoveTo(entityx::Entity& e, float x, float y, float z) : e(e), x(x), y(y), z(z) {}

  entityx::Entity& e;
  float x, y, z;
};