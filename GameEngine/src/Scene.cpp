#include "Scene.h"

#include "logger.h"
#include "engine.h"

#include <tinyxml2.h>

#include <sstream>
#include <string.h>

#include "Components/Components.h"

using namespace tinyxml2;

string Scene::getName() {
    return name;
}

void Scene::load(vector<Entity>& persistentEntities) {

    Logger::getInstance() << "Scene " << name << " is being parsed!\n";

    //load doc
    TinyXMLDocument doc;
    XMLError xmlerr = doc.LoadFile(fileName.c_str());
    if (xmlerr != XML_SUCCESS) {
        Logger::getInstance() << "Could not read TMX file!\n";
        return;
    }

    XMLElement* map = doc.FirstChildElement("map");
    XMLElement* objectGroup = map->FirstChildElement("objectgroup");
    XMLNode* object = objectGroup->FirstChild();

    Engine &e = Engine::getInstance();
    int layerNum = 0;
    while (object != NULL) { //while there are objects to process
        XMLElement* elem = object->ToElement();

        if (!strcmp(elem->Name(), "object")) { //check to make sure it's actually an object

            //Check if the object has the same name as any of the persistent ones
            //If it does, dont create the Entity
            
            const XMLAttribute* persAttr = elem->FirstAttribute(); //get the object's attributes
            bool isPersistent = false;
            Entity* theEntity = nullptr;
            while (persAttr != NULL) {
                string aName = persAttr->Name();
                if (aName == "name") {
                    Logger::getInstance() << "Got the name attribute: " << persAttr->Value() << "\n";
                    string name = persAttr->Value();
                    for (Entity pEntity : persistentEntities) {
                        if (pEntity.valid() && pEntity.has_component<Name>() && pEntity.component<Name>()->getName() == name) {
                            Logger::getInstance() << "This entity matches a persistent!\n";
                            isPersistent = true;
                            theEntity = &pEntity;
                            break;
                        }
                    }
                    break;
                }
                persAttr = persAttr->Next();
            }

            if (isPersistent) {
                Logger::getInstance() << "Update layer and continue\n";
                theEntity->component<Transform>()->z = layerNum / 1000.0f;
                layerNum++;
                object = object->NextSibling();
                continue;
            }

            Entity ent = e.entities.create(); //create the entity
            const XMLAttribute* attr = elem->FirstAttribute(); //get the object's attributes

            float x = 0, y = 0, z = 0, width = 0, height = 0, rotAngle = 0;
            bool isVisible = true;
            while (attr != NULL) { //loop through object attributes, assign certain components based on them
                string aName = attr->Name();
                if (aName == "name") {
                    ent.assign<Name>(attr->Value());
                } else if (aName == "x") {
                    stringstream str(attr->Value());
                    str >> x;
                } else if (aName == "y") {
                    stringstream str(attr->Value());
                    str >> y;
                } else if (aName == "width") {
                    stringstream str(attr->Value());
                    str >> width;
                } else if (aName == "height") {
                    stringstream str(attr->Value());
                    str >> height;
                } else if (aName == "rotation") {
                    stringstream str(attr->Value());
                    str >> rotAngle;
                } else if (aName == "visible") {
                    stringstream str(attr->Value());
                    isVisible = false;
                }

                attr = attr->Next();
            }
            ent.assign<Active>(isVisible);
            // Add rotation passing in a degrees (rotAngle)
            // Convert width and height to sprite vertices
            x = x + width/2;
            y = (y + height/2) * -1;
            z = layerNum / 1000.f;
            ent.assign<Transform>(x, y, z, rotAngle, 0, 0, 1);
            layerNum++;
            //TODO width and height need to be applied to sprite vertices
            ent.assign<SpriteVertices>(
                -width/2, -height/2, 0.0f, 0.0f,
                 width/2, -height/2, 1.0f, 0.0f,
                 width/2,  height/2, 1.0f, 1.0f,
                -width/2,  height/2, 0.0f, 1.0f,

                0,1,2,
                2,3,0
            );

            //loop through properties for most other components
            //get first property
            XMLNode* properties = elem->FirstChild();
            if (properties != NULL) {
                XMLNode* property = properties->FirstChild();

                while (property != NULL) { //loop through all properties
                    //each property will have a name attribute, and a value attribute
                    //name attribute determines component type
                    //value is a comma delimited string, where each value between commas is one value for the component
                        //needs to be parsed
                    XMLElement* propElem = property->ToElement();
                    //get name
                    string compType = propElem->Attribute("name");
                    string compValues = propElem->Attribute("value");

                    vector<string> parameters = parseParameters(compValues);

                    if (compType == "AudioSource") {
                        addAudioSource(parameters, ent);
                    } else if (compType == "BoxCollider") {
                        addBoxCollider(parameters, ent);
                    } else if (compType == "Camera") {
                        addCamera(parameters, ent);
                    } else if (compType == "CapsuleCollider") {
                        addCapsuleCollider(parameters, ent);
                    } else if (compType == "CircleCollider") {
                        addCircleCollider(parameters, ent);
                    } else if (compType == "CustomScript") {
                        addCustomScript(parameters, ent);
                    } else if (compType == "Rigidbody_2D") {
                        addRigidBody_2D(parameters, ent);
                    } else if (compType == "ShaderComp") {
                        addShaderComp(parameters, ent);
                    } else if (compType == "TextureComp") {
                        addTextureComp(parameters, ent);
                    } else if (compType == "Tag") {
                        addTag(parameters, ent);
                    } else if (compType == "Persistent") {
                        addPersistent(parameters, ent);
                    }
                    property = property->NextSibling();
                }
            }
        }

        object = object->NextSibling();
    }

}

