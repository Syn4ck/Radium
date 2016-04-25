#ifndef RADIUMENGINE_RENDERER_PASS_NODE_H
#define RADIUMENGINE_RENDERER_PASS_NODE_H

#include <Engine/RadiumEngine.hpp>
#include <Engine/Renderer/Renderer.hpp>

#include <Engine/Renderer/Passes/Pass.hpp>

namespace Ra {
    namespace Engine {

        class PassNode : public Pass
        {
        public:
            PassNode(const std::string& name, uint w, uint h, uint nTexIn, uint nTexOut, uint priority);
            ~PassNode();
            void renderPass();
            void resizePass(uint w, uint h);
            void resizePass();
            void init();

            virtual Texture* getOut(uint slot);
            virtual std::shared_ptr<Texture> getInternTextures(uint i);

        private:
            std::vector<std::shared_ptr<Pass>> m_subpasses;

            // tree structure
            Pass* parent;
            std::vector<Pass*> childs;

        };

    }
}

#endif // RADIUMENGINE_RENDERER_PASS_NODE_H
