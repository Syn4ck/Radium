#include "PassRedux.hpp"

namespace Ra
{
    namespace Engine
    {

        PassRedux::PassRedux(const std::string& name, uint w, uint h, uint nTexIn, uint nTexOut, Mesh* canvas, uint priority, uint ratio)
            : Pass(name, w, h, nTexIn, nTexOut, canvas, priority)
            , m_ratio   ( ratio )
            , m_pingpong( 0 )
        {
            // textures
            m_texOut[TEX_PING].reset( new Texture("Ping", GL_TEXTURE_2D) );
            m_texOut[TEX_PONG].reset( new Texture("Pong", GL_TEXTURE_2D) );
        }

        PassRedux::~PassRedux()
        {
        }

        void PassRedux::init()
        {
            // internal FBO
            m_fbo.reset( new FBO( FBO::Components(FBO::COLOR), m_width, m_height ));
        }

        void PassRedux::resizePass(uint w, uint h)
        {
            m_width  = w;
            m_height = h;
            resizePass();
        }

        void PassRedux::resizePass()
        {
            m_pingPongSize = std::pow(2.0, Scalar(uint(std::log2(std::min(m_width, m_height)))));

            m_texOut[TEX_PING]->initGL(GL_RGBA32F, m_width, m_height, GL_RGBA, GL_FLOAT, nullptr);
            m_texOut[TEX_PONG]->initGL(GL_RGBA32F, m_width, m_height, GL_RGBA, GL_FLOAT, nullptr);

            m_fbo->bind();
            m_fbo->setSize(m_width, m_height);
            m_fbo->attachTexture( GL_COLOR_ATTACHMENT0, m_texIn[0] );
            m_fbo->attachTexture( GL_COLOR_ATTACHMENT1, m_texOut[TEX_PING].get() );
            m_fbo->attachTexture( GL_COLOR_ATTACHMENT2, m_texOut[TEX_PONG].get() );
            m_fbo->unbind( true );

            GL_CHECK_ERROR;

            // reset GL state
            GL_ASSERT( glBindFramebuffer( GL_FRAMEBUFFER, 0 ) );

            GL_ASSERT( glDrawBuffer( GL_BACK ) );
            GL_ASSERT( glReadBuffer( GL_BACK ) );
        }

        void PassRedux::renderPass()
        {
            m_fbo->useAsTarget(m_width, m_height);

            GL_ASSERT( glViewport(0, 0, m_width, m_height) );

            uint size  = m_pingPongSize;

            // first write to ping
            GL_ASSERT( glDrawBuffers(1, buffers + 1) );
            m_shader->bind();
            m_params->bind(m_shader);
            m_shader->setUniform("color",  m_texIn[0], 0);
            m_canvas->render();

            m_pingpong = 0;

            // reduuuuuuuuuuuuuuuux
            while (size != 1)
            {
                size /= m_ratio;

                ++ m_pingpong %= 2;

                GL_ASSERT( glDrawBuffers(1, buffers + 1 + m_pingpong) );
                m_shader->bind();
                m_params->bind(m_shader);
                m_shader->setUniform("color", m_texOut[((m_pingpong + 1) % 2)].get(), 0);
                m_canvas->render();
            }
        }

        std::shared_ptr<Texture> PassRedux::getInternTextures(uint i)
        {
            return std::shared_ptr<Texture> (nullptr);
        }

        Texture* PassRedux::getOut(uint slot)
        {
            return m_texOut[m_pingpong].get();
        }

        void PassRedux::setShader(const ShaderProgram* shader, RenderParameters* params)
        {
            m_shader = shader;
            m_params = params;
        }

    }
}
