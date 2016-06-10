
#include "PassPingPong.hpp"

namespace Ra
{
    namespace Engine
    {

        PassPingPong::PassPingPong(const std::string& name, uint w, uint h, uint nTexIn, uint nTexOut,
                                   uint loop, const std::string& shaderPingPong)
            : Pass(name, w, h, nTexIn, nTexOut)
            , m_loop      ( loop )
            , m_paramPing ( m_paramIn )
            , m_shadername( shaderPingPong )
        {
        }

        PassPingPong::~PassPingPong()
        {
        }

        void PassPingPong::init()
        {
            // internal FBO
            m_fbo.reset( new FBO( FBO::Components(FBO::COLOR), m_width, m_height ));

            // shader
            ShaderProgramManager* shaderMgr = ShaderProgramManager::getInstance();
            m_shader = shaderMgr->addShaderProgram(m_shadername, "../Shaders/Basic2D.vert.glsl",
                                                   "../Shaders/" + m_shadername + ".frag.glsl");

            // set inputs from shader
            paramNamesFromShaderProgram(m_shader);

            // generate outputs
            m_outputs.push_back( std::unique_ptr<Texture>( new Texture("output_pong", GL_TEXTURE_2D)) );
            m_paramOut.addParameter( "", m_outputs.back().get() );

            m_texIntern[TEX_PING].reset( new Texture("internal_pong", GL_TEXTURE_2D));
        }

        void PassPingPong::resizePass(uint w, uint h)
        {
            m_width  = ((float) w * m_hModifier);
            m_height = ((float) h * m_hModifier);
            resizePass();
        }

        void PassPingPong::resizePass()
        {
            m_texIntern[TEX_PING]->initGL(GL_RGBA32F, m_width, m_height, GL_RGBA, GL_FLOAT, nullptr);

            for (auto& texout : m_outputs)
            {
                texout->initGL(GL_RGBA32F, m_width, m_height, GL_RGBA, GL_FLOAT, nullptr);
            }

            m_fbo->bind();
            m_fbo->setSize(m_width, m_height);
            m_fbo->attachTexture( GL_COLOR_ATTACHMENT0, m_texIntern[TEX_PING].get() );
            m_fbo->attachTexture( GL_COLOR_ATTACHMENT1, m_outputs[TEX_PONG].get() );
            m_fbo->unbind( true );

            GL_CHECK_ERROR;

            // reset GL state
            GL_ASSERT( glBindFramebuffer( GL_FRAMEBUFFER, 0 ) );

            GL_ASSERT( glDrawBuffer( GL_BACK ) );
            GL_ASSERT( glReadBuffer( GL_BACK ) );
        }

        void PassPingPong::renderPass()
        {
            m_fbo->useAsTarget(m_width, m_height);

            GL_ASSERT( glViewport(0, 0, m_width, m_height) );

            // first write to pong
            GL_ASSERT( glDrawBuffers(1, buffers + 1) );
            m_shader->bind();
            m_paramPing.bind(m_shader);
            m_canvas->render();

            // actually do the ping-pong
            for (uint i = 0; i < m_loop; ++i)
            {
                // pong->ping
                GL_ASSERT( glDrawBuffers(1, buffers) );
                m_shader->bind();
                m_paramPing.bind(m_shader); // here it should be a pong
                m_canvas->render();

                // ping->pong
                GL_ASSERT( glDrawBuffers(1, buffers + 1) );
                m_shader->bind();
                m_paramPing.bind(m_shader);
                m_canvas->render();
            }
        }

    }
}

