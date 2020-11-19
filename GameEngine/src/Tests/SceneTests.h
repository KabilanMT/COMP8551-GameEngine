#include <gtest/gtest.h>
#include <entityx/entityx.h>
#include <vector>

#include "../Components/Components.h"
#include "../Scene.h"
#include "../engine.h"

using namespace std;
using namespace entityx;

TEST (AddAudioSource, ValidArguments) {
    vector<string> parameters;
    parameters.push_back("../TempleAtonement/src/sounds/coin_01.mp3");
    parameters.push_back("0");
    parameters.push_back("0");
    parameters.push_back("Fx");

    Scene scene("Scene", "tmx");
    Entity e = Engine::getInstance().entities.create();
    scene.addAudioSource(parameters, e);

    EXPECT_TRUE(e.has_component<AudioSource>());

    ComponentHandle<AudioSource> audio = e.component<AudioSource>();
    
    EXPECT_STREQ(audio.get()->name.c_str(), "../TempleAtonement/src/sounds/coin_01.mp3");
    EXPECT_FALSE(audio.get()->playOnStart);
    EXPECT_FALSE(audio.get()->getIfLoop());
    EXPECT_STREQ(audio.get()->tag.c_str(), "Fx");
}

TEST (AddAudioSource, InvalidArguments) {
    vector<string> parameters;
    parameters.push_back("9");
    parameters.push_back("hello");
    parameters.push_back("world");
    parameters.push_back("42");

    Scene scene("Scene", "tmx");
    Entity e = Engine::getInstance().entities.create();
    scene.addAudioSource(parameters, e);

    EXPECT_TRUE(e.has_component<AudioSource>());

    ComponentHandle<AudioSource> audio = e.component<AudioSource>();
    
    EXPECT_TRUE(audio.get()->playOnStart);
    EXPECT_FALSE(audio.get()->getIfLoop());
}

TEST (AddAudioSource, MissingArguments) {
    vector<string> parameters;

    Scene scene("Scene", "tmx");
    Entity e = Engine::getInstance().entities.create();
    scene.addAudioSource(parameters, e);

    EXPECT_FALSE(e.has_component<AudioSource>());

    parameters.push_back("../TempleAtonement/src/sounds/coin_01.mp3");
    EXPECT_TRUE(e.has_component<AudioSource>());

    ComponentHandle<AudioSource> audio = e.component<AudioSource>();
    
    EXPECT_TRUE(audio.get()->playOnStart);
    EXPECT_FALSE(audio.get()->getIfLoop());
}

TEST (AddBoxCollider, ValidArguments) {
    vector<string> parameters;
    parameters.push_back("24");
    parameters.push_back("32");
    parameters.push_back("0");
    parameters.push_back("0");
    parameters.push_back("0");
    parameters.push_back("0");
    parameters.push_back("0");

    Scene scene("Scene", "tmx");
    Entity e = Engine::getInstance().entities.create();
    scene.addBoxCollider(parameters, e);

    EXPECT_TRUE(e.has_component<BoxCollider>());

    ComponentHandle<BoxCollider> boxCol = e.component<BoxCollider>();

    EXPECT_EQ(boxCol.get()->width, 24);
    EXPECT_EQ(boxCol.get()->height, 32);
    EXPECT_EQ(boxCol.get()->x, 0);
    EXPECT_EQ(boxCol.get()->y, 0);
    EXPECT_EQ(boxCol.get()->z, 0);
    EXPECT_FALSE(boxCol.get()->isTrigger);
    EXPECT_FALSE(boxCol.get()->render);
}

TEST (AddBoxCollider, InvalidArguments) {
    vector<string> parameters;
    parameters.push_back("hello");
    parameters.push_back("world");
    parameters.push_back("a");
    parameters.push_back("s");
    parameters.push_back("d");
    parameters.push_back("f");
    parameters.push_back("g");

    Scene scene("Scene", "tmx");
    Entity e = Engine::getInstance().entities.create();
    scene.addBoxCollider(parameters, e);

    EXPECT_TRUE(e.has_component<BoxCollider>());

    ComponentHandle<BoxCollider> boxCol = e.component<BoxCollider>();

    EXPECT_EQ(boxCol.get()->width, 1);
    EXPECT_EQ(boxCol.get()->height, 1);
    EXPECT_EQ(boxCol.get()->x, 0);
    EXPECT_EQ(boxCol.get()->y, 0);
    EXPECT_EQ(boxCol.get()->z, 0);
    EXPECT_FALSE(boxCol.get()->isTrigger);
    EXPECT_FALSE(boxCol.get()->render);
}