vector<string> Scene::parseParameters(string parameters) {
    vector<string> values;
    size_t i = 0;
    while ((i = parameters.find(",")) != string::npos) {
        values.push_back(parameters.substr(0, i));
        parameters.erase(0, i + 1);
    }
    values.push_back(parameters);

    return values;
}

Scene::Scene(string sceneName, string tmxFile) : name(sceneName), fileName(tmxFile) {
    
}


void Scene::addAudioSource(vector<string>& parameters, Entity& e) {
    //requires string,bool,bool,string

    if (parameters.size() == 0) {
        Logger::getInstance() << "Scene parsing - AddAudioSource Error: no parameters - requires at least 1\n";
        return;
    }

    if (parameters.size() == 1) {
        e.assign<AudioSource>(parameters.at(0).c_str());
        return;
    }

    bool isPlayOnLoad;
    stringstream str(parameters.at(1));
    //error check - bool 1
    if (str.str() == "true" || str.str() == "false") {
        Logger::getInstance() << "Scene parsing - AddAudioSource Error: boolean 1 was found to be true/false - use 1/0 instead.\n";
        return;
    } else if (str.str() != "0" && str.str() != "1") {
        Logger::getInstance() << "Scene parsing - AddAudioSource Error: boolean 1 incorrect value - use 1/0.\n";
        return;
    }
    str >> isPlayOnLoad;

    if (parameters.size() == 2) {
        e.assign<AudioSource>(parameters.at(0).c_str(), isPlayOnLoad);
        return;
    }

    str = stringstream(parameters.at(2));
        //error check - bool 2
    if (str.str() == "true" || str.str() == "false") {
        Logger::getInstance() << "Scene parsing - AddAudioSource Error: boolean 2 was found to be true/false - use 1/0 instead.\n";
        return;
    } else if (str.str() != "0" && str.str() != "1") {
        Logger::getInstance() << "Scene parsing - AddAudioSource Error: boolean 2 invalid value - use 1/0.\n";
        return;
    }
    bool isLoop;
    str >> isLoop;

    if (parameters.size() == 3) {
        e.assign<AudioSource>(parameters.at(0).c_str(), isPlayOnLoad, isLoop);
        return;
    }

    e.assign<AudioSource>(parameters.at(0).c_str(), isPlayOnLoad, isLoop, parameters.at(3));
}

