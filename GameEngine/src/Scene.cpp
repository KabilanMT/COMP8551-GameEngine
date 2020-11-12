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

            //Check if the object has the same name as any of the persistant ones
            //If it does, dont create the Entity
            
            const XMLAttribute* persAttr = elem->FirstAttribute(); //get the object's attributes
            bool isPersistent = false;
            Entity* theEntity = nullptr;
            while (persAttr != NULL) {
                string aName = persAttr->Name();
                if (aName == "name") {
                    string name = persAttr->Value();
                    for (Entity pEntity : persistentEntities) {
                        if (pEntity.valid() && pEntity.has_component<Name>() && pEntity.component<Name>()->getName() == name) {
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
                theEntity->component<Transform>()->z = layerNum / 1000.0f;
                layerNum++;
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
    bool isPlayOnLoad;
    stringstream str(parameters.at(1));
    str >> isPlayOnLoad;
    str = stringstream(parameters.at(2));
    bool isLoop;
    str >> isLoop;
    e.assign<AudioSource>(parameters.at(0).c_str(), isPlayOnLoad, isLoop, parameters.at(3));
}

void Scene::addBoxCollider(vector<string>& parameters, Entity& e) {
    stringstream str(parameters.at(0));
    float width;
    str >> width;
    
    str = stringstream(parameters.at(1));
    float height;
    str >> height;

    str = stringstream(parameters.at(2));
    float x;
    str >> x;

    str = stringstream(parameters.at(3));
    float y;
    str >> y;

    str = stringstream(parameters.at(4));
    float z;
    str >> z;

    str = stringstream(parameters.at(5));
    bool isTrigger;
    str >> isTrigger;
    
    str = stringstream(parameters.at(6));
    bool render;
    str >> render;

    e.assign<BoxCollider>(width, height, x, y, z, isTrigger, render);
}

void Scene::addCamera(vector<string>& parameters, Entity& e) {
    float lf, rf, bf, tf, dnp, dfp;

    stringstream str(parameters.at(0));
    str >> lf;
	str = stringstream(parameters.at(1));
    str >> rf;
	str = stringstream(parameters.at(2));
    str >> bf;
	str =  stringstream(parameters.at(3));
    str >> tf;
	str =  stringstream(parameters.at(4));
    str >> dnp;
	str =  stringstream(parameters.at(5));
    str >> dfp;

    e.assign<Camera>(lf, rf, bf, tf, dnp, dfp);

}

void Scene::addCapsuleCollider(vector<string>& parameters, Entity& e) {

    stringstream str(parameters.at(0));
    float radius;
    str >> radius;
    
    str = stringstream(parameters.at(1));
    float a;
    str >> a;

    str = stringstream(parameters.at(2));
    float x;
    str >> x;

    str = stringstream(parameters.at(3));
    float y;
    str >> y;

    str = stringstream(parameters.at(4));
    float z;
    str >> z;

    str = stringstream(parameters.at(5));
    bool isTrigger;
    str >> isTrigger;
    
    str = stringstream(parameters.at(6));
    bool render;
    str >> render;

    e.assign<CapsuleCollider>(radius, a, x, y, z, isTrigger, render);
}

void Scene::addCircleCollider(vector<string>& parameters, Entity& e) {
    stringstream str(parameters.at(0));
    float radius;
    str >> radius;

    str = stringstream(parameters.at(1));
    float x;
    str >> x;

    str = stringstream(parameters.at(2));
    float y;
    str >> y;

    str = stringstream(parameters.at(3));
    float z;
    str >> z;

    str = stringstream(parameters.at(4));
    bool isTrigger;
    str >> isTrigger;
    
    str = stringstream(parameters.at(5));
    bool render;
    str >> render;

    e.assign<CircleCollider>(radius, x, y, z, isTrigger, render);
}

void Scene::addCustomScript(vector<string>& parameters, Entity& e) {
    e.assign<CustomScript>(parameters.at(0));
}

void Scene::addRigidBody_2D(vector<string>& parameters, Entity& e) {

    stringstream str(parameters.at(0));
    float gravity;
    str >> gravity;

    str = stringstream(parameters.at(1));
    float mass;
    str >> mass;

    str = stringstream(parameters.at(2));
    float linDrag;
    str >> linDrag;

    str = stringstream(parameters.at(3));
    float angDrag;
    str >> angDrag;

    str = stringstream(parameters.at(4));
    float cmX;
    str >> cmX;

    str = stringstream(parameters.at(5));
    float cmY;
    str >> cmY;

    str = stringstream(parameters.at(6));
    float cmZ;
    str >> cmZ;

    str = stringstream(parameters.at(7));
    float velocityX;
    str >> velocityX;

    str = stringstream(parameters.at(8));
    float velocityY;
    str >> velocityY;

    str = stringstream(parameters.at(9));
    float angVelocityX;
    str >> angVelocityX;

    str = stringstream(parameters.at(10));
    float angVelocityY;
    str >> angVelocityY;

    str = stringstream(parameters.at(11));
    float accelerationX;
    str >> accelerationX;

    str = stringstream(parameters.at(12));
    float accelerationY;
    str >> accelerationY;

    str = stringstream(parameters.at(13));
    float angAcceleration;
    str >> angAcceleration;

    e.assign<Rigidbody_2D>(gravity, mass, linDrag, angDrag, cmX, cmY, cmZ, velocityX, velocityY, angVelocityX, angVelocityY, accelerationX, accelerationY, angAcceleration);
}

void Scene::addShaderComp(vector<string>& parameters, Entity& e) {
    string temp = parameters.at(0);
    int n=temp.length();
    char *chararray= new char [n+1];
    strcpy(chararray,temp.c_str());
  
    e.assign<ShaderComp>(chararray);
}

void Scene::addTextureComp(vector<string>& parameters, Entity& e) {
    string temp = parameters.at(0);
    int n=temp.length();
    char *chararray= new char [n+1];
    strcpy(chararray,temp.c_str());
  
    e.assign<TextureComp>(chararray);
}

void Scene::addTag(vector<string>& parameters, Entity& e) {
    e.assign<Tag>(parameters.at(0));
}

void Scene::addPersistent(vector<string>& parameters, Entity& e) {
    e.assign<Persistent>();
}