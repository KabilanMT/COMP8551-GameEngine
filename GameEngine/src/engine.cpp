#include "engine.h"
#include "Systems/SoundSystem.h"
#include "Systems/InputSystem.h"
#include "Systems/CustomScriptSystem.h"
#include "Systems/PhysicsSystem.h"
#include "Systems/RenderingSystem.h"
#include "SceneManager.h"

#include <glad/glad.h>
#include <glfw/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <ft2build.h>
#include FT_FREETYPE_H

#include <iostream>
#include <fstream>
#include <sstream>
#include <map>

#include "renderer.h"
#include "vertexBuffer.h"
#include "indexBuffer.h"
#include "vertexArray.h"
#include "shader.h"
#include "vertexBufferLayout.h"
#include "texture.h"

#include "Systems/InputSystem.h"
#include "Systems/PhysicsSystem.h"
#include "Systems/SoundSystem.h"
#include "Systems/CustomScriptSystem.h"
#include "Systems/RenderingSystem.h"

void framebuffer_size_callback(GLFWwindow* window, int width, int height);
//add any method implementations here
std::map<char, Character> Characters;

Character Engine::getCharDetails(std::string::const_iterator c){
    Character ch = Characters[*c];
    return ch;
}

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
    GLCall(glPixelStorei(GL_UNPACK_ALIGNMENT, 1)); // disable byte-alignment restriction

    lastTime = std::chrono::duration_cast<std::chrono::milliseconds>(clock.now().time_since_epoch());
    initialized = true;


    //Free Type Initialization
    FT_Library ft;
    if (FT_Init_FreeType(&ft))
    {
        std::cout << "ERROR::FREETYPE: Could not init FreeType Library" << std::endl;
        return;
    }

    FT_Face face;
    if (FT_New_Face(ft, "src/res/fonts/arial.ttf", 0, &face))
    {
        std::cout << "ERROR::FREETYPE: Failed to load font" << std::endl;  
        return;
    }
    FT_Set_Pixel_Sizes(face, 0, 48);  
    if (FT_Load_Char(face, 'X', FT_LOAD_RENDER))
    {
        std::cout << "ERROR::FREETYTPE: Failed to load Glyph" << std::endl;  
        return;
    }
    
    for (unsigned char c = 0; c < 128; c++)
    {
        // load character glyph 
        if (FT_Load_Char(face, c, FT_LOAD_RENDER))
        {
            std::cout << "ERROR::FREETYTPE: Failed to load Glyph" << std::endl;
            continue;
        }
        // generate texture
        unsigned int texture;
        glGenTextures(1, &texture);
        glBindTexture(GL_TEXTURE_2D, texture);
        glTexImage2D(
            GL_TEXTURE_2D,
            0,
            GL_ALPHA,
            face->glyph->bitmap.width,
            face->glyph->bitmap.rows,
            0,
            GL_ALPHA,
            GL_UNSIGNED_BYTE,
            face->glyph->bitmap.buffer
        );
        // set texture options
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        // now store character for later use
        Character character = {
            texture, 
            glm::ivec2(face->glyph->bitmap.width, face->glyph->bitmap.rows),
            glm::ivec2(face->glyph->bitmap_left, face->glyph->bitmap_top),
            face->glyph->advance.x
        };
        Characters.insert(std::pair<char, Character>(c, character));
    }
    FT_Done_Face(face);
    FT_Done_FreeType(ft);
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
    entityx::Entity e2 = entities.create();
    //e2.assign<AudioSource>(new Sound("kick-trimmed.wav", true));
    e2.assign<SpriteVertices>(
        -25.0f, -25.0f, 0.0f, 0.0f,
         25.0f, -25.0f, 1.0f, 0.0f,
         25.0f,  25.0f, 1.0f, 1.0f,
        -25.0f,  25.0f, 0.0f, 1.0f,

        0,1,2,
        2,3,0
    );
    e2.assign<ShaderComp>("src/res/shaders/Basic.shader");
    e2.assign<TextureComp>("src/res/textures/platformChar_idle.png");
    e2.assign<Transform>(0.0f, 0.0f, 0.0f, 0, -969, 0, 1, 2);
    // e2.assign<Camera>((float)SCR_WIDTH / 2 * -1, (float)SCR_WIDTH / 2, (float)SCR_HEIGHT / 2 * -1, (float)SCR_HEIGHT / 2, -1.0f, 1.0f);

    entityx::Entity entity = Engine::getInstance().entities.create();

    entity.assign<SpriteVertices>(
        -50.0f, -50.0f, 0.0f, 0.0f,
         50.0f, -50.0f, 1.0f, 0.0f,
         50.0f,  50.0f, 1.0f, 1.0f,
        -50.0f,  50.0f, 0.0f, 1.0f,

        0,1,2,
        2,3,0
    );

    entity.assign<ShaderComp>("src/res/shaders/Basic.shader");
    entity.assign<TextureComp>("src/res/textures/Sport.png");
    // entity.assign<CustomScript>("src/CustomScripts/XmlExampleCustomScript.xml");

    entity.assign<Transform>(-100.0f, 0.0f, 0.0f, 90, 0, 0, 1);

    entityx::Entity e3 = entities.create();
    e3.assign<TextComp>("pls let me sleep", 5, 5, 1.0f, 0.5, 0.8f, 0.2f);
 
    //test for sound system
    // Entity e1 = entities.create();
    // e1.assign<AudioSource>(new Sound("Red Dead Redemption 2 - See the Fire in Your Eyes.mp3"));

    // float i = 800.0f;
    // float y = -300.0f;
    // float alpha = 0.01;

    // // iterate through all characters
    // string text = "Sans Undertale";
    // string::const_iterator c;

    float x = 25;
    float y = 25;
    float scale = 1;

    // if(FT_Init_FreeType(&ft)) {
    // fprintf(stderr, "Could not init freetype library\n");
    // return;
    // }

    // if(FT_New_Face(ft, "src/res/fonts/COMIC.TTF", 0, &face)) {
    // fprintf(stderr, "Could not open font\n");
    // return;
    // }

    // FT_Set_Pixel_Sizes(face, 0, 48);

    // // For some reason it can't seem to find the glyph
    // if(FT_Load_Char(face, 'a', FT_LOAD_RENDER)) {
    // fprintf(stderr, "Could not load character 'a'\n");
    // //return;
    // }
    // FT_GlyphSlot g = face->glyph;

    // GLuint tex;
    // glActiveTexture(GL_TEXTURE0);
    // glGenTextures(1, &tex);
    // glBindTexture(GL_TEXTURE_2D, tex);
    // glUniform1i(uniform_tex, 0);

    // glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    // glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

    // glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    // glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    // glGenBuffers(1, &vbo);
    // glEnableVertexAttribArray(attribute_coord);
    // glBindBuffer(GL_ARRAY_BUFFER, vbo);
    // glVertexAttribPointer(attribute_coord, 4, GL_FLOAT, GL_FALSE, 0, 0);

    // glm::mat4 projection = glm::ortho(0.0f, 800.0f, 0.0f, 600.0f);
    // GLuint VAO, VBO;
    // glGenVertexArrays(1, &VAO);
    // glGenBuffers(1, &VBO);
    // glBindVertexArray(VAO);
    // glBindBuffer(GL_ARRAY_BUFFER, VBO);
    // glBufferData(GL_ARRAY_BUFFER, sizeof(float) * 6 * 4, NULL, GL_DYNAMIC_DRAW);
    // glEnableVertexAttribArray(0);
    // glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 4 * sizeof(float), 0);
    // glBindBuffer(GL_ARRAY_BUFFER, 0);
    // glBindVertexArray(0);


    SceneManager::getInstance().start();
    // render loop
    // -----------
    while (!glfwWindowShouldClose(window))
    {   
        update();
        //RenderText(shader, "This is sample text", 25.0f, 25.0f, 1.0f, glm::vec3(0.5, 0.8f, 0.2f));
        //entityx::ComponentHandle<Transform> transform = e2.component<Transform>();

        //LEEEEERP
        //transform.get()->y = transform.get()->y * (1.0 - alpha) + i * alpha;

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

