#ifndef engine_h
#define engine_h
#pragma once

#include <entityx/entityx.h>
#include <chrono>
#include <map>
#include <string>
#include <glad/glad.h>
#include <glfw/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <functional>

using namespace entityx;

struct Character {
    unsigned int TextureID;  // ID handle of the glyph texture
    glm::ivec2   Size;       // Size of glyph
    glm::ivec2   Bearing;    // Offset from baseline to left/top of glyph
    unsigned int Advance;    // Offset to advance to next glyph
};

class Engine : public EntityX{
    public:
        static Engine& getInstance();

        void start();
        const unsigned int SCR_WIDTH = 960;
        const unsigned int SCR_HEIGHT = 540;
        GLFWwindow* window;
        Character getCharDetails(std::string::const_iterator c);

    private:
        explicit Engine();
        std::chrono::high_resolution_clock clock;
        std::chrono::milliseconds lastTime;
        std::chrono::milliseconds deltaTime;
        bool initialized = false;
        void initialize();
        void update();
        // GLint get_uniform(GLuint program, const char *name);
        // GLint get_attrib(GLuint program, const char *name);
        // GLuint create_program(const char *vertexfile, const char *fragmentfile);
        // GLuint create_shader(const char* filename, GLenum type);
        // void print_log(GLuint object);
        // char* file_read(const char* filename);
        //void render_text(const char *text, float x, float y, float sx, float sy);
    
    public:
        //remove other constructors
        Engine(Engine const&) = delete;
        void operator=(Engine const&) = delete;
};

// static methods implemented inside the header file
inline Engine& Engine::getInstance() {
    static Engine instance;
    return instance;
}

#endif /* engine_h */