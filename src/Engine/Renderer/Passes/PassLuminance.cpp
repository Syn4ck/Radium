#include "PassLuminance.hpp"

#include <iostream>

namespace Ra {
    namespace Engine {

        PassLuminance::PassLuminance(const std::string& name, uint w, uint h, uint nTexIn, uint nTexOut,
                                     Mesh* canvas, uint priority)
            : Pass(name, w, h, nTexIn, nTexOut, canvas, priority)
            , m_redux("redux", w, h, 1, 2, canvas, 0, 2)
        {
            // pre-output only, the real output is m_redux's one
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
            m_shader[SHADER_LUMINANCE]  = shaderMgr->addShaderProgram("Luminance", "../Shaders/Basic2D.vert.glsl", "../Shaders/Luminance.frag.glsl");
            m_shader[SHADER_MIN_MAX]    = shaderMgr->addShaderProgram("MinMax", "../Shaders/Basic2D.vert.glsl", "../Shaders/MinMax.frag.glsl");

            // init. redux pass
            m_redux.setCanvas(m_canvas);
            m_redux.setIn(0, m_texOut[TEX_LUM].get());
            m_redux.init();

            // tell it to use the min-max shader
            m_params.addParameter("color", m_texOut[TEX_LUM].get(), 0);
            m_redux.setShader(m_shader[SHADER_MIN_MAX], &m_params);

            // then tell it to return result at GL_COLOR_ATTACHMENT1
            m_redux.setReturningBuffer(m_fbo[FBO_MAIN].get(), 1);
        }

        void PassLuminance::resizePass(uint w, uint h)
        {
            m_width  = w;
            m_height = h;
            resizePass();
        }

        void PassLuminance::resizePass()
        {
            // resize textures
            m_texOut[TEX_LUM]->initGL(GL_RGBA32F, m_width, m_height, GL_RGBA, GL_FLOAT, nullptr);

            // initiate, bind and configure the main fbo
            m_fbo[FBO_MAIN]->bind();
            m_fbo[FBO_MAIN]->setSize( m_width, m_height );
            m_fbo[FBO_MAIN]->attachTexture( GL_COLOR_ATTACHMENT0, m_texIn[TEX_HDR] );
            m_fbo[FBO_MAIN]->attachTexture( GL_COLOR_ATTACHMENT1, m_texOut[TEX_LUM].get() );
            m_fbo[FBO_MAIN]->unbind( true );

            // cascade resizement
            m_redux.resizePass(m_width, m_height);

            GL_CHECK_ERROR;

            // Reset framebuffer state
            GL_ASSERT( glBindFramebuffer( GL_FRAMEBUFFER, 0 ) );

            GL_ASSERT( glDrawBuffer( GL_BACK ) );
            GL_ASSERT( glReadBuffer( GL_BACK ) );
        }

        void PassLuminance::renderPass()
        {
            // first apply the Luminance shader to the input texture (HDR) and write to output (LUM)
            m_fbo[FBO_MAIN]->useAsTarget(m_width, m_height);

            GL_ASSERT( glDrawBuffers(1, buffers + 1) );
            GL_ASSERT( glViewport(0, 0, m_width, m_height) );

            // luminance transformation
            m_shader[SHADER_LUMINANCE]->bind();
            m_shader[SHADER_LUMINANCE]->setUniform("hdr", m_texIn[TEX_HDR], 0);
            m_canvas->render();

            // apply min-max
            m_redux.renderPass();
        }

        std::shared_ptr<Texture> PassLuminance::getInternTextures(uint i)
        {
            return m_texOut[0];
        }

    }
}

