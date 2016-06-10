#include "PassRedux.hpp"

namespace Ra
{
    namespace Engine
    {

        PassRedux::PassRedux(const std::string& name, uint w, uint h, uint nTexIn, uint nTexOut,
                             const std::string& shader, uint ratio)
            : Pass(name, w, h, nTexIn, 2)
            , m_shadername( shader )
            , m_ratio     ( ratio  )
            , m_pingpong  ( 0      )
        {
        }

        PassRedux::~PassRedux()
        {
        }

        void PassRedux::init()
        {
            // internal FBO
            m_fbo.reset( new FBO( FBO::Components(FBO::COLOR), 1, 1 ));

            // get shader
            ShaderProgramManager* shaderMgr = ShaderProgramManager::getInstance();
            m_shader = shaderMgr->addShaderProgram(m_shadername, "../Shaders/Basic2D.vert.glsl", "../Shaders/" + m_shadername + ".frag.glsl");

            // inputs
            paramNamesFromShaderProgram(m_shader);

            // outputs
            m_outputs.push_back( std::unique_ptr<Texture>( new Texture("output_a", GL_TEXTURE_2D) ));
            m_outputs.push_back( std::unique_ptr<Texture>( new Texture("output_b", GL_TEXTURE_2D) ));

            // compute the required number of reductions and tell which texture will be the output
            if ( (nbResizements() % 2) == 0 )
            {
                m_paramOut.addParameter( "",             m_outputs[TEX_PING].get() );
                m_paramOut.addParameter( "intern_redux", m_outputs[TEX_PONG].get() );
            }
            else
            {
                m_paramOut.addParameter( "intern_redux", m_outputs[TEX_PING].get() );
                m_paramOut.addParameter( "",             m_outputs[TEX_PONG].get() );
            }
        }

        void PassRedux::resizePass(uint w, uint h)
        {
            m_width  = ((float) w * m_wModifier);
            m_height = ((float) h * m_hModifier);
            resizePass();
        }

        void PassRedux::resizePass()
        {
            m_pingPongSize = std::pow(2.0, Scalar(uint(std::log2(std::min(m_width, m_height)))));

            m_outputs[TEX_PING]->initGL(GL_RGBA32F, m_pingPongSize, m_pingPongSize, GL_RGBA, GL_FLOAT, nullptr);
            m_outputs[TEX_PONG]->initGL(GL_RGBA32F, m_pingPongSize, m_pingPongSize, GL_RGBA, GL_FLOAT, nullptr);

            m_fbo->bind();
            m_fbo->setSize(m_pingPongSize, m_pingPongSize);
            m_fbo->attachTexture( GL_COLOR_ATTACHMENT0, m_outputs[TEX_PING].get() );
            m_fbo->attachTexture( GL_COLOR_ATTACHMENT1, m_outputs[TEX_PONG].get() );
            m_fbo->unbind( true );

            GL_CHECK_ERROR;

            // reset GL state
            GL_ASSERT( glBindFramebuffer( GL_FRAMEBUFFER, 0 ) );

            GL_ASSERT( glDrawBuffer( GL_BACK ) );
            GL_ASSERT( glReadBuffer( GL_BACK ) );

//            // compute the required number of reductions and tell which texture will be the output
//            if ( (nbResizements() % 2) == 0 )
//            {
//                m_paramOut.addParameter( "",             m_outputs[TEX_PING].get() );
//                m_paramOut.addParameter( "intern_redux", m_outputs[TEX_PONG].get() );
//            }
//            else
//            {
//                m_paramOut.addParameter( "intern_redux", m_outputs[TEX_PING].get() );
//                m_paramOut.addParameter( "",             m_outputs[TEX_PONG].get() );
//            }
        }

        void PassRedux::renderPass()
        {
            // locally load the drawscreen shader
            ShaderProgramManager* shaderMgr = ShaderProgramManager::getInstance();
            const ShaderProgram* drawscreen = shaderMgr->getShaderProgram("DrawScreen");

            m_fbo->useAsTarget();

            uint     size = m_pingPongSize;
            Texture* tex  = m_paramIn.getTexParameter("color");

            // first write to ping
            GL_ASSERT( glDrawBuffers(1, buffers) );
            GL_ASSERT( glViewport(0, 0, size, size) );
            drawscreen->bind();
            drawscreen->setUniform("color", tex);
            m_canvas->render();

            m_shader->bind();
            m_pingpong = 0;

            // the actual redux loop
            while (size != 1)
            {
                size /= m_ratio;

                ++ m_pingpong %= 2;
                tex = m_outputs[(m_pingpong+1)%2].get();

                GL_ASSERT( glViewport(0, 0, size, size) );
                GL_ASSERT( glDrawBuffers(1, buffers + m_pingpong) );
                m_shader->setUniform("color", tex);
                m_canvas->render();
            }
        }

        uint PassRedux::nbResizements() const
        {
            uint size = m_pingPongSize;
            uint nb;

            while (size > 1)
            {
                size /= m_ratio;
                ++ nb;
            }

            return nb;
        }

    }
}

