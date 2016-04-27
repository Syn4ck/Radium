#include "PassRedux.hpp"

namespace Ra
{
    namespace Engine
    {

        PassRedux::PassRedux(const std::string& name, uint w, uint h, uint nTexIn, uint nTexOut, uint ratio,
                             const std::string& shader)
            : Pass(name, w, h, nTexIn, nTexOut)
            , m_ratio( ratio )
            , m_shadername(shader)
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
            m_fbo.reset( new FBO( FBO::Components(FBO::COLOR), 1, 1 ));

            // get shader
            ShaderProgramManager* shaderMgr = ShaderProgramManager::getInstance();
            m_shader = shaderMgr->addShaderProgram(m_shadername, "../Shaders/Basic2D.vert.glsl", "../Shaders/" + m_shadername + ".frag.glsl");
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

            m_texOut[TEX_PING]->initGL(GL_RGBA32F, m_pingPongSize, m_pingPongSize, GL_RGBA, GL_FLOAT, nullptr);
            m_texOut[TEX_PONG]->initGL(GL_RGBA32F, m_pingPongSize, m_pingPongSize, GL_RGBA, GL_FLOAT, nullptr);

            m_fbo->bind();
            m_fbo->setSize(m_pingPongSize, m_pingPongSize);
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
            // locally load the drawscreen shader
            ShaderProgramManager* shaderMgr = ShaderProgramManager::getInstance();
            const ShaderProgram* drawscreen = shaderMgr->getShaderProgram("DrawScreen");

            m_fbo->useAsTarget();

            uint size  = m_pingPongSize;

            // first write to ping
            GL_ASSERT( glDrawBuffers(1, buffers + 1) );
            GL_ASSERT( glViewport(0, 0, size, size) );

            // bring the texture to internal fbo
            drawscreen->bind();
            m_params.bind(drawscreen);
            m_canvas->render();

            m_pingpong = 0;

            // the actual redux loop
            while (size != 1)
            {
                size /= m_ratio;

                ++ m_pingpong %= 2;

                GL_ASSERT( glViewport(0, 0, size, size) );
                GL_ASSERT( glDrawBuffers(1, buffers + 1 + m_pingpong) );
                m_shader->bind();
                m_shader->setUniform("color", m_texOut[(m_pingpong+1)%2].get());
                m_canvas->render();
            }

            // write back to the place specified by caller
            GL_ASSERT( glDrawBuffers(1, buffers + m_parentAttachOffset) );
            GL_ASSERT( glViewport(0, 0, m_width, m_height) );

            // bring the result back to parent fbo
            m_parentFbo->useAsTarget();
            drawscreen->bind();
            drawscreen->setUniform("screenTexture", m_texOut[(m_pingpong+1)%2].get());
            m_canvas->render();
        }

        std::shared_ptr<Texture> PassRedux::getInternTextures(uint i) const
        {
            return std::shared_ptr<Texture> (nullptr);
        }

        Texture* PassRedux::getOut(uint slot) const
        {
            return m_texOut[(m_pingpong+1)%2].get();
        }

        void PassRedux::setReturningBuffer(FBO* fbo, uint attachOffset)
        {
            m_parentFbo = fbo;
            m_parentAttachOffset = attachOffset;
        }

    }
}
