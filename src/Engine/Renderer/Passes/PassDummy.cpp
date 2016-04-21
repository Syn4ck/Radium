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

        PassDummy::PassDummy(const std::string& name, uint w, uint h, uint nTexIn, uint nTexOut,
                             Mesh* canvas, uint priority)
            : Pass(name, w, h, nTexIn, nTexOut, canvas, priority)
            , m_shader(nullptr)
        {
            // generate output texture
            m_texOut[TEX_DUMMY].reset( new Texture("Dummy", GL_TEXTURE_2D) );
        }

        PassDummy::~PassDummy()
        {
        }

        void PassDummy::init()
        {
            // initialize internal FBO
            m_fbo[FBO_MAIN].reset( new FBO( FBO::Components(FBO::COLOR), m_width, m_height ));

            // get shader
            ShaderProgramManager* shaderMgr = ShaderProgramManager::getInstance();
            m_shader = shaderMgr->addShaderProgram("Dummy", "../Shaders/Basic2D.vert.glsl", "../Shaders/Dummy.frag.glsl");
        }

        void PassDummy::resizePass(uint w, uint h)
        {
            m_width  = w;
            m_height = h;
            resizePass();
        }

        void PassDummy::resizePass()
        {
            // resize output textures accordingly
            m_texOut[TEX_DUMMY]->initGL(GL_RGBA32F, m_width, m_height, GL_RGBA, GL_FLOAT, nullptr);

            // initiate, bind and configure the main fbo
            m_fbo[FBO_MAIN]->bind();
            m_fbo[FBO_MAIN]->setSize( m_width, m_height );
            m_fbo[FBO_MAIN]->attachTexture( GL_COLOR_ATTACHMENT0, m_texIn[TEX_COLOR] );
            m_fbo[FBO_MAIN]->attachTexture( GL_COLOR_ATTACHMENT1, m_texOut[TEX_DUMMY].get() );
            m_fbo[FBO_MAIN]->unbind( true );
        }

        void PassDummy::renderPass()
        {
            m_fbo[FBO_MAIN]->useAsTarget(m_width, m_height);

            GL_ASSERT( glViewport(0, 0, m_width, m_height) );
            GL_ASSERT( glDrawBuffers(1, buffers+1) );

            m_shader->bind();
            m_shader->setUniform("color", m_texIn[TEX_COLOR]);
            m_canvas->render();
        }

        std::shared_ptr<Texture> PassDummy::getInternTextures(uint i)
        {
            std::shared_ptr<Texture> empty( nullptr );
            return empty;
        }


    }
}
