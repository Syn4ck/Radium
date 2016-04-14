#include "PassPingPong.hpp"

namespace Ra
{
    namespace Engine
    {

        PassPingPong::PassPingPong(const std::string& name, uint w, uint h, uint nTexIn, uint nTexOut, uint priority,
                                   uint loop, const ShaderProgram* pingpong, const ShaderProgram* pongping)
            : Pass(name, w, h, nTexIn, nTexOut, nullptr, priority)
            , m_loop( loop )
        {
            // textures
            m_texOut   [TEX_PONG].reset( new Texture("Pong", GL_TEXTURE_2D) );
            m_texIntern[TEX_PING].reset( new Texture("Ping", GL_TEXTURE_2D) );

            // shaders
            m_shader[SHADER_PINGPONG] = pingpong;
            m_shader[SHADER_PONGPING] = pongping;
        }

        PassPingPong::~PassPingPong()
        {
        }

        void PassPingPong::init()
        {
            // internal FBO
            m_fbo.reset( new FBO( FBO::Components(FBO::COLOR), m_width, m_height ));
        }

        void PassPingPong::resizePass(uint w, uint h)
        {
            m_width  = w;
            m_height = h;
            resizePass();
        }

        void PassPingPong::resizePass()
        {
            m_texIntern[TEX_PING]->initGL(GL_RGBA32F, m_width, m_height, GL_RGBA, GL_FLOAT, nullptr);
            m_texOut   [TEX_PONG]->initGL(GL_RGBA32F, m_width, m_height, GL_RGBA, GL_FLOAT, nullptr);

            m_fbo->bind();
            m_fbo->setSize(m_width, m_height);
            m_fbo->attachTexture( GL_COLOR_ATTACHMENT0, m_texIn[0] );
            m_fbo->attachTexture( GL_COLOR_ATTACHMENT1, m_texIntern[TEX_PING].get() );
            m_fbo->attachTexture( GL_COLOR_ATTACHMENT2, m_texOut[TEX_PONG].get() );
            m_fbo->unbind( true );

            GL_CHECK_ERROR;

            // reset GL state
            GL_ASSERT( glBindFramebuffer( GL_FRAMEBUFFER, 0 ) );

            GL_ASSERT( glDrawBuffer( GL_BACK ) );
            GL_ASSERT( glReadBuffer( GL_BACK ) );
        }

        void PassPingPong::renderPass()
        {
            m_fbo->useAsTarget(m_width, m_height);

            GL_ASSERT( glViewport(0, 0, m_width, m_height) );

            // first write to pong
            GL_ASSERT( glDrawBuffers(1, buffers + 2) );
            m_shader[SHADER_PINGPONG]->bind();
            m_shader[SHADER_PINGPONG]->setUniform("color", m_texIn[0], 0);
            m_shader[SHADER_PINGPONG]->setUniform("offset", Core::Vector2(1.0 / m_width, 0.0));
            m_shader[SHADER_PINGPONG]->setUniform("wView", m_width);
            m_shader[SHADER_PINGPONG]->setUniform("hView", m_height);
            m_canvas->render();

            // actually do the ping-pong
            for (uint i = 0; i < m_loop; ++i)
            {
                // pong->ping
                GL_ASSERT( glDrawBuffers(1, buffers + 1) );
                //if (m_shader[SHADER_PINGPONG] != m_shader[SHADER_PONGPING])
                    //m_shader[SHADER_PONGPING]->bind();
                m_shader[SHADER_PINGPONG]->setUniform("color", m_texIntern[TEX_PONG].get(), 0);
                m_shader[SHADER_PINGPONG]->setUniform("offset", Core::Vector2(0.0, 1.0 / m_height));
                m_shader[SHADER_PINGPONG]->setUniform("wView", m_width);
                m_shader[SHADER_PINGPONG]->setUniform("hView", m_height);
                m_canvas->render();

                // ping->pong
                GL_ASSERT( glDrawBuffers(1, buffers + 2) );
                //if (m_shader[SHADER_PINGPONG] != m_shader[SHADER_PONGPING])
                    //m_shader[SHADER_PINGPONG]->bind();
                m_shader[SHADER_PINGPONG]->setUniform("color", m_texIntern[TEX_PING].get(), 0);
                m_shader[SHADER_PINGPONG]->setUniform("offset", Core::Vector2(1.0 / m_width, 0.0));
                m_shader[SHADER_PINGPONG]->setUniform("wView", m_width);
                m_shader[SHADER_PINGPONG]->setUniform("hView", m_height);
                m_canvas->render();
            }
        }

        std::shared_ptr<Texture> PassPingPong::getInternTextures(uint i)
        {
            if (i < TEX_INTERNAL_COUNT)
                return m_texIntern[i];
            else
                return m_texIntern[0];
        }
    }
}
