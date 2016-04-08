#include "PassHighpass.hpp"

//#include <iostream>

namespace Ra {
    namespace Engine {

        PassHighpass::PassHighpass(const std::string& name, uint w, uint h, uint nTexIn=1, uint nTexOut=1)
            : Pass(name, w, h, nTexIn, nTexOut)
        {
        }

        PassHighpass::~PassHighpass()
        {
        }

        void PassHighpass::initFbos()
        {
            // create internal FBOs
            m_fbo[FBO_BLOOM].reset( new FBO( FBO::Components(FBO::COLOR), m_width, m_height ));
        }

        void PassHighpass::resizePass(uint w, uint h)
        {
            m_width  = w;
            m_height = h;
            resizePass();
        }

        void PassHighpass::resizePass()
        {
            // initiate, bind and configure the main fbo
            m_fbo[FBO_BLOOM]->bind();
            m_fbo[FBO_BLOOM]->setSize( m_width, m_height );
            m_fbo[FBO_BLOOM]->attachTexture( GL_COLOR_ATTACHMENT0, m_texIn[TEX_LIT] );
            m_fbo[FBO_BLOOM]->attachTexture( GL_COLOR_ATTACHMENT1, m_texIn[TEX_LUM] );
            m_fbo[FBO_BLOOM]->attachTexture( GL_COLOR_ATTACHMENT2, m_texOut[TEX_HPASS] );
            m_fbo[FBO_BLOOM]->unbind( true );

            GL_CHECK_ERROR;

            // Reset framebuffer state
            GL_ASSERT( glBindFramebuffer( GL_FRAMEBUFFER, 0 ) );

            GL_ASSERT( glDrawBuffer( GL_BACK ) );
            GL_ASSERT( glReadBuffer( GL_BACK ) );
        }

        void PassHighpass::renderPass(ShaderProgramManager* shaderMgr, Mesh *screen)
        {
            const ShaderProgram* shader = nullptr;

            m_fbo[FBO_BLOOM]->useAsTarget();

            GL_ASSERT( glViewport(0, 0, m_width/8, m_height/8) );
            GL_ASSERT( glDrawBuffers(1, buffers+2) );

            shader = shaderMgr->getShaderProgram("Highpass");
            shader->bind();
            shader->setUniform("hdr",  m_texIn[TEX_LIT], 0);
            shader->setUniform("lum", m_texOut[TEX_LUM], 0);
            screen->render();
        }

        std::shared_ptr<Texture> PassHighpass::getInternTextures(uint i)
        {
            std::shared_ptr<Texture> empty( nullptr );
            return empty;
        }

    }
}

