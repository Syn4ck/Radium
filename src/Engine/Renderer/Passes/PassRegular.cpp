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

        PassRegular::PassRegular(const std::string& name, uint w, uint h, uint nbIn, uint nbOut, std::string shader)
            : Pass(name, w, h, nbIn, nbOut)
            , m_shadername(shader)
        {
        }

        PassRegular::~PassRegular()
        {
        }

        void PassRegular::init()
        {
            // initialize internal FBO
            m_fbo.reset( new FBO( FBO::Components(FBO::COLOR), m_width, m_height ));

            // generate outputs
            for (auto& out : m_nameOut)
            {
                // only for textures
                if (out.second == PARAM_TEX)
                {
                    m_outputs.push_back( std::unique_ptr<Texture>( new Texture(out.first, GL_TEXTURE_2D)) );
                    m_paramOut.addParameter( out.first.c_str(), m_outputs.back().get() );
                }
            }

            // get shader
            ShaderProgramManager* shaderMgr = ShaderProgramManager::getInstance();
            m_shader = shaderMgr->addShaderProgram(m_shadername, "../Shaders/Basic2D.vert.glsl", "../Shaders/" + m_shadername + ".frag.glsl");
        }

        void PassRegular::resizePass(uint w, uint h)
        {
            m_width  = ((float) w * m_wModifier);
            m_height = ((float) h * m_hModifier);
            resizePass();
        }

        void PassRegular::resizePass()
        {
            // resize output textures accordingly
            for (auto& texout : m_outputs)
            {
                texout->initGL(GL_RGBA32F, m_width, m_height, GL_RGBA, GL_FLOAT, nullptr);
            }

            // initiate, bind and configure the fbo
            m_fbo->bind();
            m_fbo->setSize( m_width, m_height );

            // when attaching textures, the pass will try to simply attach everybody
            // until the guaranteed number of attachments is reached
            int i = 0, nbouts = m_outputs.size();
            while ((i < 8) && (i < nbouts))
            {
                m_fbo->attachTexture(GL_COLOR_ATTACHMENT0 + i, m_outputs[i].get());
                ++ i;
            }

            m_fbo->unbind(true); // TODO(Hugo) remove when fbo bound status merged ?*/
        }

        void PassRegular::renderPass()
        {
            m_fbo->useAsTarget(m_width, m_height);

            GL_ASSERT( glViewport(0, 0, m_width, m_height) );
            GL_ASSERT( glDrawBuffers(1, buffers) );

            m_shader->bind();
            m_paramIn.bind(m_shader);
            m_canvas->render();
        }

    }
}
