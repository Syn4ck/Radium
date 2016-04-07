#include <Engine/Renderer/OpenGL/FBO.hpp>
#include <Engine/Renderer/Passes/Pass.hpp>
#include <Engine/Renderer/Texture/Texture.hpp>


namespace Ra
{
    namespace Engine {

        // a static buffer for every passes to use in glDrawBuffers
        const GLenum Pass::buffers[] = {
                    GL_COLOR_ATTACHMENT0,
                    GL_COLOR_ATTACHMENT1,
                    GL_COLOR_ATTACHMENT2,
                    GL_COLOR_ATTACHMENT3,
                    GL_COLOR_ATTACHMENT4,
                    GL_COLOR_ATTACHMENT5,
                    GL_COLOR_ATTACHMENT6,
                    GL_COLOR_ATTACHMENT7
                };

        Pass::Pass(const std::string& name, int w, int h, int nTexIn=0, int nTexOut=0)
            : m_name    ( name )
            , m_width   ( w )
            , m_height  ( h )
            , m_nTexIn  ( nTexIn )
            , m_nTexOut ( nTexOut )
        {
            m_texIn.resize  (nTexIn, nullptr);
            m_texOut.resize (nTexOut, nullptr);
        }

        Pass::~Pass() {}

        void Pass::setIn(int slot, Texture* tex)
        {
            if ( slot < m_nTexIn )
            {
                m_texIn[slot].reset( tex );
            }
            else
            {
                // (1) ask for error management policy, e.g exit() or return ERROR
            }
        }

        void Pass::setOut(int slot, Texture* tex)
        {
            if ( slot < m_nTexOut )
            {
                m_texOut[slot].reset( tex );
            }
            else
            {
                // TODO(Hugo) manage error (see (1))
            }
        }


    }
}
