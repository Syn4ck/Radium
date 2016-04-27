#include <algorithm>

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

        Pass::Pass(const std::string& name, uint w, uint h, uint nTexIn, uint nTexOut)
            : m_name     ( name )
            , m_width    ( w )
            , m_height   ( h )
            , m_nTexIn   ( nTexIn )
            , m_nTexOut  ( nTexOut )
            , m_texIn    ( nTexIn )
            , m_texOut   ( nTexOut )
            , m_canvas   ( nullptr )
        {
            // resize vectors of textures if necessary
            if (m_nTexIn > 1)
                m_texIn.resize(m_nTexIn, nullptr);

            if (m_nTexOut > 1)
                m_texOut.resize(m_nTexOut, nullptr);
        }

        Pass::~Pass() {}

        void Pass::setIn(uint slot, Texture* tex)
        {
            m_texIn[slot] = tex;
        }

        void Pass::setOut(uint slot, Texture* tex)
        {
            m_texOut[slot].reset(tex);
        }

        Texture* Pass::getIn(uint slot) const
        {
            return m_texIn[slot];
        }

        Texture* Pass::getOut(uint slot) const
        {
            return m_texOut[slot].get();
        }

        void Pass::setCanvas(Mesh* canvas)
        {
            m_canvas = canvas;
        }

        uint Pass::getNIn()  const { return m_nTexIn;  }

        uint Pass::getNOut() const { return m_nTexOut; }

        uint Pass::getId()   const { return m_id;      }

        std::string Pass::getName() const { return m_name; }

    }
}
