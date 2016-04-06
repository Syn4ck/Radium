#include <Engine/Renderer/Passes/Pass.hpp>
#include <Engine/Renderer/OpenGL/FBO.hpp>

namespace Ra
{
    namespace Engine {

        Pass::Pass(const std::string& name, int w, int h, int nTexIn=1, int nTexOut=1)
            : m_name( name )
            , m_width( w )
            , m_height( h )
            , m_fbo( nullptr )
        {
            // m_fbo.reset( new FBO( FBO::Components( FBO::COLOR ), m_width, m_height ) );
        }

        Pass::initFbo()
        {
            // how many fbos are needed
            int fboCount = m_texIn + m_texOut / GL_MAX_COLOR_ATTACHMENTS;

            // reallocate a new array if more space is required
            if (fboCount > 1)
                m_fbo.reset( new std::array<FBO, fboCount> );

            // initiate every fbo
            std::array<FBO,int>::iterator it_texIn  = m_texIn.begin();
            std::array<FBO,int>::iterator it_texOut = m_texOut.begin();

            for (int j = 0; j < fboCount; ++ j)
            {
                int i = 0;
                FBO f = m_fbo.get() + j;

                f.bind();
                f.setSize( m_width, m_height );

                // while there is space to attach textures IN
                while (( i < GL_MAX_COLOR_ATTACHMENTS ) && ( it_texIn != m_texIn.end() ))
                {
                    f.attachTexture( GL_COLOR_ATTACHMENT0 + (i ++), m_texIn );
                }

                // while there is space to attach textures OUT
                while (( i < GL_MAX_COLOR_ATTACHMENTS ) && ( it_texOut != m_texOut.end() ))
                {
                    f.attachTexture( GL_COLOR_ATTACHMENT0 + (i ++) );
                }

                f.unbind( true );
            }

        }

    }
}
