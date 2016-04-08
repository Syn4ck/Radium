#include "PassBlur.hpp"

//#include <iostream>

namespace Ra {
    namespace Engine {

        PassBlur::PassBlur(const std::string& name, uint w, uint h, uint nTexIn=1, uint nTexOut=1)
            : Pass(name, w, h, nTexIn, nTexOut)
        {
            m_pingPongSize = std::pow(2.0, Scalar(uint(std::log2(std::min(m_width, m_height)))));
        }

        PassBlur::~PassBlur()
        {
        }

        void PassBlur::initFbos()
        {
            // create internal FBOs
            m_fbo[FBO_MAIN].reset( new FBO( FBO::Components(FBO::COLOR), m_width, m_height ));
            m_fbo[FBO_BLUR].reset( new FBO( FBO::Components(FBO::COLOR), m_width / 8, m_height / 8 ));

            // actually create necessary intermediate textures (ping-pong here)
            //m_internalTextures[TEX_BLUR_PING].reset( new Texture( "Ping", GL_TEXTURE_2D ) );
            //m_internalTextures[TEX_BLUR_PONG].reset( new Texture( "Pong", GL_TEXTURE_2D ) );
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

            //m_internalTextures[TEX_BLUR_PING]->initGL(GL_RGBA32F, m_pingPongSize, m_pingPongSize, GL_RGBA, GL_FLOAT, nullptr);
            //m_internalTextures[TEX_BLUR_PONG]->initGL(GL_RGBA32F, m_pingPongSize, m_pingPongSize, GL_RGBA, GL_FLOAT, nullptr);

            // initiate, bind and configure the main fbo
            m_fbo[FBO_MAIN]->bind();
            m_fbo[FBO_MAIN]->setSize( m_width, m_height );
            m_fbo[FBO_MAIN]->attachTexture( GL_COLOR_ATTACHMENT0, m_texIn[TEX_COLOR] );
            m_fbo[FBO_MAIN]->attachTexture( GL_COLOR_ATTACHMENT1, m_texOut[TEX_BLURRED] );
            m_fbo[FBO_MAIN]->unbind( true );

            // initiate, bind and configure the ping-pong blurring fbo
            m_fbo[FBO_BLUR]->bind();
            m_fbo[FBO_BLUR]->setSize( m_width / 8, m_height / 8 );
            m_fbo[FBO_BLUR]->attachTexture( GL_COLOR_ATTACHMENT0, m_internalTextures[TEX_BLUR_PING].get() );
            m_fbo[FBO_BLUR]->attachTexture( GL_COLOR_ATTACHMENT1, m_internalTextures[TEX_BLUR_PONG].get() );
            m_fbo[FBO_BLUR]->unbind( true );

            GL_CHECK_ERROR;

            // Reset framebuffer state
            GL_ASSERT( glBindFramebuffer( GL_FRAMEBUFFER, 0 ) );

            GL_ASSERT( glDrawBuffer( GL_BACK ) );
            GL_ASSERT( glReadBuffer( GL_BACK ) );
        }

        void PassBlur::renderPass(ShaderProgramManager* shaderMgr, Mesh *screen)
        {
            const ShaderProgram* shader = nullptr;

            shader = shaderMgr->getShaderProgram("Blur");
            shader->bind();

            m_fbo[FBO_BLUR]->useAsTarget();
            GL_ASSERT( glViewport(0, 0, m_width / 8, m_height / 8) );

            for (uint i = 0; i < 2; ++i)
            {
                // X blur
                GL_ASSERT( glDrawBuffers(1, buffers + TEX_BLUR_PING) );
                shader->setUniform("color", m_internalTextures[TEX_BLUR_PONG].get(), 0);
                shader->setUniform("offset", Core::Vector2(8.0 / m_width, 0.0));
                screen->render();

                // Y blur
                GL_ASSERT( glDrawBuffers(1, buffers + TEX_BLUR_PONG) );
                shader->setUniform("color", m_internalTextures[TEX_BLUR_PING].get(), 0);
                shader->setUniform("offset", Core::Vector2(0.0, 8.0 / m_height));
                screen->render();
            }

            // bring back to the output texture
        }

        std::shared_ptr<Texture> PassBlur::getInternTextures(uint i)
        {
            if (i < TEX_INTERNAL_COUNT)
                return m_internalTextures[i];
            else
                return m_internalTextures[0];  // FIXME(Hugo) what an error management
        }

    }
}

