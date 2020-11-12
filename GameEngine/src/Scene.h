#pragma once

#include <string>
#include <vector>
#include <entityx/entityx.h>



using namespace std;
using namespace entityx;
class Scene {
    public:
        string getName();
        void load(vector<Entity>& persistentEntities);

        Scene(string sceneName, string tmxFile);
    private:
        string name;
        string fileName;

        vector<string> parseParameters(string);

        void addAudioSource(vector<string>& parameters, Entity& e);
        void addBoxCollider(vector<string>& parameters, Entity& e);
        void addCamera(vector<string>& parameters, Entity& e);
        void addCapsuleCollider(vector<string>& parameters, Entity& e);
        void addCircleCollider(vector<string>& parameters, Entity& e);
        void addCustomScript(vector<string>& parameters, Entity& e);
        void addRigidBody_2D(vector<string>& parameters, Entity& e);
        void addShaderComp(vector<string>& parameters, Entity& e);
        void addTextureComp(vector<string>& parameters, Entity& e);
        void addTag(vector<string>& parameters, Entity& e);
        void addPersistent(vector<string>& parameters, Entity& e);
};