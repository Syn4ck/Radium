#include "PassCompose.hpp"

#include <Engine/Renderer/OpenGL/FBO.hpp>
#include <Engine/Renderer/Mesh/Mesh.hpp>
#include <Engine/Renderer/Texture/Texture.hpp>
#include <Engine/Renderer/RenderTechnique/ShaderProgram.hpp>
#include <Engine/Renderer/RenderTechnique/ShaderProgramManager.hpp>

namespace Ra
{
    namespace Engine
    {

        PassCompose::PassCompose(const std::string& name, uint w, uint h, uint nTexIn, uint nTexOut)
            : Pass(name, w, h, nTexIn, nTexOut)
        {
            // create out texture
            m_texOut[TEX_AB].reset( new Texture("Composite", GL_TEXTURE_2D) );
        }

        PassCompose::~PassCompose()
        {
        }

        void PassCompose::init()
        {
            m_fbo[FBO_MAIN].reset( new FBO( FBO::Components(FBO::COLOR), m_width, m_height ));

            // get shaders
            ShaderProgramManager* shaderMgr = ShaderProgramManager::getInstance();
            m_shader = shaderMgr->getShaderProgram("FinalCompose");
        }

        void PassCompose::resizePass(uint w, uint h)
        {
            m_width  = w;
            m_height = h;
            resizePass();
        }

        void PassCompose::resizePass()
        {
            // resize texture output
            m_texOut[TEX_AB]->initGL(GL_RGBA32F, m_width, m_height, GL_RGBA, GL_FLOAT, nullptr);

            // initiate, bind and configure the main fbo
            m_fbo[FBO_MAIN]->bind();
            m_fbo[FBO_MAIN]->setSize( m_width, m_height );
            m_fbo[FBO_MAIN]->attachTexture( GL_COLOR_ATTACHMENT0, m_texIn[TEX_A] );
            m_fbo[FBO_MAIN]->attachTexture( GL_COLOR_ATTACHMENT1, m_texIn[TEX_B] );
            m_fbo[FBO_MAIN]->attachTexture( GL_COLOR_ATTACHMENT2, m_texOut[TEX_AB].get() );
            m_fbo[FBO_MAIN]->unbind( true );
        }

        void PassCompose::renderPass(Mesh* screen)
        {
            m_fbo[FBO_MAIN]->useAsTarget(m_width, m_height);

            GL_ASSERT( glViewport(0, 0, m_width, m_height) );
            GL_ASSERT( glDrawBuffers(1, buffers+2) );

            m_shader->bind();
            m_shader->setUniform("texA", m_texIn[TEX_A], 0);
            m_shader->setUniform("texB", m_texIn[TEX_B], 1);
            screen->render();
        }

        std::shared_ptr<Texture> PassCompose::getInternTextures(uint i)
        {
            std::shared_ptr<Texture> empty;
            return empty;                      // Good/Bad ? nullptr ?
        }


    }
}
