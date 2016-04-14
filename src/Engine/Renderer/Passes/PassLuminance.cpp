#include "PassLuminance.hpp"

#include <iostream>

namespace Ra {
    namespace Engine {

        PassLuminance::PassLuminance(const std::string& name, uint w, uint h, uint nTexIn, uint nTexOut,
                                     Mesh* canvas, uint priority)
            : Pass(name, w, h, nTexIn, nTexOut, canvas, priority)
        {
            // internal
            m_texIntern[TEX_PING].reset( new Texture("intern_Ping", GL_TEXTURE_2D) );
            m_texIntern[TEX_PONG].reset( new Texture("intern_Pong", GL_TEXTURE_2D) );

            // output
            m_texOut[TEX_LUM].reset( new Texture("Lum", GL_TEXTURE_2D) );
        }

        PassLuminance::~PassLuminance()
        {
        }

        void PassLuminance::init()
        {
            // create internal FBOs
            m_fbo[FBO_MAIN].reset( new FBO( FBO::Components(FBO::COLOR), m_width, m_height ));
            m_fbo[FBO_PING_PONG].reset( new FBO( FBO::Components(FBO::COLOR), 1, 1 ));

            // shaders
            ShaderProgramManager* shaderMgr = ShaderProgramManager::getInstance();
            m_shader[SHADER_DRAWSCREEN] = shaderMgr->getShaderProgram("DrawScreen");
            m_shader[SHADER_LUMINANCE]  = shaderMgr->addShaderProgram("Luminance", "../Shaders/Basic2D.vert.glsl", "../Shaders/Luminance.frag.glsl");
            m_shader[SHADER_MIN_MAX]    = shaderMgr->addShaderProgram("MinMax", "../Shaders/Basic2D.vert.glsl", "../Shaders/MinMax.frag.glsl");
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
            m_texIntern[TEX_PING]->initGL(GL_RGBA32F, m_pingPongSize, m_pingPongSize, GL_RGBA, GL_FLOAT, nullptr);
            m_texIntern[TEX_PONG]->initGL(GL_RGBA32F, m_pingPongSize, m_pingPongSize, GL_RGBA, GL_FLOAT, nullptr);
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
            m_fbo[FBO_PING_PONG]->attachTexture( GL_COLOR_ATTACHMENT0, m_texIntern[TEX_PING].get() );
            m_fbo[FBO_PING_PONG]->attachTexture( GL_COLOR_ATTACHMENT1, m_texIntern[TEX_PONG].get() );
            m_fbo[FBO_PING_PONG]->unbind( true );

            GL_CHECK_ERROR;

            // Reset framebuffer state
            GL_ASSERT( glBindFramebuffer( GL_FRAMEBUFFER, 0 ) );

            GL_ASSERT( glDrawBuffer( GL_BACK ) );
            GL_ASSERT( glReadBuffer( GL_BACK ) );
        }

        void PassLuminance::renderPass()
        {
            // first apply the Luminance shader to the input texture (HDR) and write to output (LUM)
            m_fbo[FBO_MAIN]->useAsTarget( m_width, m_height );

            GL_ASSERT( glDrawBuffers(1, buffers + 1) );

            m_shader[SHADER_LUMINANCE]->bind();
            m_shader[SHADER_LUMINANCE]->setUniform("hdr", m_texIn[TEX_HDR], 0);
            m_canvas->render();

            // now that the LUM texture contains luminance(HDR), ping-pong with MinMax
            m_fbo[FBO_PING_PONG]->useAsTarget(); // prepare for ping-pong/redux combo

            uint size = m_pingPongSize;

            // first draw on PING to start
            GL_ASSERT( glDrawBuffers(1, buffers) );
            GL_ASSERT( glViewport(0, 0, size, size) );
            m_shader[SHADER_DRAWSCREEN]->bind();
            m_shader[SHADER_DRAWSCREEN]->setUniform( "screenTexture", m_texOut[TEX_LUM].get(), 0);
            m_canvas->render();

            // ping-pong to min/max and avg
            m_shader[SHADER_MIN_MAX]->bind();
            uint ping = 0;
            while (size != 1)
            {
                size /= 2;
                GL_ASSERT( glDrawBuffers(1, buffers + (ping + 1)%2) );
                GL_ASSERT( glViewport(0, 0, size, size) );

                m_shader[SHADER_MIN_MAX]->setUniform("color", m_texIntern[TEX_PING + ping].get(), 0);
                m_canvas->render();

                ++ ping %= 2;
            }

            // then draw the final result to LUM, whom (0,0) texel contains min, max
            m_fbo[FBO_MAIN]->useAsTarget();

            GL_ASSERT( glDrawBuffers(1, buffers + 1) );
            GL_ASSERT( glViewport(0, 0, m_width, m_height) );

            m_shader[SHADER_DRAWSCREEN]->bind();
            m_shader[SHADER_DRAWSCREEN]->setUniform( "screenTexture", m_texIntern[TEX_PING + ((ping+1)%2)].get(), 0 );
            m_canvas->render();
        }

        std::shared_ptr<Texture> PassLuminance::getInternTextures(uint i)
        {
            if (i < TEX_INTERNAL_COUNT)
                return m_texIntern[i];
            else
                return m_texIntern[0];
        }

    }
}

