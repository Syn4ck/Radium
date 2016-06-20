
#include "PassBlur.hpp"

namespace Ra
{
    namespace Engine
    {
        void PassBlur::renderPass( const RenderData& renderData )
        {
            m_fbo->useAsTarget(m_width, m_height);

            GL_ASSERT( glViewport(0, 0, m_width, m_height) );

            // this bypasses the RenderParameter in order to tweak input.
            // as an offset (vec2) is required, the idea is to use x as X blur (x/m_width, 0)
            // and y as Y blur rate (0, y/m_height).
            Core::Vector2 xblur, yblur;

            Core::Vector2& offset = m_paramPing.getVec2Parameter("offset");

            yblur = Core::Vector2( 0, offset(1) / (float) m_height );
            xblur = Core::Vector2( offset(0) / (float) m_width,  0 );

            // first write to pong
            GL_ASSERT( glDrawBuffers(1, buffers + 1) );
            m_shader->bind();
            m_paramPing.addParameter("offset", xblur);
            m_paramPing.bind(m_shader);
            m_canvas->render();

            // actually do the ping-pong
            for (uint i = 0; i < m_loop; ++i)
            {
                // pong->ping
                GL_ASSERT( glDrawBuffers(1, buffers) );
                m_shader->bind();
                m_shader->setUniform("color",  m_outputs[TEX_PONG].get());
                m_shader->setUniform("offset", yblur);
                m_canvas->render();

                // ping->pong
                GL_ASSERT( glDrawBuffers(1, buffers + 1) );
                m_shader->bind();
                m_shader->setUniform("color",  m_texIntern[TEX_PING].get());
                m_shader->setUniform("offset", xblur);
                m_paramPing.bind(m_shader);
                m_canvas->render();
            }
        }

    }
}

