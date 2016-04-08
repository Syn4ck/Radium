#include "PassLuminance.hpp"

#include <iostream>

namespace Ra {
    namespace Engine {

        PassLuminance::PassLuminance(const std::string& name, uint w, uint h, uint nTexIn=1, uint nTexOut=1)
            : Pass(name, w, h, nTexIn, nTexOut)
        {
            m_pingPongSize = std::pow(2.0, Scalar(uint(std::log2(std::min(m_width, m_height)))));
        }

        PassLuminance::~PassLuminance()
        {
        }

        void PassLuminance::initFbos()
        {
            // create internal FBOs
            m_fbo[FBO_MAIN].reset( new FBO( FBO::Components(FBO::COLOR), m_width, m_height ));
            m_fbo[FBO_PING_PONG].reset( new FBO( FBO::Components(FBO::COLOR), 1, 1 ));

            // actually create necessary intermediate textures (ping-pong here)
            m_internalTextures[TEX_PING].reset( new Texture( "Pong", GL_TEXTURE_2D ) );
            m_internalTextures[TEX_PONG].reset( new Texture( "Ping", GL_TEXTURE_2D ) );
        }

        void PassLuminance::resizePass(uint w, uint h)
        {
            m_width  = w;
            m_height = h;
            resizePass();
        }

        void PassLuminance::resizePass()
        {
            m_pingPongSize = std::pow(2.0, Scalar(uint(std::log2(std::min(m_width, m_height)))));

            // m_texOut[TEX_LUM]->initGL(GL_RGBA32F, m_width, m_height, GL_RGBA, GL_FLOAT, nullptr);
            // m_internalTextures[TEX_PING]->initGL(GL_RGBA32F, m_pingPongSize, m_pingPongSize, GL_RGBA, GL_FLOAT, nullptr);
            // m_internalTextures[TEX_PONG]->initGL(GL_RGBA32F, m_pingPongSize, m_pingPongSize, GL_RGBA, GL_FLOAT, nullptr);

            // initiate, bind and configure the main fbo
            m_fbo[FBO_MAIN]->bind();
            m_fbo[FBO_MAIN]->setSize( m_width, m_height );
            m_fbo[FBO_MAIN]->attachTexture( GL_COLOR_ATTACHMENT0, m_texIn[TEX_LIT] );
            m_fbo[FBO_MAIN]->attachTexture( GL_COLOR_ATTACHMENT1, m_texOut[TEX_LUM] );
            m_fbo[FBO_MAIN]->unbind( true );

            // initiate, bind and configure the ping-pong fbo
            m_fbo[FBO_PING_PONG]->bind();
            m_fbo[FBO_PING_PONG]->setSize( m_width, m_height );
            m_fbo[FBO_PING_PONG]->attachTexture( GL_COLOR_ATTACHMENT0, m_internalTextures[TEX_PING].get() );
            m_fbo[FBO_PING_PONG]->attachTexture( GL_COLOR_ATTACHMENT1, m_internalTextures[TEX_PONG].get() );
            m_fbo[FBO_PING_PONG]->unbind( true );

            GL_CHECK_ERROR;

            // Reset framebuffer state
            GL_ASSERT( glBindFramebuffer( GL_FRAMEBUFFER, 0 ) );

            GL_ASSERT( glDrawBuffer( GL_BACK ) );
            GL_ASSERT( glReadBuffer( GL_BACK ) );
        }

        void PassLuminance::renderPass(ShaderProgramManager* shaderMgr, Mesh *screen)
        {
            const ShaderProgram* shader = nullptr;

            // get per pixel luminance
            m_fbo[FBO_MAIN]->useAsTarget( m_width, m_height );

            GL_ASSERT( glDrawBuffers(1, buffers + 1) ); // draw on TEX_LUM (out)
            shader = shaderMgr->getShaderProgram("Luminance");

            shader->bind();
            shader->setUniform("hdr", m_texIn[TEX_LIT], 0); // set TEX_LIT as uniform source
            screen->render();


            m_fbo[FBO_PING_PONG]->useAsTarget(); // prepare for ping-pong

            uint size = m_pingPongSize;

            glDrawBuffers(1, buffers); // draw on ping first
            glViewport(0, 0, size, size);
            shader = shaderMgr->getShaderProgram("DrawScreen");
            shader->bind();
            shader->setUniform( "screenTexture", m_texOut[TEX_LUM], 0);
            screen->render();

            // ping-pong to min/max and avg
            shader = shaderMgr->getShaderProgram("MinMax");
            shader->bind();
            uint ping = 0;
            while (size != 1)
            {
                size /= 2;
                GL_ASSERT( glDrawBuffers(1, buffers + (ping + 1)%2) );
                GL_ASSERT( glViewport(0, 0, size, size) );

                shader->setUniform("color", m_internalTextures[TEX_PING + ping].get(), 0);
                screen->render();

                ++ ping %= 2;
            }

            // now the right texture should be outputted (verb from wikipedia)
            m_texOut[TEX_LUM] = m_internalTextures[TEX_PING + ((ping+1)%2)].get();
        }

        std::shared_ptr<Texture> PassLuminance::getInternTextures(uint i)
        {
            if (i < TEX_INTERNAL_COUNT)
                return m_internalTextures[i];
            else
                return m_internalTextures[0];  // FIXME(Hugo) what an error management
        }

    }
}

