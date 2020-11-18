#pragma once

#include "entityx/entityx.h"
#include <glad/glad.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <ft2build.h>
#include FT_FREETYPE_H

#include <iostream>
#include <map>

#include "../engine.h"
#include "../renderer.h"
#include "../vertexBuffer.h"
#include "../indexBuffer.h"
#include "../vertexArray.h"
#include "../shader.h"
#include "../vertexBufferLayout.h"
#include "../texture.h"

#include "../Components/SpriteVertices.h"
#include "../Components/ShaderComp.h"
#include "../Components/TextureComp.h"
#include "../Components/Camera.h"
#include "../Components/Transform.h"
#include "../Components/Active.h"

#define textWIDTH   256
#define textHEIGHT  256

// struct Character {
//     unsigned int TextureID;  // ID handle of the glyph texture
//     glm::ivec2   Size;       // Size of glyph
//     glm::ivec2   Bearing;    // Offset from baseline to left/top of glyph
//     unsigned int Advance;    // Offset to advance to next glyph
// };
//extern std::map<char, Character> Characters;

using namespace entityx;
class RenderingSystem : public System<RenderingSystem> {
    public:
        struct Character {
            unsigned int TextureID;  // ID handle of the glyph texture
            glm::ivec2   Size;       // Size of glyph
            glm::ivec2   Bearing;    // Offset from baseline to left/top of glyph
            unsigned int Advance;    // Offset to advance to next glyph
        };
        void update(EntityManager& es, EventManager& events, TimeDelta dt) override {
            //update loop
            renderer renderer;
            renderer.Clear();
            float lf, rf, bf, tf, dnp, dfp, x, y, z;

            ComponentHandle<Camera> mainCamera;
            ComponentHandle<Transform> mainTransform;
            ComponentHandle<TextComp> textComp;
            glm::mat4 proj, view;
            
            for (Entity entity : es.entities_with_components(mainTransform, mainCamera)) {
                // Do things with entity, position and direction.
                mainCamera = entity.component<Camera>();
                mainTransform = entity.component<Transform>();
                lf = mainCamera.get()->lf;
                rf = mainCamera.get()->rf;
                bf = mainCamera.get()->bf;
                tf = mainCamera.get()->tf;
                dnp = mainCamera.get()->dnp;
                dfp = mainCamera.get()->dfp;
                x = mainCamera.get()->x - mainTransform.get()->x;
                y = mainCamera.get()->y - mainTransform.get()->y;
                z = mainCamera.get()->z - mainTransform.get()->z;

                // Model matrix: defines position, rotation and scale of the vertices of the model in the world.
                // View matrix: defines position and orientation of the "camera".
                // Projection matrix: Maps what the "camera" sees to NDC, taking care of aspect ratio and perspective.
            
                //Orthographic projection between (-2 and 2 (x) / -1.5 and 1.5 (y) / -1 and 1 (z))
                //This is the view, if any of the positions are outside of this view, they won't be rendered
                //EG. if position x is -0.5 and ortho view is -2  - 2:
                //  then it will be a quarter of the way to the left since -0.5 is 1/4th of 2 which will make it 0.25
                //1:1 pixel mapping for 960x540 res:
                // EG: glm::mat4 proj = glm::ortho(0.0f, 960.0f, 0.0f, 540.0f, -1.0f, 1.0f);
                
                //Make projection matrix editable
                //Creates a matrix for an orthographic parallel viewing volume.

                //   LEFT, RIGHT, BOTTOM, TOP, ZNEAR, ZFAR
                proj = glm::ortho(lf, rf, bf, tf, dnp, dfp);
                view = glm::translate(glm::mat4(1.0f), glm::vec3(x, y, z));
                break;
            }
            es.each<SpriteVertices, ShaderComp, TextureComp, Transform, Active>([dt, renderer, proj, view](
                Entity entity, SpriteVertices &position, ShaderComp &shaderComp, TextureComp &textureComp,
                Transform &transformComp, Active &activeComp) {
                if (activeComp.isActive){
                    //For large objects just 1 vertex buffer and multiple index buffers for different material types
                    //create vertex buffer
                    float positions[] = {
                        //positions x //postions y  //texture coords
                        position.v0x, position.v0y, position.v0t1, position.v0t2,
                        position.v1x, position.v1y, position.v1t1, position.v1t2,
                        position.v2x, position.v2y, position.v2t1, position.v2t2,
                        position.v3x, position.v3y, position.v3t1, position.v3t2,
                    };

                    unsigned int indices[] = {
                        position.v0, position.v1, position.v2,
                        position.v3, position.v4, position.v5
                    };

                    vertexArray va;
                    vertexBuffer vb(positions, 4 * 4 * sizeof(float));

                    vertexBufferLayout layout;
                    layout.Push<float>(2);
                    layout.Push<float>(2);
                    va.AddBuffer(vb, layout);

                    //create index buffer
                    indexBuffer ib(indices, 6);

                    //Have to set uniforms to the same bind slot (default 0)
                    shader shader(shaderComp.filepath);
                    shader.Bind();
                    //shader.setUniforms4f("u_Color", 0.8f, 0.3f, 0.8f, 1.0f); //sets colours
                    //shader.setUniformsMat4f("u_MVP", mvp); // sets textures

                    texture Texture(textureComp.filepath); 
                    shader.setUniforms1i("u_Texture", 0);

                    /*
                    Shader binds program for the gpu to use and tells it what to do with data
                    VA = The Data itself.
                    VB = vertex data, positions, texture coords
                    IB = contains vertex indices
                    //Draw uses IB access VB and call shader program on all vertices individually
                    */
                    va.Unbind();
                    vb.Unbind();
                    ib.Unbind();
                    shader.Unbind();
                    
                    glm::vec3 translation((int)round(transformComp.x), (int)round(transformComp.y), (int)round(transformComp.z));

                    shader.Bind();
                    Texture.Bind();
                    glm::mat4 model = glm::translate(glm::mat4(1.0f), glm::vec3(translation));
                    model = glm::rotate(model, 3.141592f / 180 * transformComp.angle, glm::vec3(transformComp.rx, transformComp.ry, transformComp.rz)); // where x, y, z is axis of rotation (e.g. 0 1 0)
                    glm::mat4 mvp;
                    mvp = proj * view * model; 

                    shader.setUniformsMat4f("u_MVP", mvp);
                    renderer.Draw(va, ib, shader);
                }
            });
            for (Entity entity : es.entities_with_components(textComp))  {

                unsigned int VAO, VBO;
                glGenVertexArrays(1, &VAO);
                glGenBuffers(1, &VBO);
                glBindVertexArray(VAO);
                glBindBuffer(GL_ARRAY_BUFFER, VBO);
                glBufferData(GL_ARRAY_BUFFER, sizeof(float) * 6 * 4, NULL, GL_DYNAMIC_DRAW);
                glEnableVertexAttribArray(0);
                glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 4 * sizeof(float), 0);
                glBindBuffer(GL_ARRAY_BUFFER, 0);
                glBindVertexArray(0);

                shader shader("src/res/shaders/Glyph.shader");
                glm::mat4 model = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 0.0f, 1.0f));
                glm::mat4 mvp;
                mvp = proj * view * model; 
                shader.Bind();
                shader.setUniformsMat4f("u_MVP", mvp);
                shader.setUniforms4f("textColor", textComp.get()->colourR, textComp.get()->colourG, textComp.get()->colourB, 1.0f);
                //glUniformMatrix4fv(glGetUniformLocation(shader.m_RendererID, "projection"), 1, GL_FALSE, glm::value_ptr(proj));
                glActiveTexture(GL_TEXTURE0);
                glBindVertexArray(VAO);
                string text = textComp.get()->text;
                float scale = textComp.get()->scale;
                std::string::const_iterator c;
                for (c = text.begin(); c != text.end(); c++) {
                    auto ch = Engine::getInstance().getCharDetails(c);
                    float xpos = textComp.get()->x + ch.Bearing.x * scale;
                    float ypos = textComp.get()->y - (ch.Size.y - ch.Bearing.y) * scale;
                    cout<<*c<<"-"<<ch.TextureID << "-" <<textComp.get()->scale<<endl;
                    float w = ch.Size.x * scale;
                    float h = ch.Size.y * scale;
                    // // update VBO for each character
                    float vertices[6][4] = {
                        { xpos,     ypos + h,   0.0f, 0.0f },            
                        { xpos,     ypos,       0.0f, 1.0f },
                        { xpos + w, ypos,       1.0f, 1.0f },

                        { xpos,     ypos + h,   0.0f, 0.0f },
                        { xpos + w, ypos,       1.0f, 1.0f },
                        { xpos + w, ypos + h,   1.0f, 0.0f }           
                    };
                    // render glyph texture over quad
                    glBindTexture(GL_TEXTURE_2D, ch.TextureID);
                    //glUniform1i (glGetUniformLocation (shader.m_RendererID, "projection"), 0);
                    // update content of VBO memory
                    glBindBuffer(GL_ARRAY_BUFFER, VBO);
                    glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(vertices), vertices); 
                    glBindBuffer(GL_ARRAY_BUFFER, 0);
                    // render quad
                    glDrawArrays(GL_TRIANGLES, 0, 6);
                    // now advance cursors for next glyph (note that advance is number of 1/64 pixels)
                    x += (ch.Advance >> 6) * scale; // bitshift by 6 to get value in pixels (2^6 = 64)
                }
                cout<<"END"<<endl;
                glBindVertexArray(0);
                glBindTexture(GL_TEXTURE_2D, 0);



                // float val;
                // GameObject::ParseVal(obj, "fontsize", val);
                // int fontPtSize = (int)val;
                // GameObject::ParseVal(obj, "dpi", val);
                // int dpi = (int)val;

                // shader shader("src/res/shaders/Glyph.shader");
                // shader.Bind();
                // unsigned char *image = (unsigned char *)calloc(textWIDTH * textHEIGHT * 4, sizeof(unsigned char));
                // float fontPtSize = 14;
                // int dpi = 2;
                // FT_Library library;
                // FT_Error error = FT_Init_FreeType(&library);        // initialize library
                // FT_Face face;
                // //std::cout << "Font file: " << "src/res/fonts/arial.ttf" << std::endl;
                // error = FT_New_Face(library, "src/res/fonts/arial.ttf", 0, &face);   // create face object
                // error = FT_Set_Char_Size(face, fontPtSize * 64, 0, dpi, 0); // set character size
                // FT_GlyphSlot slot = face->glyph;
                
                // // (x,y) relative to the upper left corner
                // FT_Vector pen;
                // pen.x = 0;
                // pen.y = (textHEIGHT - (int)(0.2*256)) * 64;

                // const char *text = textComp.get()->text;
                // size_t numChars = strlen(text);
                // int n, tw = 0, th = 0, ww, hh;
                // for (n = 0; n < numChars; n++)
                // {
                //     FT_Set_Transform(face, NULL, &pen);
                //     error = FT_Load_Char(face, text[n], FT_LOAD_RENDER);
                //     if (!error)
                //     {
                //         ww = slot->bitmap_left + slot->bitmap.width;
                //         if ((ww < textWIDTH) && (tw < ww))
                //             tw = ww;
                //         hh = textHEIGHT - slot->bitmap_top + slot->bitmap.rows;
                //         if ((hh < textHEIGHT) && (th < hh))
                //             th = hh;
                //         pen.x += slot->advance.x;
                //         pen.y += slot->advance.y;
                //     }
                // }
                // int offset = 0;
                // // tinyxml2::XMLElement *align = "center";
                // // if (align)
                // // {
                // //     if (strcmp(align->GetText(), "centre") == 0)
                // //         offset = (textWIDTH - tw) / 2;
                // //     else if (strcmp(align->GetText(), "right") == 0)
                // //         offset = textWIDTH - tw;
                // // }
                // pen.x = 0;
                // pen.y = (textHEIGHT - (int)(0.2*256)) * 64;
                // for (n = 0; n < numChars; n++)
                // {
                //     // set transformation
                //     FT_Set_Transform(face, NULL, &pen);
                    
                //     // load glyph image into the slot (erase previous one)
                //     error = FT_Load_Char(face, text[n], FT_LOAD_RENDER);
                //     if (error)
                //         continue;   // ignore errors
                    
                //     // now, draw to our target surface (convert position)
                //     DrawBitmap(image, &slot->bitmap, slot->bitmap_left+offset,
                //             textHEIGHT-slot->bitmap_top);
                    
                //     // increment pen position
                //     pen.x += slot->advance.x;
                //     pen.y += slot->advance.y;
                // }
                
                // FT_Done_Face(face);
                // FT_Done_FreeType(library);
                // unsigned int textureId;
                // // Now transfer the internal bitmap to a GL texture
                // glGenTextures(1, &textureId);
                // glBindTexture(GL_TEXTURE_2D, textureId);
                // glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
                // glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, textWIDTH, textHEIGHT, 0, GL_RGBA, GL_UNSIGNED_BYTE, (void *)image);
                // glActiveTexture(GL_TEXTURE0);
                // glBindTexture(GL_TEXTURE_2D, textureId);
                // //SetUniformScalar(shader, shader.textureVar, 0);
                // glm::mat4 model = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 0.0f, 1.0f));
                // glm::mat4 mvp;
                // mvp = proj * view * model; 
                // shader.setUniformsMat4f("u_MVP", mvp);
                // shader.setUniforms4f("u_Color", textComp.get()->colourR, textComp.get()->colourG, textComp.get()->colourB, 1.0f);
                
                // free(image);
            
        }
    }
    // private:
    //     static void DrawBitmap(unsigned char *image, FT_Bitmap *bitmap, FT_Int x, FT_Int y)
    //     {
    //         FT_Int  i, j, p, q;
    //         FT_Int  x_max = x + bitmap->width;
    //         FT_Int  y_max = y + bitmap->rows;

            
    //         for ( i = x, p = 0; i < x_max; i++, p++ )
    //         {
    //             for ( j = y, q = 0; j < y_max; j++, q++ )
    //             {
    //                 if ( i < 0      || j < 0       ||
    //                     i >= textWIDTH || j >= textHEIGHT )
    //                     continue;
    //                 image[(textHEIGHT-j+1)*textWIDTH*4+i*4] = bitmap->buffer[q * bitmap->width + p];
    //                 image[(textHEIGHT-j+1)*textWIDTH*4+i*4+1] = bitmap->buffer[q * bitmap->width + p];
    //                 image[(textHEIGHT-j+1)*textWIDTH*4+i*4+2] = bitmap->buffer[q * bitmap->width + p];
    //                 image[(textHEIGHT-j+1)*textWIDTH*4+i*4+3] = 255;
    //             }
    //         }
    //     }
};