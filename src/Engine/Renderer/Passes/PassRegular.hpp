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
            PassRegular(const std::string& name, uint w, uint h, uint nTexIn, uint nTexOut, std::string shader);
            ~PassRegular();
            virtual void renderPass();
            virtual void resizePass(uint w, uint h);
            virtual void resizePass();
            virtual void init();

            virtual Texture* getOut(uint slot);
            virtual std::shared_ptr<Texture> getInternTextures(uint i) const;

        public:
            std::string      m_shadername;
            const ShaderProgram* m_shader;
            RenderParameters     m_params;

        private:
            std::unique_ptr<FBO> m_fbo;
        };

    }
}


#endif // RADIUMENGINE_RENDERER_PASS_REGULAR_HPP
