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

        Pass::Pass(const std::string& name, uint w, uint h, uint nTexIn, uint nTexOut,
                   Mesh* canvas, uint priority)
            : m_priority ( priority )
            , m_name     ( name )
            , m_width    ( w )
            , m_height   ( h )
            , m_nTexIn   ( nTexIn )
            , m_nTexOut  ( nTexOut )
            , m_texIn    ( nTexIn )
            , m_texOut   ( nTexOut )
            , m_canvas   ( canvas )
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

        Texture* Pass::getIn(uint slot)
        {
            return m_texIn[slot];
        }

        Texture* Pass::getOut(uint slot)
        {
            return m_texOut[slot].get();
        }

        void Pass::setCanvas(Mesh* canvas)
        {
            m_canvas = canvas;
        }

        std::string Pass::getName() const
        {
            return m_name;
        }


//        void Pass::sort(std::vector<std::unique_ptr<Pass>>& passVector)
//        {
//            auto f = [](const std::unique_ptr<Pass>& p1, const std::unique_ptr<Pass>& p2) -> bool
//            {
//                // return true if p1 is before p2 and is non-0 or if p2 is 0
//                return ((p1->m_priority < p2->m_priority) && (p1->m_priority != 0)) || (p2->m_priority == 0);
//            };

//            std::sort(passVector.begin(), passVector.end(), f);
//        }


    }
}
