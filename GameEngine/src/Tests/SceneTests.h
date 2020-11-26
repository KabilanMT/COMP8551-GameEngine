#pragma once
#include <gtest/gtest.h>
#include <entityx/entityx.h>
#include <vector>

#include "../Components/Components.h"
#include "../Scene.h"
#include "../engine.h"

using namespace std;
using namespace entityx;

TEST (AddAudioSource, ValidParameters) {
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

TEST (AddAudioSource, InvalidParameters) {
    vector<string> parameters;
    parameters.push_back("9");
    parameters.push_back("hello");
    parameters.push_back("world");
    parameters.push_back("42");

    Scene scene("Scene", "tmx");
    Entity e = Engine::getInstance().entities.create();
    scene.addAudioSource(parameters, e);

    EXPECT_FALSE(e.has_component<AudioSource>());
}

TEST (AddAudioSource, MissingParameters) {
    vector<string> parameters;

    Scene scene("Scene", "tmx");
    Entity e = Engine::getInstance().entities.create();
    scene.addAudioSource(parameters, e);

    EXPECT_FALSE(e.has_component<AudioSource>());

    parameters.push_back("../TempleAtonement/src/sounds/coin_01.mp3");
    scene.addAudioSource(parameters, e);
    EXPECT_TRUE(e.has_component<AudioSource>());

    ComponentHandle<AudioSource> audio = e.component<AudioSource>();
    
    EXPECT_TRUE(audio.get()->playOnStart);
    EXPECT_FALSE(audio.get()->getIfLoop());
}

TEST (AddBoxCollider, ValidParameters) {
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

TEST (AddBoxCollider, InvalidParameters) {
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

    EXPECT_FALSE(e.has_component<BoxCollider>());
}

TEST (AddBoxCollider, MissingParameters) {
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

TEST (AddCamera, ValidParameters) {
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

TEST (AddCamera, InvalidParameters) {
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

    EXPECT_FALSE(e.has_component<Camera>());
}

TEST (AddCamera, MissingParameters) {
    vector<string> parameters;

    Scene scene("Scene", "tmx");
    Entity e = Engine::getInstance().entities.create();
    scene.addCamera(parameters, e);

    EXPECT_TRUE(e.has_component<Camera>());

    ComponentHandle<Camera> camera = e.component<Camera>();

    EXPECT_EQ(camera.get()->lf, -480);
    EXPECT_EQ(camera.get()->rf, 480);
    EXPECT_EQ(camera.get()->bf, -270);
    EXPECT_EQ(camera.get()->tf, 270);
    EXPECT_EQ(camera.get()->dnp, -1);
    EXPECT_EQ(camera.get()->dfp, 1);
    EXPECT_EQ(camera.get()->x, 0);
    EXPECT_EQ(camera.get()->y, 0);
    EXPECT_EQ(camera.get()->z, 0);
}

TEST (AddCapsuleCollider, ValidParameters) {
    vector<string> parameters;
    parameters.push_back("2");
    parameters.push_back("45");
    parameters.push_back("0");
    parameters.push_back("0");
    parameters.push_back("0");
    parameters.push_back("0");
    parameters.push_back("0");

    Scene scene("Scene", "tmx");
    Entity e = Engine::getInstance().entities.create();
    scene.addCapsuleCollider(parameters, e);

    EXPECT_TRUE(e.has_component<CapsuleCollider>());

    ComponentHandle<CapsuleCollider> capCol = e.component<CapsuleCollider>();

    EXPECT_EQ(capCol.get()->radius, 2);
    EXPECT_EQ(capCol.get()->a, 45);
    EXPECT_EQ(capCol.get()->x, 0);
    EXPECT_EQ(capCol.get()->y, 0);
    EXPECT_EQ(capCol.get()->z, 0);
    EXPECT_FALSE(capCol.get()->isTrigger);
    EXPECT_FALSE(capCol.get()->render);
}

TEST (AddCapsuleCollider, InvalidParameters) {
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
    scene.addCapsuleCollider(parameters, e);

    EXPECT_FALSE(e.has_component<CapsuleCollider>());
}

TEST (AddCapsuleCollider, MissingParameters) {
    vector<string> parameters;

    Scene scene("Scene", "tmx");
    Entity e = Engine::getInstance().entities.create();
    scene.addCapsuleCollider(parameters, e);

    EXPECT_TRUE(e.has_component<CapsuleCollider>());

    ComponentHandle<CapsuleCollider> capCol = e.component<CapsuleCollider>();

    EXPECT_EQ(capCol.get()->radius, 1);
    EXPECT_EQ(capCol.get()->a, 0);
    EXPECT_EQ(capCol.get()->x, 0);
    EXPECT_EQ(capCol.get()->y, 0);
    EXPECT_EQ(capCol.get()->z, 0);
    EXPECT_FALSE(capCol.get()->isTrigger);
    EXPECT_FALSE(capCol.get()->render);
}

TEST (AddCircleCollider, ValidParameters) {
    vector<string> parameters;
    parameters.push_back("2");
    parameters.push_back("0");
    parameters.push_back("0");
    parameters.push_back("0");
    parameters.push_back("0");
    parameters.push_back("0");

    Scene scene("Scene", "tmx");
    Entity e = Engine::getInstance().entities.create();
    scene.addCircleCollider(parameters, e);

    EXPECT_TRUE(e.has_component<CircleCollider>());

    ComponentHandle<CircleCollider> cirCol = e.component<CircleCollider>();

    EXPECT_EQ(cirCol.get()->radius, 2);
    EXPECT_EQ(cirCol.get()->x, 0);
    EXPECT_EQ(cirCol.get()->y, 0);
    EXPECT_EQ(cirCol.get()->z, 0);
    EXPECT_FALSE(cirCol.get()->isTrigger);
    EXPECT_FALSE(cirCol.get()->render);
}

TEST (AddCircleCollider, InvalidParameters) {
    vector<string> parameters;
    parameters.push_back("hello");
    parameters.push_back("a");
    parameters.push_back("s");
    parameters.push_back("d");
    parameters.push_back("f");
    parameters.push_back("g");

    Scene scene("Scene", "tmx");
    Entity e = Engine::getInstance().entities.create();
    scene.addCircleCollider(parameters, e);

    EXPECT_FALSE(e.has_component<CircleCollider>());
}

TEST (AddCircleCollider, MissingParameters) {
    vector<string> parameters;

    Scene scene("Scene", "tmx");
    Entity e = Engine::getInstance().entities.create();
    scene.addCircleCollider(parameters, e);

    EXPECT_TRUE(e.has_component<CircleCollider>());

    ComponentHandle<CircleCollider> cirCol = e.component<CircleCollider>();

    EXPECT_EQ(cirCol.get()->radius, 1);
    EXPECT_EQ(cirCol.get()->x, 0);
    EXPECT_EQ(cirCol.get()->y, 0);
    EXPECT_EQ(cirCol.get()->z, 0);
    EXPECT_FALSE(cirCol.get()->isTrigger);
    EXPECT_FALSE(cirCol.get()->render);
}

TEST (AddCustomScript, ValidPath) {
    vector<string> parameters;
    parameters.push_back("../TempleAtonement/src/scripts/player.xml");

    Scene scene("Scene", "tmx");
    Entity e = Engine::getInstance().entities.create();
    scene.addCustomScript(parameters, e);

    EXPECT_TRUE(e.has_component<CustomScript>());
}

TEST (AddCustomScript, InvalidPath) {
    vector<string> parameters;
    parameters.push_back("asd");

    Scene scene("Scene", "tmx");
    Entity e = Engine::getInstance().entities.create();
    
    EXPECT_ANY_THROW(scene.addCustomScript(parameters, e));
    
}

TEST (AddCustomScript, MissingPath) {
    vector<string> parameters;

    Scene scene("Scene", "tmx");
    Entity e = Engine::getInstance().entities.create();
   EXPECT_ANY_THROW(scene.addCustomScript(parameters, e));
}

TEST (AddRigidbody_2D, ValidParameters) {
    vector<string> parameters;
    parameters.push_back("2");
    parameters.push_back("4");
    parameters.push_back("6");
    parameters.push_back("7");
    parameters.push_back("23");
    parameters.push_back("123");
    parameters.push_back("67");
    parameters.push_back("34");
    parameters.push_back("456");
    parameters.push_back("456");
    parameters.push_back("-90");
    parameters.push_back("34");
    parameters.push_back("23");
    parameters.push_back("45");

    Scene scene("Scene", "tmx");
    Entity e = Engine::getInstance().entities.create();
    scene.addRigidBody_2D(parameters, e);

    EXPECT_TRUE(e.has_component<Rigidbody_2D>());

    ComponentHandle<Rigidbody_2D> rigidbody2D = e.component<Rigidbody_2D>();

    EXPECT_EQ(rigidbody2D.get()->gravity, 2);
    EXPECT_EQ(rigidbody2D.get()->mass, 4);
    EXPECT_EQ(rigidbody2D.get()->linDrag, 6);
    EXPECT_EQ(rigidbody2D.get()->angDrag, 7);
    EXPECT_EQ(rigidbody2D.get()->cmX, 23);
    EXPECT_EQ(rigidbody2D.get()->cmY, 123);
    EXPECT_EQ(rigidbody2D.get()->cmZ, 67);
    EXPECT_EQ(rigidbody2D.get()->velocityX, 34);
    EXPECT_EQ(rigidbody2D.get()->velocityY, 456);
    EXPECT_EQ(rigidbody2D.get()->angVelocityX, 456);
    EXPECT_EQ(rigidbody2D.get()->angVelocityY, -90);
    EXPECT_EQ(rigidbody2D.get()->accelerationX, 34);
    EXPECT_EQ(rigidbody2D.get()->accelerationY, 23);
    EXPECT_EQ(rigidbody2D.get()->angAcceleration, 45);
}

TEST (AddRigidbody_2D, InvalidParameters) {
    vector<string> parameters;
    parameters.push_back("a");
    parameters.push_back("b");
    parameters.push_back("c");
    parameters.push_back("d");
    parameters.push_back("e");
    parameters.push_back("f");
    parameters.push_back("g");
    parameters.push_back("h");
    parameters.push_back("-i");
    parameters.push_back("j");
    parameters.push_back("k");
    parameters.push_back("");
    parameters.push_back("m");
    parameters.push_back("n");

    Scene scene("Scene", "tmx");
    Entity e = Engine::getInstance().entities.create();
    scene.addRigidBody_2D(parameters, e);

    EXPECT_FALSE(e.has_component<Rigidbody_2D>());
}

TEST (AddRigidbody_2D, MissingParameters) {
    vector<string> parameters;

    Scene scene("Scene", "tmx");
    Entity e = Engine::getInstance().entities.create();
    scene.addRigidBody_2D(parameters, e);

    EXPECT_TRUE(e.has_component<Rigidbody_2D>());

    ComponentHandle<Rigidbody_2D> rigidbody2D = e.component<Rigidbody_2D>();

    EXPECT_EQ(rigidbody2D.get()->gravity, 1);
    EXPECT_EQ(rigidbody2D.get()->mass, 0);
    EXPECT_EQ(rigidbody2D.get()->linDrag, -1);
    EXPECT_EQ(rigidbody2D.get()->angDrag, 0);
    EXPECT_EQ(rigidbody2D.get()->cmX, 0);
    EXPECT_EQ(rigidbody2D.get()->cmY, 0);
    EXPECT_EQ(rigidbody2D.get()->cmZ, 0);
    EXPECT_EQ(rigidbody2D.get()->velocityX, 0);
    EXPECT_EQ(rigidbody2D.get()->velocityY, 0);
    EXPECT_EQ(rigidbody2D.get()->angVelocityX, 0);
    EXPECT_EQ(rigidbody2D.get()->angVelocityY, 0);
    EXPECT_EQ(rigidbody2D.get()->accelerationX, 0);
    EXPECT_EQ(rigidbody2D.get()->accelerationY, 0);
    EXPECT_EQ(rigidbody2D.get()->angAcceleration, 0);
}


TEST (AddShaderComp, ValidParameters) {
    vector<string> parameters;
    parameters.push_back("src/res/shaders/Basic.shader");

    Scene scene("Scene", "tmx");
    Entity e = Engine::getInstance().entities.create();
    scene.addShaderComp(parameters, e);

    EXPECT_TRUE(e.has_component<ShaderComp>());
}

TEST (AddTextureComp, ValidParameters) {
    vector<string> parameters;
    parameters.push_back("../TempleAtonement/texture/player_up.png");

    Scene scene("Scene", "tmx");
    Entity e = Engine::getInstance().entities.create();
    scene.addTextureComp(parameters, e);

    EXPECT_TRUE(e.has_component<TextureComp>());
}

TEST (AddTag, ValidParameters) {
    vector<string> parameters;
    parameters.push_back("Tag");

    Scene scene("Scene", "tmx");
    Entity e = Engine::getInstance().entities.create();
    scene.addTag(parameters, e);

    EXPECT_TRUE(e.has_component<Tag>());
}

TEST (AddTag, NullCharacter) {
    vector<string> parameters;
    parameters.push_back("\0");

    Scene scene("Scene", "tmx");
    Entity e = Engine::getInstance().entities.create();
    scene.addTag(parameters, e);
    
    EXPECT_TRUE(e.has_component<Tag>());
}

TEST (AddTag, MissingParameters) {
    vector<string> parameters;

    Scene scene("Scene", "tmx");
    Entity e = Engine::getInstance().entities.create();
    scene.addTag(parameters, e);

    EXPECT_TRUE(e.has_component<Tag>());
}

TEST (AddPersistent, NoParameters) {
    vector<string> parameters;

    Scene scene("Scene", "tmx");
    Entity e = Engine::getInstance().entities.create();
    scene.addPersistent(parameters, e);

    EXPECT_TRUE(e.has_component<Persistent>());
}

TEST (AddPersistent, ManyParameters) {
    vector<string> parameters;
    parameters.push_back("1");
    parameters.push_back("2");

    Scene scene("Scene", "tmx");
    Entity e = Engine::getInstance().entities.create();
    scene.addPersistent(parameters, e);

    EXPECT_TRUE(e.has_component<Persistent>());
}