void Scene::addBoxCollider(vector<string>& parameters, Entity& e) {
    //requires float,float,float,float,float,bool,bool

    if (parameters.size() == 0) {
        e.assign<BoxCollider>();
        return;
    }

    stringstream str(parameters.at(0));
    float width;
    //error check - float 1
    if (!(str >> width)) {
        Logger::getInstance() << "Scene parsing - AddBoxCollider Error: float 1 invalid value.\n";
        return;
    }
    if (parameters.size() == 1) {
        e.assign<BoxCollider>(width);
        return;
    }
    
    str = stringstream(parameters.at(1));
    float height;
    //error check - float 2
    if (!(str >> height)) {
        Logger::getInstance() << "Scene parsing - AddBoxCollider Error: float 2 invalid value.\n";
        return;
    }
    if (parameters.size() == 2) {
        e.assign<BoxCollider>(width, height);
        return;
    }

    str = stringstream(parameters.at(2));
    float x;
    //error check - float 3
    if (!(str >> x)) {
        Logger::getInstance() << "Scene parsing - AddBoxCollider Error: float 3 invalid value.\n";
        return;
    }
    if (parameters.size() == 3) {
        e.assign<BoxCollider>(width, height, x);
        return;
    }

    str = stringstream(parameters.at(3));
    float y;
    //error check - float 4
    if (!(str >> y)) {
        Logger::getInstance() << "Scene parsing - AddBoxCollider Error: float 4 invalid value.\n";
        return;
    }
    if (parameters.size() == 4) {
        e.assign<BoxCollider>(width, height, x, y);
        return;
    }

    str = stringstream(parameters.at(4));
    float z;
    //error check - float 5
    if (!(str >> z)) {
        Logger::getInstance() << "Scene parsing - AddBoxCollider Error: float 5 invalid value.\n";
        return;
    }
    if (parameters.size() == 5) {
        e.assign<BoxCollider>(width, height, x, y, z);
        return;
    }

    str = stringstream(parameters.at(5));
    bool isTrigger;
    //error check - bool 1
    if (str.str() == "true" || str.str() == "false") {
        Logger::getInstance() << "Scene parsing - AddBoxCollider Error: boolean 1 was found to be true/false - use 1/0 instead.\n";
        return;
    } else if (str.str() != "0" && str.str() != "1") {
        Logger::getInstance() << "Scene parsing - AddBoxCollider Error: boolean 1 invalid value - use 1/0.\n";
        return;
    }
    str >> isTrigger;
    if (parameters.size() == 6) {
        e.assign<BoxCollider>(width, height, x, y, z, isTrigger);
        return;
    }
    
    str = stringstream(parameters.at(6));
    bool render;
    //error check - bool 2
    if (str.str() == "true" || str.str() == "false") {
        Logger::getInstance() << "Scene parsing - AddBoxCollider Error: boolean 2 was found to be true/false - use 1/0 instead.\n";
        return;
    } else if (str.str() != "0" && str.str() != "1") {
        Logger::getInstance() << "Scene parsing - AddBoxCollider Error: boolean 2 invalid value - use 1/0.\n";
        return;
    }
    str >> render;

    e.assign<BoxCollider>(width, height, x, y, z, isTrigger, render);
}

