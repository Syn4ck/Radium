//#include "PassCompose.hpp"

//#include <Engine/Renderer/OpenGL/FBO.hpp>
//#include <Engine/Renderer/Mesh/Mesh.hpp>
//#include <Engine/Renderer/RenderTechnique/ShaderProgram.hpp>
//#include <Engine/Renderer/RenderTechnique/ShaderProgramManager.hpp>

//namespace Ra
//{
//    namespace Engine
//    {

//        PassCompose::PassCompose(const std::string& name, uint w, uint h, uint nTexIn, uint nTexOut)
//            : Pass(name, w, h, nTexIn, nTexOut)
//        {
//        }

//        PassCompose::~PassCompose()
//        {
//        }

//        void PassCompose::initFbos()
//        {
//            m_fbo[FBO_MAIN].reset( new FBO( FBO::Components(FBO::COLOR), m_width, m_height ));

//        }

//        void PassCompose::resizePass(uint w, uint h)
//        {
//            m_width  = w;
//            m_height = h;
//            resizePass();
//        }

//        void PassCompose::resizePass()
//        {
//            // initiate, bind and configure the main fbo
//            m_fbo[FBO_MAIN]->bind();
//            m_fbo[FBO_MAIN]->setSize( m_width, m_height );
//            m_fbo[FBO_MAIN]->attachTexture( GL_COLOR_ATTACHMENT0, m_texIn[TEX_A] );
//            m_fbo[FBO_MAIN]->attachTexture( GL_COLOR_ATTACHMENT1, m_texIn[TEX_B] );
//            m_fbo[FBO_MAIN]->attachTexture( GL_COLOR_ATTACHMENT2, m_texOut[TEX_AB] );
//            m_fbo[FBO_MAIN]->unbind( true );
//        }

//        void PassCompose::renderPass(ShaderProgramManager* shaderMgr, Mesh* screen)
//        {
//            const ShaderProgram* shader = nullptr;

//            m_fbo[FBO_MAIN]->useAsTarget(m_width, m_height);

//            GL_ASSERT( glViewport(0, 0, m_width, m_height) );
//            GL_ASSERT( glDrawBuffers(1, buffers+2) );

//            shader = shaderMgr->getShaderProgram("Tonemapping");
//            shader->bind();
//            shader->setUniform("texA", m_texIn[TEX_A], 0);
//            shader->setUniform("texB", m_texIn[TEX_B], 0);
//            screen->render();
//        }

//        std::shared_ptr<Texture> PassCompose::getInternTextures(uint i)
//        {
//            std::shared_ptr<Texture> empty;
//            return empty;                      // Good/Bad ? nullptr ?
//        }


//    }
//}
