#pragma once

#include <vector>
#include "renderer.h"

struct vertexBufferElement
{
    unsigned int type;
    unsigned int count;
    unsigned char normalised;

    static unsigned int GetSizeOfType(unsigned int type)
    {
        switch(type)
        {
            case GL_FLOAT: return 4;
            case GL_UNSIGNED_INT: return 4;
            case GL_UNSIGNED_BYTE: return 1;
        }
        ASSERT(false);
        return 0;
    }
};

class vertexBufferLayout
{
    private:
        std::vector<vertexBufferElement> m_Elements;
        unsigned int m_Stride;
    public:
        vertexBufferLayout()
            :m_Stride(0) {}

        template<typename T> inline
        void Push(unsigned int count)
        {
            static_assert(true);
        }

        inline const std::vector<vertexBufferElement> GetElements() const { return m_Elements; }
        inline unsigned int GetStride() const { return m_Stride; }
};



template<> inline
void vertexBufferLayout::Push<float>(unsigned int count)
{
    m_Elements.push_back({ GL_FLOAT, count, GL_FALSE });
    m_Stride += count * vertexBufferElement::GetSizeOfType(GL_FLOAT);
}

template<> inline
void vertexBufferLayout::Push<unsigned int>(unsigned int count)
{
    m_Elements.push_back({ GL_UNSIGNED_INT, count, GL_FALSE });
    m_Stride += count * vertexBufferElement::GetSizeOfType(GL_UNSIGNED_INT);
}


template<> inline
void vertexBufferLayout::Push<unsigned char>(unsigned int count)
{
    m_Elements.push_back({ GL_UNSIGNED_BYTE, count, GL_TRUE });
    m_Stride += count * vertexBufferElement::GetSizeOfType(GL_UNSIGNED_BYTE);
}
