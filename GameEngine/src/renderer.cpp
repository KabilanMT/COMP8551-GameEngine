#include "renderer.h"
#include <iostream>

void GLClearError()
{
    while(glGetError() != glGetError());
}

bool GLLogCall( const char* function, const char* file, int line)
{
    while(GLenum error = glGetError())
    {
        std::cout << "[OpenGL Error] (" << error << "):" << function << " " << file << ":" << line << std::endl;
        return false;
    }
    return true;
}

void renderer::Clear() const
{
    GLCall(glClear(GL_COLOR_BUFFER_BIT));
}

void renderer::Draw(const vertexArray& va, const indexBuffer& ib, const shader& shader) const
{
        shader.Bind();
        va.Bind();
        ib.Bind();
        GLCall(glDrawElements(GL_TRIANGLES, ib.GetCount(), GL_UNSIGNED_INT, nullptr));
}