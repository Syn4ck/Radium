#ifndef RADIUMENGINE_RENDERER_PASS_REGULAR_H
#define RADIUMENGINE_RENDERER_PASS_REGULAR_H

#include <Engine/RadiumEngine.hpp>
#include <Engine/Renderer/Renderer.hpp>

#include <Engine/Renderer/Passes/Pass.hpp>

namespace Ra
{
    namespace Engine
    {

        class RA_ENGINE_API PassRegular : public Pass
        {
        public:
            PassRegular(const std::string& name, uint w, uint h, uint nTexIn, uint nTexOut, uint priority, std::string shader);
            ~PassRegular();
            void renderPass();
            void resizePass(uint w, uint h);
            void resizePass();
            void init();

            virtual Texture* getOut(uint slot);
            virtual std::shared_ptr<Texture> getInternTextures(uint i);

        public:
            RenderParameters     m_params;
            const ShaderProgram* m_shader;
            std::string      m_shadername;

        private:
            std::unique_ptr<FBO> m_fbo;
        };

    }
}


#endif // RADIUMENGINE_RENDERER_PASS_REGULAR_HPP
