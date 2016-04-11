#include "PassLuminance.hpp"

#include <iostream>

namespace Ra {
    namespace Engine {

        PassLuminance::PassLuminance(const std::string& name, uint w, uint h, uint nTexIn=1, uint nTexOut=1)
            : Pass(name, w, h, nTexIn, nTexOut)
        {
            // internal
            m_internalTextures[TEX_PING].reset( new Texture("intern_Ping", GL_TEXTURE_2D) );
            m_internalTextures[TEX_PONG].reset( new Texture("intern_Pong", GL_TEXTURE_2D) );

            // output
            m_texOut[TEX_LUM].reset( new Texture("Lum", GL_TEXTURE_2D) );
        }

        PassLuminance::~PassLuminance()
        {
        }

        void PassLuminance::initFbos()
        {
            // create internal FBOs
            m_fbo[FBO_MAIN].reset( new FBO( FBO::Components(FBO::COLOR), m_width, m_height ));
            m_fbo[FBO_PING_PONG].reset( new FBO( FBO::Components(FBO::COLOR), 1, 1 ));
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

            // resize textures
            m_internalTextures[TEX_PING]->initGL(GL_RGBA32F, m_pingPongSize, m_pingPongSize, GL_RGBA, GL_FLOAT, nullptr);
            m_internalTextures[TEX_PONG]->initGL(GL_RGBA32F, m_pingPongSize, m_pingPongSize, GL_RGBA, GL_FLOAT, nullptr);
            m_texOut[TEX_LUM]->initGL(GL_RGBA32F, m_width, m_height, GL_RGBA, GL_FLOAT, nullptr);

            // initiate, bind and configure the main fbo
            m_fbo[FBO_MAIN]->bind();
            m_fbo[FBO_MAIN]->setSize( m_width, m_height );
            m_fbo[FBO_MAIN]->attachTexture( GL_COLOR_ATTACHMENT0, m_texIn[TEX_HDR] );
            m_fbo[FBO_MAIN]->attachTexture( GL_COLOR_ATTACHMENT1, m_texOut[TEX_LUM].get() );
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

            // first apply the Luminance shader to the input texture (HDR) and write to output (LUM)
            m_fbo[FBO_MAIN]->useAsTarget( m_width, m_height );

            GL_ASSERT( glDrawBuffers(1, buffers + 1) );
            shader = shaderMgr->getShaderProgram("Luminance");

            shader->bind();
            shader->setUniform("hdr", m_texIn[TEX_HDR], 0);
            screen->render();

            // now that the LUM texture contains luminance(HDR), ping-pong with MinMax
            m_fbo[FBO_PING_PONG]->useAsTarget(); // prepare for ping-pong/redux combo

            uint size = m_pingPongSize;

            // first draw on PING to start
            GL_ASSERT( glDrawBuffers(1, buffers) );
            GL_ASSERT( glViewport(0, 0, size, size) );
            shader = shaderMgr->getShaderProgram("DrawScreen");
            shader->bind();
            shader->setUniform( "screenTexture", m_texOut[TEX_LUM].get(), 0);
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

            // then draw the final result to LUM, whom (0,0) texel contains min, max
            m_fbo[FBO_MAIN]->useAsTarget();

            GL_ASSERT( glDrawBuffers(1, buffers + 1) );
            GL_ASSERT( glViewport(0, 0, m_width, m_height) );

            shader = shaderMgr->getShaderProgram("DrawScreen");
            shader->bind();
            shader->setUniform( "screenTexture", m_internalTextures[TEX_PING + ((ping+1)%2)].get(), 0 );
            screen->render();
        }

        std::shared_ptr<Texture> PassLuminance::getInternTextures(uint i)
        {
            if (i < TEX_INTERNAL_COUNT)
                return m_internalTextures[i];
            else
                return m_internalTextures[0];
        }

    }
}