TEST (AddBoxCollider, MissingArguments) {
    vector<string> parameters;

    Scene scene("Scene", "tmx");
    Entity e = Engine::getInstance().entities.create();
    scene.addBoxCollider(parameters, e);

    EXPECT_TRUE(e.has_component<BoxCollider>());

    ComponentHandle<BoxCollider> boxCol = e.component<BoxCollider>();

    EXPECT_EQ(boxCol.get()->width, 1);
    EXPECT_EQ(boxCol.get()->height, 1);
    EXPECT_EQ(boxCol.get()->x, 0);
    EXPECT_EQ(boxCol.get()->y, 0);
    EXPECT_EQ(boxCol.get()->z, 0);
    EXPECT_FALSE(boxCol.get()->isTrigger);
    EXPECT_FALSE(boxCol.get()->render);
}

TEST (AddCamera, ValidArguments) {
    vector<string> parameters;
    parameters.push_back("-480");
    parameters.push_back("480");
    parameters.push_back("-270");
    parameters.push_back("270");
    parameters.push_back("-1000");
    parameters.push_back("1000");
    parameters.push_back("0");
    parameters.push_back("0");
    parameters.push_back("0");

    Scene scene("Scene", "tmx");
    Entity e = Engine::getInstance().entities.create();
    scene.addCamera(parameters, e);

    EXPECT_TRUE(e.has_component<Camera>());

    ComponentHandle<Camera> camera = e.component<Camera>();

    EXPECT_EQ(camera.get()->lf, -480);
    EXPECT_EQ(camera.get()->rf, 480);
    EXPECT_EQ(camera.get()->bf, -270);
    EXPECT_EQ(camera.get()->tf, 270);
    EXPECT_EQ(camera.get()->dnp, -1000);
    EXPECT_EQ(camera.get()->dfp, 1000);
    EXPECT_EQ(camera.get()->x, 0);
    EXPECT_EQ(camera.get()->y, 0);
    EXPECT_EQ(camera.get()->z, 0);
}

TEST (AddCamera, InvalidArguments) {
    vector<string> parameters;
    parameters.push_back("asd");
    parameters.push_back("fgh");
    parameters.push_back("jhk");
    parameters.push_back("l;");
    parameters.push_back("");
    parameters.push_back("wer");
    parameters.push_back("e");
    parameters.push_back("f");
    parameters.push_back("c");

    Scene scene("Scene", "tmx");
    Entity e = Engine::getInstance().entities.create();
    scene.addCamera(parameters, e);

    EXPECT_TRUE(e.has_component<Camera>());

    ComponentHandle<Camera> camera = e.component<Camera>();

    EXPECT_EQ(camera.get()->lf, 0);
    EXPECT_EQ(camera.get()->rf, -960);
    EXPECT_EQ(camera.get()->bf, 0);
    EXPECT_EQ(camera.get()->tf, 540);
    EXPECT_EQ(camera.get()->dnp, -1);
    EXPECT_EQ(camera.get()->dfp, 1);
    EXPECT_EQ(camera.get()->x, 0);
    EXPECT_EQ(camera.get()->y, 0);
    EXPECT_EQ(camera.get()->z, 0);
}

TEST (AddCamera, MissingArguments) {
    vector<string> parameters;

    Scene scene("Scene", "tmx");
    Entity e = Engine::getInstance().entities.create();
    scene.addCamera(parameters, e);

    EXPECT_TRUE(e.has_component<Camera>());

    ComponentHandle<Camera> camera = e.component<Camera>();

    EXPECT_EQ(camera.get()->lf, 0);
    EXPECT_EQ(camera.get()->rf, -960);
    EXPECT_EQ(camera.get()->bf, 0);
    EXPECT_EQ(camera.get()->tf, 540);
    EXPECT_EQ(camera.get()->dnp, -1);
    EXPECT_EQ(camera.get()->dfp, 1);
    EXPECT_EQ(camera.get()->x, 0);
    EXPECT_EQ(camera.get()->y, 0);
    EXPECT_EQ(camera.get()->z, 0);
}
