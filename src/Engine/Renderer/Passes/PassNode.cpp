#include <Engine/Renderer/Passes/PassNode.hpp>

namespace Ra
{
    namespace Engine
    {

        PassNode::PassNode(const std::string& name, uint w, uint h, uint nTexIn, uint nTexOut, uint priority)
            : Pass(name, w, h, nTexIn, nTexOut, priority)
        {
        }

        PassNode::~PassNode()
        {
        }

        void PassNode::init()
        {
            // init every subpasses
            for (auto const& pass : m_subpasses)
            {
                pass->init();
            }
        }

        void PassNode::resizePass(uint w, uint h)
        {
             m_width  = w;
             m_height = h;
             resizePass();
        }

        void PassNode::resizePass()
        {
            // resize every subpasses
            for (auto const& pass : m_subpasses)
            {
                pass->resizePass(m_width, m_height);
            }
        }

        void PassNode::renderPass()
        {
        }

        Texture* PassNode::getOut(uint slot) { return nullptr; }

        std::shared_ptr<Texture> PassNode::getInternTextures(uint i) { return nullptr; }

    }
}