void Scene::addCamera(vector<string>& parameters, Entity& e) {
    //requires float,float,float,float,float,float
    float lf, rf, bf, tf, dnp, dfp;

    if (parameters.size() == 0) {
        e.assign<Camera>();
        return;
    }

    stringstream str(parameters.at(0));
    //error check - float 1
    if (!(str >> lf)) {
        Logger::getInstance() << "Scene parsing - addCamera Error: float 1 invalid value.\n";
        return;
    }
    if (parameters.size() == 1) {
        e.assign<Camera>(lf);
        return;
    }

	str = stringstream(parameters.at(1));
    //error check - float 2
    if (!(str >> rf)) {
        Logger::getInstance() << "Scene parsing - addCamera Error: float 2 invalid value.\n";
        return;
    }
    if (parameters.size() == 2) {
        e.assign<Camera>(lf, rf);
        return;
    }

	str = stringstream(parameters.at(2));
    //error check - float 3
    if (!(str >> bf)) {
        Logger::getInstance() << "Scene parsing - addCamera Error: float 3 invalid value.\n";
        return;
    }
    if (parameters.size() == 3) {
        e.assign<Camera>(lf, rf, bf);
        return;
    }

	str =  stringstream(parameters.at(3));
    //error check - float 4
    if (!(str >> tf)) {
        Logger::getInstance() << "Scene parsing - addCamera Error: float 4 invalid value.\n";
        return;
    }
    if (parameters.size() == 4) {
        e.assign<Camera>(lf, rf, bf, tf);
        return;
    }

	str =  stringstream(parameters.at(4));
    //error check - float 5
    if (!(str >> dnp)) {
        Logger::getInstance() << "Scene parsing - addCamera Error: float 5 invalid value.\n";
        return;
    }
    if (parameters.size() == 5) {
        e.assign<Camera>(lf, rf, bf, tf, dnp);
        return;
    }

	str =  stringstream(parameters.at(5));
    //error check - float 6
    if (!(str >> dfp)) {
        Logger::getInstance() << "Scene parsing - addCamera Error: float 6 invalid value.\n";
        return;
    }

    e.assign<Camera>(lf, rf, bf, tf, dnp, dfp);

}

void Scene::addCapsuleCollider(vector<string>& parameters, Entity& e) {
    //requires float,float,float,float,float,bool,bool
    if (parameters.size() == 0) {
        e.assign<CapsuleCollider>();
        return;
    }

    stringstream str(parameters.at(0));
    float radius;
    //error check - float 1
    if (!(str >> radius)) {
        Logger::getInstance() << "Scene parsing - addCapsuleCollider Error: float 1 invalid value.\n";
        return;
    }
    if (parameters.size() == 1) {
        e.assign<CapsuleCollider>(radius);
        return;
    }
    
    str = stringstream(parameters.at(1));
    float a;
    //error check - float 2
    if (!(str >> a)) {
        Logger::getInstance() << "Scene parsing - addCapsuleCollider Error: float 2 invalid value.\n";
        return;
    }
    if (parameters.size() == 2) {
        e.assign<CapsuleCollider>(radius, a);
        return;
    }

    str = stringstream(parameters.at(2));
    float x;
    //error check - float 3
    if (!(str >> x)) {
        Logger::getInstance() << "Scene parsing - addCapsuleCollider Error: float 3 invalid value.\n";
        return;
    }
    if (parameters.size() == 3) {
        e.assign<CapsuleCollider>(radius, a, x);
        return;
    }

    str = stringstream(parameters.at(3));
    float y;
    //error check - float 4
    if (!(str >> y)) {
        Logger::getInstance() << "Scene parsing - addCapsuleCollider Error: float 4 invalid value.\n";
        return;
    }
    if (parameters.size() == 4) {
        e.assign<CapsuleCollider>(radius, a, x, y);
        return;
    }

    str = stringstream(parameters.at(4));
    float z;
    //error check - float 5
    if (!(str >> z)) {
        Logger::getInstance() << "Scene parsing - addCapsuleCollider Error: float 5 invalid value.\n";
        return;
    }
    if (parameters.size() == 5) {
        e.assign<CapsuleCollider>(radius, a, x, y, z);
        return;
    }

    str = stringstream(parameters.at(5));
    bool isTrigger;
    //error check - bool 1
    if (str.str() == "true" || str.str() == "false") {
        Logger::getInstance() << "Scene parsing - addCapsuleCollider Error: boolean 1 was found to be true/false - use 1/0 instead.\n";
        return;
    } else if (str.str() != "0" && str.str() != "1") {
        Logger::getInstance() << "Scene parsing - addCapsuleCollider Error: boolean 1 invalid value - use 1/0.\n";
        return;
    }
    str >> isTrigger;
    if (parameters.size() == 6) {
        e.assign<CapsuleCollider>(radius, a, x, y, z, isTrigger);
        return;
    }
    
    str = stringstream(parameters.at(6));
    bool render;
    //error check - bool 2
    if (str.str() == "true" || str.str() == "false") {
        Logger::getInstance() << "Scene parsing - addCapsuleCollider Error: boolean 2 was found to be true/false - use 1/0 instead.\n";
        return;
    } else if (str.str() != "0" && str.str() != "1") {
        Logger::getInstance() << "Scene parsing - addCapsuleCollider Error: boolean 2 invalid value - use 1/0.\n";
        return;
    }
    str >> render;

    e.assign<CapsuleCollider>(radius, a, x, y, z, isTrigger, render);
}

