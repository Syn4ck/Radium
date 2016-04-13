#ifndef RADIUMENGINE_RENDERER_PASS_COMPOSE_HPP
#define RADIUMENGINE_RENDERER_PASS_COMPOSE_HPP

#include <Engine/RadiumEngine.hpp>
#include <Engine/Renderer/Renderer.hpp>

#include <Engine/Renderer/Passes/Pass.hpp>

namespace Ra
{
    namespace Engine
    {

        class PassCompose : public Pass
        {
        public:
            PassCompose(const std::string& name, uint w, uint h, uint nTexIn, uint nTexOut);
            ~PassCompose();
            void renderPass(Mesh* screen);
            void resizePass(uint w, uint h);
            void resizePass();
            void init();

            virtual std::shared_ptr<Texture> getInternTextures(uint i);

            enum FboTags
            {
                FBO_MAIN = 0,
                FBO_COUNT,
            };

            enum TextureInTags
            {
                TEX_A = 0,
                TEX_B,
                TEX_IN_COUNT,
            };

            enum TextureOutTags
            {
                TEX_AB = 0,
                TEX_OUT_COUNT,
            };

            std::array<std::unique_ptr<FBO>,FBO_COUNT> m_fbo;
            const ShaderProgram* m_shader;
        };

    }
}


#endif // RADIUMENGINE_RENDERER_PASS_COMPOSE_HPP
