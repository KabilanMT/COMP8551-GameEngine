#include "engine.h"
#include "SceneManager.h"

#include <glad/glad.h>
#include <glfw/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include <iostream>
#include <fstream>
#include <string>
#include <sstream>

#include "renderer.h"
#include "vertexBuffer.h"
#include "indexBuffer.h"
#include "vertexArray.h"
#include "shader.h"
#include "vertexBufferLayout.h"
#include "texture.h"

#include "Systems/InputSystem.h"


void framebuffer_size_callback(GLFWwindow* window, int width, int height);
//add any method implementations here


Engine::Engine() {
    //add systems
    systems.add<InputSystem>();
    systems.add<PhysicsSystem>();
    systems.add<SoundSystem>();
    systems.add<CustomScriptSystem>();
    systems.add<RenderingSystem>();
    systems.configure();
}


void Engine::initialize() {
    //initialize
    // screen res
    const unsigned int SCR_WIDTH = 960;
    const unsigned int SCR_HEIGHT = 540;

    //Put setup here
    
    // glfw: initialize and configure
    // ------------------------------
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    // glfw window creation
    // --------------------
    window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "Mada Mada", NULL, NULL);
    if (window == NULL)
    {
        std::cout << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return;
    }
    glfwMakeContextCurrent(window);
    glfwSwapInterval(1); //VSYNC
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);

    glfwSetInputMode(window, GLFW_STICKY_KEYS, 1);
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
    glfwSetInputMode(window, GLFW_STICKY_MOUSE_BUTTONS, 1);
    glfwSetWindowUserPointer(window, reinterpret_cast<void *>(this));

    // glad: load all OpenGL function pointers
    // ---------------------------------------
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        std::cout << "Failed to initialize GLAD" << std::endl;
        return;
    }

    GLCall(glEnable(GL_BLEND));
    GLCall(glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA));

    lastTime = std::chrono::duration_cast<std::chrono::milliseconds>(clock.now().time_since_epoch());
    initialized = true;
}

void Engine::update() {
    if (!initialized) {
        initialize();
    }

    deltaTime = std::chrono::duration_cast<std::chrono::milliseconds>(clock.now().time_since_epoch()) - lastTime;
    lastTime += deltaTime;
    TimeDelta dt = deltaTime.count();
    dt *= 0.001f; //convert to seconds
    
    //do updates
    systems.update<InputSystem>(dt);
    systems.update<PhysicsSystem>(dt);
    systems.update<SoundSystem>(dt);
    systems.update<CustomScriptSystem>(dt);
    systems.update<RenderingSystem>(dt);
   
}

void Engine::start() {
    Logger::getInstance() << "Start of game engine.\n";

    entityx::Entity entity = Engine::getInstance().entities.create();

    entity.assign<Position>(
        -50.0f,  -50.0f, 0.0f, 0.0f,
         50.0f, -50.0f, 1.0f, 0.0f,
         50.0f,  50.0f, 1.0f, 1.0f,
        -50.0f,  50.0f, 0.0f, 1.0f,

        0,1,2,
        2,3,0
    );

    entity.assign<ShaderComp>("src/res/shaders/Basic.shader");
    entity.assign<TextureComp>("src/res/textures/Sport.png");
    entity.assign<Translation>(50, 50, 0);
    entity.assign<Rotate>(0, 0, 0, 1);
    entity.assign<Camera>(0.0f, 960.0f, 0.0f, 540.0f, -1.0f, 1.0f);

    SceneManager::getInstance().start();
    // render loop
    // -----------
    while (!glfwWindowShouldClose(window))
    {   
        update();

        //Swap front and back buffers
        glfwSwapBuffers(window);
     
    }
    // glfw: terminate, clearing all previously allocated GLFW resources.
    // ------------------------------------------------------------------
   
    glfwTerminate();
    return;
}

// glfw: whenever the window size changed (by OS or user resize) this callback function executes
// ---------------------------------------------------------------------------------------------
void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
    // make sure the viewport matches the new window dimensions; note that width and 
    // height will be significantly larger than specified on retina displays.
    glViewport(0, 0, width, height);
} 