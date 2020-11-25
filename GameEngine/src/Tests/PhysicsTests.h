#pragma once
#include <gtest/gtest.h>

#include "../Systems/PhysicsFunctions.h"
#include "../engine.h"

using namespace Physics;

TEST (AABBTests, IsFirstInsideSecond){
    EXPECT_TRUE(DetectAABB(0.0f,0.0f,1.0f,1.0f,0.0f,0.0f,2.0f,2.0f));
}