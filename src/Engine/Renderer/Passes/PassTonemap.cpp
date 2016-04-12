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

        PassTonemap::PassTonemap(const std::string& name, uint w, uint h, uint nTexIn, uint nTexOut)
            : Pass(name, w, h, nTexIn, nTexOut)
        {
            m_texOut[TEX_TONEMAP].reset( new Texture("TonemapPass", GL_TEXTURE_2D) );
        }

        PassTonemap::~PassTonemap()
        {
        }

        void PassTonemap::initFbos()
        {
            m_fbo[FBO_MAIN].reset( new FBO( FBO::Components(FBO::COLOR), m_width, m_height ));
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

        void PassTonemap::renderPass(ShaderProgramManager* shaderMgr, Mesh* screen)
        {
        }

        void PassTonemap::renderPass(ShaderProgramManager* shaderMgr, Mesh* screen, uint pingpongsize)
        {
            const ShaderProgram* shader = nullptr;

            m_fbo[FBO_MAIN]->useAsTarget(m_width, m_height);

            GL_ASSERT( glViewport(0, 0, m_width, m_height) );
            GL_ASSERT( glDrawBuffers(1, buffers+2) );

            shader = shaderMgr->getShaderProgram("TonemappingTEST");
            shader->bind();
            shader->setUniform("hdr", m_texIn[TEX_HDR], 0);
            shader->setUniform("lum", m_texIn[TEX_LUM], 1);
            shader->setUniform("pingpongsz", pingpongsize);
            screen->render();
        }

        void PassTonemap::renderPass(ShaderProgramManager* shaderMgr, Mesh* screen, float lumMin, float lumMax, float lumMean)
        {
            const ShaderProgram* shader = nullptr;

            m_fbo[FBO_MAIN]->useAsTarget(m_width, m_height);

            GL_ASSERT( glViewport(0, 0, m_width, m_height) );
            GL_ASSERT( glDrawBuffers(1, buffers + 2) );

            shader = shaderMgr->getShaderProgram("Tonemapping");
            shader->bind();
            shader->setUniform("hdr", m_texIn[TEX_HDR], 0);
            shader->setUniform("lumMin",  lumMin);
            shader->setUniform("lumMax",  lumMax);
            shader->setUniform("lumMean", lumMean);
            screen->render();
        }

        std::shared_ptr<Texture> PassTonemap::getInternTextures(uint i)
        {
            std::shared_ptr<Texture> empty;
            return empty;
        }


    }
}

