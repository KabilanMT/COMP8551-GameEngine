#pragma once

#include "entityx/entityx.h"
#include <glad/glad.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include <iostream>
#include "../renderer.h"
#include "../vertexBuffer.h"
#include "../indexBuffer.h"
#include "../vertexArray.h"
#include "../shader.h"
#include "../vertexBufferLayout.h"
#include "../texture.h"

#include "../Components/Position.h"
#include "../Components/ShaderComp.h"
#include "../Components/TextureComp.h"
#include "../Components/Translation.h"
#include "../Components/Rotate.h"
#include "../Components/Camera.h"

using namespace entityx;
class RenderingSystem : public System<RenderingSystem> {
    public:
        void update(EntityManager& es, EventManager& events, TimeDelta dt) override {
            //update loop
            renderer renderer;
            renderer.Clear();
            es.each<Position, ShaderComp, TextureComp, Translation, Rotate, Camera>([dt, renderer](
                Entity entity, Position &position, ShaderComp &shaderComp, TextureComp &textureComp,
                Translation &translationComp, Rotate &rotateComp, Camera &cameraComp) {
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

                //Orthographic projection between (-2 and 2 (x) / -1.5 and 1.5 (y) / -1 and 1 (z))
                //This is the view, if any of the positions are outside of this view, they won't be rendered
                //EG. if position x is -0.5 and ortho view is -2  - 2:
                //  then it will be a quarter of the way to the left since -0.5 is 1/4th of 2 which will make it 0.25
                //1:1 pixel mapping for 960x540 res:
                // EG: 
                //      glm::mat4 proj = glm::ortho(0.0f, 960.0f, 0.0f, 540.0f, -1.0f, 1.0f);
                
                /*
                Model matrix: defines position, rotation and scale of the vertices of the model in the world.
                View matrix: defines position and orientation of the "camera".
                Projection matrix: Maps what the "camera" sees to NDC, taking care of aspect ratio and perspective.
                */

                //Make projection matrix editable
                glm::mat4 proj = glm::ortho(cameraComp.lf, cameraComp.rf, cameraComp.bf, cameraComp.tf, cameraComp.dnp, cameraComp.dfp);
                glm::mat4 view = glm::translate(glm::mat4(1.0f), glm::vec3(0, 0, 0));
                //Need to eventually move this model view out in to imgui

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

                glm::vec3 translation(translationComp.x, translationComp.y, translationComp.z);

                shader.Bind();
                Texture.Bind();
                glm::mat4 model = glm::translate(glm::mat4(1.0f), glm::vec3(translation));

                model = glm::rotate(model, 3.141592f / 180 * rotateComp.angle, glm::vec3(rotateComp.x, rotateComp.y, rotateComp.z)); // where x, y, z is axis of rotation (e.g. 0 1 0)

                glm::mat4 mvp = proj * view * model; 
                shader.setUniformsMat4f("u_MVP", mvp);
                renderer.Draw(va, ib, shader);
            });
        }
};
