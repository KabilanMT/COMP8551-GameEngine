#pragma once

#include <fstream>
#include <experimental/filesystem>
#include <iostream>
#include <string>
#include <windows.h>

#include "entityx/entityx.h"
#include "../Components/Components.h"
#include "../Events/Events.h"

using namespace entityx;
class CustomScriptSystem : public System<CustomScriptSystem>, public Receiver<CustomScriptSystem> {
public:
    void configure(EventManager& events) override {
        events.subscribe<SceneLoad>(*this);
    }

    void receive(const SceneLoad& sl) {
        //this method will be called when the scene loads

        //call start methods
    }

    void update(EntityManager& es, EventManager& events, TimeDelta dt) override 
    {
        // toFile(ccodestring, "ExampleCustomScript");

        // es.each<Transform>([dt, this](Entity entity, Transform &customScript) {

        //     std::cout << customScript.x << std::endl; 

        //     CScript* cscript;
        //     cscript = getCustomScriptObject("ExampleCustomScript", &entity);

        //     if (cscript != NULL)
        //         cscript->update();
        //     else
        //         std::cout << "getCustomScriptObject failed" << std::endl;

        //     std::cout << customScript.x << std::endl; 
        // });

        // cleanDir();
    }

    /**
     * Opens or Creates a cpp file to fill with a string of c++ code
     * #PARAM ccode: String C++ of code
     * #PARAM filename: Name of file you want to create to file   
     */
    // void toFile(std::string ccode, std::string filename)
    // {
    //     ofstream myfile;
    //     std::string filepath = "src/CustomScripts/" + filename + ".cpp";
    //     myfile.open(filepath);
    //     myfile << ccode;
    //     myfile.close();
    // }

    // /**
    //  * Builds dll file from Custom script and calls it's CreateCustomScriptObject function.
    //  * #PARAM filename: name of file that contains the C++ code
    //  * #PARAM ex: Entity to be referenced in the Custom Script
    //  * #RETURN CScript: Custom Script object
    //  */
    // CScript* getCustomScriptObject(std::string filename, entityx::Entity* ex)
    // {
    //     HINSTANCE hdll = NULL;
    //     CScript* cscript = NULL;
    //     typedef void* (*pvScriptObject)(entityx::Entity*);
    //     pvScriptObject createCustomScriptObject;

    //     // Create dll file 
    //     std::string gccCommand = "g++ -Iincludes -Llibs src/CustomScripts/" + filename + ".cpp -o src/CustomScripts/" + filename + ".dll -shared -fPIC -lentityx";
    //     const char *cgccCommand = gccCommand.c_str();
    //     system(cgccCommand);
        
    //     // Load the dll ( It says there is an error but it compiles and works anyways :/ )
    //     std::string path = "src\\CustomScripts\\" + filename + ".dll";
    //     const char * cpath = path.c_str();
    //     hdll = LoadLibrary(cpath);

    //     if (!hdll)
    //     {
    //        Logger::getInstance() << "Error: Failed to load dll " + filename;
    //        return NULL;
    //     }

    //     // Get the function pointer
    //     createCustomScriptObject = (pvScriptObject) (GetProcAddress(hdll, "CreateCustomScriptObject"));

    //     if (!createCustomScriptObject)
    //     {
    //         Logger::getInstance() << "Error: Failed to find CreateCustomScriptObject in " + filename;
    //         return NULL;
    //     }

    //     // Create CScript Object
    //     cscript = static_cast<CScript*> ( createCustomScriptObject(ex) ); 
        
    //     return cscript;
    // }

    // /**
    //  * Deletes all files with the exception of the ExampleCustomScript from the CustomScripts directory.
    //  * Should be called when creating new scene or ending the program.
    //  */
    // void cleanDir()
    // {
    //     for (const auto& entry : std::experimental::filesystem::directory_iterator("src/CustomScripts"))
    //     {
    //         if (entry.path().string().compare("src/CustomScripts/ExampleCustomScript.cpp") != 0)
    //             std::experimental::filesystem::remove_all(entry.path());
    //     }
    // }

private:
    std::string ccodestring = 
        "#include <iostream> \n"  
        "#include <entityx/entityx.h> \n"
        "#include \"../CScript.h\" \n"
        "#include \"../Components/Components.h\" \n\n"
        "#define EXPORT extern \"C\" __declspec(dllexport) \n\n"  
        "class ExampleCustomScript : public CScript \n" 
        "{ \n" 
        "private: \n"
        "   typedef CScript super; \n"
        "public: \n" 
        "   ExampleCustomScript(entityx::Entity* ex) : super(ex) {} \n"
        "   void update(); \n" 
        "}; \n\n" 
        "void ExampleCustomScript::update() \n" 
        "{ \n" 
        "   entityx::ComponentHandle<Transform> transform = entity->component<Transform>(); \n"
        "   transform.get()->x = 2; \n"
        "   // std::cout << entity->component<Transform>().get()->x << std::endl; \n" 
        "} \n\n"
        "EXPORT CScript* CreateCustomScriptObject(entityx::Entity* ex) \n"
        "{ \n"
        "   return new ExampleCustomScript(ex); \n"
        "} \n";
}; 