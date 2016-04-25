#include "PassRegular.hpp"

#include <Engine/Renderer/Mesh/Mesh.hpp>
#include <Engine/Renderer/OpenGL/FBO.hpp>
#include <Engine/Renderer/Texture/Texture.hpp>
#include <Engine/Renderer/RenderTechnique/ShaderProgram.hpp>
#include <Engine/Renderer/RenderTechnique/ShaderProgramManager.hpp>

#include <iostream>

namespace Ra
{
    namespace Engine
    {

        PassRegular::PassRegular(const std::string& name, uint w, uint h, uint nTexIn, uint nTexOut, uint priority, std::string shader)
            : Pass(name, w, h, nTexIn, nTexOut, priority)
            , m_shadername(shader)
        {
            // generate output textures
            char c[2] = "A";
            for (auto& texout : m_texOut)
            {
                std::cout << m_name << " generating tex " << c << std::endl;
                texout.reset( new Texture(std::string(c), GL_TEXTURE_2D) );
                ++ c[0];
            }
        }

        PassRegular::~PassRegular()
        {
        }

        void PassRegular::init()
        {
            // initialize internal FBO
            m_fbo.reset( new FBO( FBO::Components(FBO::COLOR), m_width, m_height ));

            // get shader
            ShaderProgramManager* shaderMgr = ShaderProgramManager::getInstance();
            m_shader = shaderMgr->addShaderProgram(m_shadername, "../Shaders/Basic2D.vert.glsl", "../Shaders/" + m_shadername + ".frag.glsl");
        }

        void PassRegular::resizePass(uint w, uint h)
        {
            m_width  = w;
            m_height = h;
            resizePass();
        }

        void PassRegular::resizePass()
        {
            // resize output textures accordingly
            for (auto& texout : m_texOut)
            {
                texout->initGL(GL_RGBA32F, m_width, m_height, GL_RGBA, GL_FLOAT, nullptr);
            }

            // initiate, bind and configure the fbo
            m_fbo->bind();
            m_fbo->setSize( m_width, m_height );

            // when attaching textures, the pass will try to simply attach everybody
            // until the guaranteed number of attachments is reached
            int i = 0;
            while ((i < 8) && (i < m_nTexOut))
            {
                m_fbo->attachTexture(GL_COLOR_ATTACHMENT0 + i, m_texOut[i].get());
                ++ i;
            }

            m_fbo->unbind(true); // TODO(Hugo) remove when fbo bound status merged ?
        }

        void PassRegular::renderPass()
        {
            m_fbo->useAsTarget(m_width, m_height);

            GL_ASSERT( glViewport(0, 0, m_width, m_height) );
            GL_ASSERT( glDrawBuffers(1, buffers) );

            m_shader->bind();
            m_params.bind(m_shader);
            m_canvas->render();
        }

        std::shared_ptr<Texture> PassRegular::getInternTextures(uint i)
        {
            return std::shared_ptr<Texture>( nullptr );
        }

        Texture* PassRegular::getOut(uint slot)
        {
            return (slot < m_nTexOut) ? m_texOut[slot].get() : nullptr;
        }

    }
}
