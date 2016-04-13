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

        Pass::Pass(const std::string& name, uint w, uint h, uint nTexIn, uint nTexOut,
                   Mesh* canvas, uint priority)
            : m_name     ( name )
            , m_priority ( priority )
            , m_width    ( w )
            , m_height   ( h )
            , m_nTexIn   ( nTexIn )
            , m_nTexOut  ( nTexOut )
            , m_texIn    ( nTexIn )
            , m_texOut   ( nTexOut )
            , m_canvas   ( canvas )
        {
        }

        Pass::~Pass() {}

        void Pass::setIn(uint slot, Texture* tex)
        {
            //CORE_ASSERT( m_texIn[slot] = tex, "bad m_texIn array index" );
            m_texIn[slot] = tex;
        }

        void Pass::setOut(uint slot, Texture* tex)
        {
            //CORE_ASSERT( m_texOut[slot].reset(tex), "bad m_texOut array index");
            m_texOut[slot].reset(tex);
        }

        Texture* Pass::getIn(uint slot)
        {
            //CORE_ASSERT( return m_texIn[slot], "bad m_texIn array index" );
            return m_texIn[slot];
        }

        Texture* Pass::getOut(uint slot)
        {
            //CORE_ASSERT( return m_texOut[slot].get(), "bad m_texOut array index" );
            return m_texOut[slot].get();
        }

        bool Pass::operator <(const Pass& other)
        {
            // this operator is present in order to set up a
            // priorisation system through the #map of passes.
            // 0 stands for undefined and is the default value
            return ((m_priority < other.m_priority) && (m_priority != 0))
                || (other.m_priority == 0);
        }

        void Pass::setCanvas(Mesh* canvas)
        {
            m_canvas = canvas;
        }

    }
}
