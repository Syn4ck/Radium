#include "PassBlur.hpp"

namespace Ra {
    namespace Engine {

        PassBlur::PassBlur(const std::string& name, uint w, uint h, uint nTexIn, uint nTexOut,
                           Mesh* canvas, uint priority, uint amount, uint box)
            : Pass(name, w, h, nTexIn, nTexOut, canvas, priority)
            , m_amount(amount)
            , m_boxfactor(box)
        {
            // internal textures
            m_texIntern[TEX_BLUR_PING].reset( new Texture( "Ping", GL_TEXTURE_2D ) );
            m_texIntern[TEX_BLUR_PONG].reset( new Texture( "Pong", GL_TEXTURE_2D ) );

            // output texture
            m_texOut[TEX_BLURRED].reset( new Texture( "Blurred", GL_TEXTURE_2D ) );
        }

        PassBlur::~PassBlur()
        {
        }

        void PassBlur::init()
        {
            // create internal FBOs
            m_fbo[FBO_MAIN].reset( new FBO( FBO::Components(FBO::COLOR), m_width, m_height ));

            // load shader
            ShaderProgramManager* shaderMgr = ShaderProgramManager::getInstance();
            m_shader[SHADER_DRAWSCREEN] = shaderMgr->getShaderProgram("DrawScreen");
            m_shader[SHADER_BLUR] = shaderMgr->addShaderProgram("Blur", "../Shaders/Basic2D.vert.glsl", "../Shaders/Blur.frag.glsl");
        }

        void PassBlur::resizePass(uint w, uint h)
        {
            m_width  = w;
            m_height = h;
            resizePass();
        }

        void PassBlur::resizePass()
        {
            m_pingPongSize = std::pow(2.0, Scalar(uint(std::log2(std::min(m_width, m_height)))));

            // resize textures to fit the viewport
            m_texIntern[TEX_BLUR_PING]->initGL(GL_RGBA32F, m_width, m_height, GL_RGBA, GL_FLOAT, nullptr);
            m_texIntern[TEX_BLUR_PONG]->initGL(GL_RGBA32F, m_width, m_height, GL_RGBA, GL_FLOAT, nullptr);
            m_texOut[TEX_BLURRED]->initGL(GL_RGBA32F, m_width, m_height, GL_RGBA, GL_FLOAT, nullptr);

            // initiate, bind and configure the main fbo
            m_fbo[FBO_MAIN]->bind();
            m_fbo[FBO_MAIN]->setSize( m_width, m_height );
            m_fbo[FBO_MAIN]->attachTexture( GL_COLOR_ATTACHMENT0, m_texIn[TEX_COLOR] );
            m_fbo[FBO_MAIN]->attachTexture( GL_COLOR_ATTACHMENT1, m_texOut[TEX_BLURRED].get() );
            m_fbo[FBO_MAIN]->attachTexture( GL_COLOR_ATTACHMENT2, m_texIntern[TEX_BLUR_PING].get() );
            m_fbo[FBO_MAIN]->attachTexture( GL_COLOR_ATTACHMENT3, m_texIntern[TEX_BLUR_PONG].get() );
            m_fbo[FBO_MAIN]->unbind( true );

            GL_CHECK_ERROR;

            // Reset framebuffer state
            GL_ASSERT( glBindFramebuffer( GL_FRAMEBUFFER, 0 ) );

            GL_ASSERT( glDrawBuffer( GL_BACK ) );
            GL_ASSERT( glReadBuffer( GL_BACK ) );
        }

        void PassBlur::renderPass()
        {
            m_fbo[FBO_MAIN]->useAsTarget(m_width, m_height);
            GL_ASSERT( glViewport(0, 0, m_width, m_height) );

            // first copy the in-texture in PING
            GL_ASSERT( glDrawBuffers(1, buffers + 2) );            
            m_shader[SHADER_DRAWSCREEN]->bind();
            m_shader[SHADER_DRAWSCREEN]->setUniform("screenTexture", m_texIn[TEX_COLOR], 0);
            m_canvas->render();

            m_shader[SHADER_BLUR]->bind();

            for (uint i = 0; i < m_amount; ++i)
            {
                // X blur | ping -> pong
                GL_ASSERT( glDrawBuffers(1, buffers + 3) );
                m_shader[SHADER_BLUR]->setUniform("color", m_texIntern[TEX_BLUR_PING].get(), 0);
                m_shader[SHADER_BLUR]->setUniform("offset", Core::Vector2(1.0 / m_width, 0.0));
                m_shader[SHADER_BLUR]->setUniform("wView", (int)m_width);
                m_shader[SHADER_BLUR]->setUniform("hView", (int)m_height);
                m_canvas->render();

                // Y blur | pong -> ping
                GL_ASSERT( glDrawBuffers(1, buffers + 2) );
                m_shader[SHADER_BLUR]->setUniform("color", m_texIntern[TEX_BLUR_PONG].get(), 0);
                m_shader[SHADER_BLUR]->setUniform("offset", Core::Vector2(0.0, 1.0 / m_height));
                m_shader[SHADER_BLUR]->setUniform("wView", (int)m_width);
                m_shader[SHADER_BLUR]->setUniform("hView", (int)m_height);
                m_canvas->render();
            }

            // bring back to the output texture
            GL_ASSERT( glDrawBuffers(1, buffers + 1) );
            m_shader[SHADER_DRAWSCREEN]->bind();
            m_shader[SHADER_DRAWSCREEN]->setUniform("screenTexture", m_texIntern[TEX_BLUR_PING].get(), 0);
            m_canvas->render();
        }

        std::shared_ptr<Texture> PassBlur::getInternTextures(uint i)
        {
            if (i < TEX_INTERNAL_COUNT)
                return m_texIntern[i];
            else
                return m_texIntern[0];  // default. TODO(Hugo) change this ugliness
        }

    }
}