void Scene::addCircleCollider(vector<string>& parameters, Entity& e) {
    //requires float,float,float,float,bool,bool
    if (parameters.size() == 0) {
        e.assign<CircleCollider>();
        return;
    }

    stringstream str(parameters.at(0));
    float radius;
    //error check - float 1
    if (!(str >> radius)) {
        Logger::getInstance() << "Scene parsing - addCircleCollider Error: float 1 invalid value.\n";
        return;
    }
    if (parameters.size() == 1) {
        e.assign<CircleCollider>(radius);
        return;
    }

    str = stringstream(parameters.at(1));
    float x;
    //error check - float 2
    if (!(str >> x)) {
        Logger::getInstance() << "Scene parsing - addCircleCollider Error: float 2 invalid value.\n";
        return;
    }
    if (parameters.size() == 2) {
        e.assign<CircleCollider>(radius, x);
        return;
    }

    str = stringstream(parameters.at(2));
    float y;
    //error check - float 3
    if (!(str >> y)) {
        Logger::getInstance() << "Scene parsing - addCircleCollider Error: float 3 invalid value.\n";
        return;
    }
    if (parameters.size() == 3) {
        e.assign<CircleCollider>(radius, x, y);
        return;
    }

    str = stringstream(parameters.at(3));
    float z;
    //error check - float 4
    if (!(str >> z)) {
        Logger::getInstance() << "Scene parsing - addCircleCollider Error: float 4 invalid value.\n";
        return;
    }
    if (parameters.size() == 4) {
        e.assign<CircleCollider>(radius, x, y, z);
        return;
    }

    str = stringstream(parameters.at(4));
    bool isTrigger;
    //error check - bool 1
    if (str.str() == "true" || str.str() == "false") {
        Logger::getInstance() << "Scene parsing - addCircleCollider Error: boolean 1 was found to be true/false - use 1/0 instead.\n";
        return;
    } else if (str.str() != "0" && str.str() != "1") {
        Logger::getInstance() << "Scene parsing - addCircleCollider Error: boolean 1 invalid value - use 1/0.\n";
        return;
    }
    str >> isTrigger;
    if (parameters.size() == 5) {
        e.assign<CircleCollider>(radius, x, y, z, isTrigger);
        return;
    }
    
    str = stringstream(parameters.at(5));
    bool render;
    //error check - bool 2
    if (str.str() == "true" || str.str() == "false") {
        Logger::getInstance() << "Scene parsing - addCircleCollider Error: boolean 2 was found to be true/false - use 1/0 instead.\n";
        return;
    } else if (str.str() != "0" && str.str() != "1") {
        Logger::getInstance() << "Scene parsing - addCircleCollider Error: boolean 2 invalid value - use 1/0.\n";
        return;
    }
    str >> render;

    e.assign<CircleCollider>(radius, x, y, z, isTrigger, render);
}

