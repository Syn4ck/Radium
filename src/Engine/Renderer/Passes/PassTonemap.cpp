#include "PassTonemap.hpp"

#include <Engine/Renderer/OpenGL/FBO.hpp>
#include <Engine/Renderer/Mesh/Mesh.hpp>
#include <Engine/Renderer/Texture/Texture.hpp>
#include <Engine/Renderer/RenderTechnique/ShaderProgram.hpp>
#include <Engine/Renderer/RenderTechnique/ShaderProgramManager.hpp>

namespace Ra
{
    namespace Engine
    {

        PassTonemap::PassTonemap(const std::string& name, uint w, uint h, uint nTexIn, uint nTexOut, Mesh* canvas)
            : Pass(name, w, h, nTexIn, nTexOut, canvas)
        {
            // out
            m_texOut[TEX_TONEMAP].reset( new Texture("TonemapPass", GL_TEXTURE_2D) );
        }

        PassTonemap::~PassTonemap()
        {
        }

        void PassTonemap::init()
        {
            m_fbo[FBO_MAIN].reset( new FBO( FBO::Components(FBO::COLOR), m_width, m_height ));

            // shaders
            ShaderProgramManager* shaderMgr = ShaderProgramManager::getInstance();
            m_shader = shaderMgr->addShaderProgram("Tonemapping", "../Shaders/Basic2D.vert.glsl", "../Shaders/Tonemapping.frag.glsl");
        }

        void PassTonemap::resizePass(uint w, uint h)
        {
            m_width  = w;
            m_height = h;
            resizePass();
        }

        void PassTonemap::resizePass()
        {
            // update out texture
            m_texOut[TEX_TONEMAP]->initGL(GL_RGBA32F, m_width, m_height, GL_RGBA, GL_FLOAT, nullptr);

            // initiate, bind and configure the main fbo
            m_fbo[FBO_MAIN]->bind();
            m_fbo[FBO_MAIN]->setSize( m_width, m_height );
            m_fbo[FBO_MAIN]->attachTexture( GL_COLOR_ATTACHMENT0, m_texIn[TEX_HDR] );
            m_fbo[FBO_MAIN]->attachTexture( GL_COLOR_ATTACHMENT1, m_texIn[TEX_LUM] );
            m_fbo[FBO_MAIN]->attachTexture( GL_COLOR_ATTACHMENT2, m_texOut[TEX_TONEMAP].get() );
            m_fbo[FBO_MAIN]->unbind( true );
        }

        void PassTonemap::renderPass()
        {
        }

        void PassTonemap::renderPass(uint pingpongsize)
        {
            m_fbo[FBO_MAIN]->useAsTarget(m_width, m_height);

            GL_ASSERT( glViewport(0, 0, m_width, m_height) );
            GL_ASSERT( glDrawBuffers(1, buffers+2) );

            m_shader->bind();
            m_shader->setUniform("hdr", m_texIn[TEX_HDR], 0);
            m_shader->setUniform("lum", m_texIn[TEX_LUM], 1);
            m_shader->setUniform("pingpongsz", pingpongsize);
            m_canvas->render();
        }

        void PassTonemap::renderPass(float lumMin, float lumMax, float lumMean)
        {
            m_fbo[FBO_MAIN]->useAsTarget(m_width, m_height);

            GL_ASSERT( glViewport(0, 0, m_width, m_height) );
            GL_ASSERT( glDrawBuffers(1, buffers + 2) );

            m_shader->bind();
            m_shader->setUniform("hdr", m_texIn[TEX_HDR], 0);
            m_shader->setUniform("lumMin",  lumMin);
            m_shader->setUniform("lumMax",  lumMax);
            m_shader->setUniform("lumMean", lumMean);
            m_canvas->render();
        }

        std::shared_ptr<Texture> PassTonemap::getInternTextures(uint i)
        {
            std::shared_ptr<Texture> empty;
            return empty;
        }


    }
}

