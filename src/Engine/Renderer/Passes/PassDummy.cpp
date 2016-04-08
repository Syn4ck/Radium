#include "PassDummy.hpp"

#include <Engine/Renderer/Mesh/Mesh.hpp>
#include <Engine/Renderer/OpenGL/FBO.hpp>
#include <Engine/Renderer/Texture/Texture.hpp>
#include <Engine/Renderer/RenderTechnique/ShaderProgram.hpp>
#include <Engine/Renderer/RenderTechnique/ShaderProgramManager.hpp>

#include <iostream>

namespace Ra
{
    namespace Engine
    {

        PassDummy::PassDummy(const std::string& name, uint w, uint h, uint nTexIn, uint nTexOut)
            : Pass(name, w, h, nTexIn, nTexOut)
        {
            // m_texOut->initGL(GL_RGBA32F, m_width, m_height, GL_RGBA, GL_FLOAT, nullptr);
        }

        PassDummy::~PassDummy()
        {
        }

        void PassDummy::initFbos()
        {
            m_fbo[FBO_MAIN].reset( new FBO( FBO::Components(FBO::COLOR), m_width, m_height ));

        }

        void PassDummy::resizePass(uint w, uint h)
        {
            m_width  = w;
            m_height = h;
            resizePass();
        }

        void PassDummy::resizePass()
        {
            // initiate, bind and configure the main fbo
            m_fbo[FBO_MAIN]->bind();
            m_fbo[FBO_MAIN]->setSize( m_width, m_height );
            m_fbo[FBO_MAIN]->attachTexture( GL_COLOR_ATTACHMENT0, m_texIn[TEX_COLOR] );
            m_fbo[FBO_MAIN]->attachTexture( GL_COLOR_ATTACHMENT1, m_texOut[TEX_DUMMY] );
            m_fbo[FBO_MAIN]->unbind( true );
        }

        void PassDummy::renderPass(ShaderProgramManager* shaderMgr, Mesh* screen)
        {
            const ShaderProgram* shader = nullptr;

            m_fbo[FBO_MAIN]->useAsTarget(m_width, m_height);

            GL_ASSERT( glViewport(0, 0, m_width, m_height) );
            GL_ASSERT( glDrawBuffers(1, buffers+1) );

            shader = shaderMgr->getShaderProgram("Dummy");
            shader->bind();
            shader->setUniform("color", m_texIn[TEX_COLOR], 0);
            screen->render();
        }

        std::shared_ptr<Texture> PassDummy::getInternTextures(uint i)
        {
            std::shared_ptr<Texture> empty( nullptr );
            return empty;
        }


    }
}