void Scene::addCustomScript(vector<string>& parameters, Entity& e) {
    e.assign<CustomScript>(parameters.at(0));
}

void Scene::addRigidBody_2D(vector<string>& parameters, Entity& e) {
    //requires float,float,float,float,float,float,float,float,float,float,float,float,float,float (14 floats)

    if (parameters.size() == 0) {
        e.assign<Rigidbody_2D>();
        return;
    }

    stringstream str(parameters.at(0));
    float gravity;
    //error check - float 1
    if (!(str >> gravity)) {
        Logger::getInstance() << "Scene parsing - addRigidBody_2D Error: float 1 invalid value.\n";
        return;
    }
    if (parameters.size() == 1) {
        e.assign<Rigidbody_2D>(gravity);
        return;
    }

    str = stringstream(parameters.at(1));
    float mass;
    //error check - float 2
    if (!(str >> mass)) {
        Logger::getInstance() << "Scene parsing - addRigidBody_2D Error: float 2 invalid value.\n";
        return;
    }
    if (parameters.size() == 2) {
        e.assign<Rigidbody_2D>(gravity, mass);
        return;
    }

    str = stringstream(parameters.at(2));
    float linDrag;
    //error check - float 3
    if (!(str >> linDrag)) {
        Logger::getInstance() << "Scene parsing - addRigidBody_2D Error: float 3 invalid value.\n";
        return;
    }
    if (parameters.size() == 3) {
        e.assign<Rigidbody_2D>(gravity, mass, linDrag);
        return;
    }

    str = stringstream(parameters.at(3));
    float angDrag;
    //error check - float 4
    if (!(str >> angDrag)) {
        Logger::getInstance() << "Scene parsing - addRigidBody_2D Error: float 4 invalid value.\n";
        return;
    }
    if (parameters.size() == 4) {
        e.assign<Rigidbody_2D>(gravity, mass, linDrag, angDrag);
        return;
    }

    str = stringstream(parameters.at(4));
    float cmX;
    //error check - float 5
    if (!(str >> cmX)) {
        Logger::getInstance() << "Scene parsing - addRigidBody_2D Error: float 5 invalid value.\n";
        return;
    }
    if (parameters.size() == 5) {
        e.assign<Rigidbody_2D>(gravity, mass, linDrag, angDrag, cmX);
        return;
    }

    str = stringstream(parameters.at(5));
    float cmY;
    //error check - float 6
    if (!(str >> cmY)) {
        Logger::getInstance() << "Scene parsing - addRigidBody_2D Error: float 6 invalid value.\n";
        return;
    }
    if (parameters.size() == 6) {
        e.assign<Rigidbody_2D>(gravity, mass, linDrag, angDrag, cmX, cmY);
        return;
    }

    str = stringstream(parameters.at(6));
    float cmZ;
    //error check - float 7
    if (!(str >> cmZ)) {
        Logger::getInstance() << "Scene parsing - addRigidBody_2D Error: float 7 invalid value.\n";
        return;
    }
    if (parameters.size() == 7) {
        e.assign<Rigidbody_2D>(gravity, mass, linDrag, angDrag, cmX, cmY, cmZ);
        return;
    }

    str = stringstream(parameters.at(7));
    float velocityX;
    //error check - float 8
    if (!(str >> velocityX)) {
        Logger::getInstance() << "Scene parsing - addRigidBody_2D Error: float 8 invalid value.\n";
        return;
    }
    if (parameters.size() == 8) {
        e.assign<Rigidbody_2D>(gravity, mass, linDrag, angDrag, cmX, cmY, cmZ, velocityX);
        return;
    }

    str = stringstream(parameters.at(8));
    float velocityY;
    //error check - float 9
    if (!(str >> velocityY)) {
        Logger::getInstance() << "Scene parsing - addRigidBody_2D Error: float 9 invalid value.\n";
        return;
    }
    if (parameters.size() == 9) {
        e.assign<Rigidbody_2D>(gravity, mass, linDrag, angDrag, cmX, cmY, cmZ, velocityX, velocityY);
        return;
    }

    str = stringstream(parameters.at(9));
    float angVelocityX;
    //error check - float 10
    if (!(str >> angVelocityX)) {
        Logger::getInstance() << "Scene parsing - addRigidBody_2D Error: float 10 invalid value.\n";
        return;
    }
    if (parameters.size() == 10) {
        e.assign<Rigidbody_2D>(gravity, mass, linDrag, angDrag, cmX, cmY, cmZ, velocityX, velocityY, angVelocityX);
        return;
    }

    str = stringstream(parameters.at(10));
    float angVelocityY;
    //error check - float 11
    if (!(str >> angVelocityY)) {
        Logger::getInstance() << "Scene parsing - addRigidBody_2D Error: float 11 invalid value.\n";
        return;
    }
    if (parameters.size() == 11) {
        e.assign<Rigidbody_2D>(gravity, mass, linDrag, angDrag, cmX, cmY, cmZ, velocityX, velocityY, angVelocityX, angVelocityY);
        return;
    }

    str = stringstream(parameters.at(11));
    float accelerationX;
    //error check - float 12
    if (!(str >> accelerationX)) {
        Logger::getInstance() << "Scene parsing - addRigidBody_2D Error: float 12 invalid value.\n";
        return;
    }
    if (parameters.size() == 12) {
        e.assign<Rigidbody_2D>(gravity, mass, linDrag, angDrag, cmX, cmY, cmZ, velocityX, velocityY, angVelocityX, angVelocityY, accelerationX);
        return;
    }

    str = stringstream(parameters.at(12));
    float accelerationY;
    //error check - float 13
    if (!(str >> accelerationY)) {
        Logger::getInstance() << "Scene parsing - addRigidBody_2D Error: float 13 invalid value.\n";
        return;
    }
    if (parameters.size() == 13) {
        e.assign<Rigidbody_2D>(gravity, mass, linDrag, angDrag, cmX, cmY, cmZ, velocityX, velocityY, angVelocityX, angVelocityY, accelerationX, accelerationY);
        return;
    }

    str = stringstream(parameters.at(13));
    float angAcceleration;
    //error check - float 14
    if (!(str >> angAcceleration)) {
        Logger::getInstance() << "Scene parsing - addRigidBody_2D Error: float 14 invalid value.\n";
        return;
    }

    e.assign<Rigidbody_2D>(gravity, mass, linDrag, angDrag, cmX, cmY, cmZ, velocityX, velocityY, angVelocityX, angVelocityY, accelerationX, accelerationY, angAcceleration);
}

void Scene::addShaderComp(vector<string>& parameters, Entity& e) {
    //requires string

    string temp = parameters.at(0);
    int n=temp.length();
    char *chararray= new char [n+1];
    strcpy(chararray,temp.c_str());
  
    e.assign<ShaderComp>(chararray);
}

void Scene::addTextureComp(vector<string>& parameters, Entity& e) {
    //requires string

    string temp = parameters.at(0);
    int n=temp.length();
    char *chararray= new char [n+1];
    strcpy(chararray,temp.c_str());
  
    e.assign<TextureComp>(chararray);
}

void Scene::addTag(vector<string>& parameters, Entity& e) {
    //requires string
    
    if (parameters.size() == 0) {
        e.assign<Tag>();
        return;
    }

    e.assign<Tag>(parameters.at(0));
}

void Scene::addPersistent(vector<string>& parameters, Entity& e) {
    //requires nothing

    e.assign<Persistent>();
}