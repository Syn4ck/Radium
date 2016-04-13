#ifndef RADIUMENGINE_RENDERER_PASS_DUMMY_H
#define RADIUMENGINE_RENDERER_PASS_DUMMY_H

#include <Engine/RadiumEngine.hpp>
#include <Engine/Renderer/Renderer.hpp>

#include <Engine/Renderer/Passes/Pass.hpp>

namespace Ra
{
    namespace Engine
    {

        class PassDummy : public Pass
        {
        public:
            PassDummy(const std::string& name, uint w, uint h, uint nTexIn, uint nTexOut, Mesh* canvas);
            ~PassDummy();
            void renderPass();
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
                TEX_COLOR = 0,
                TEX_IN_COUNT,
            };

            enum TextureOutTags
            {
                TEX_DUMMY = 0,
                TEX_OUT_COUNT,
            };

            std::array<std::unique_ptr<FBO>,FBO_COUNT> m_fbo;
            const ShaderProgram* m_shader;
        };

    }
}


#endif // RADIUMENGINE_RENDERER_PASS_DUMMY_HPP
