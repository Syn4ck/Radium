#ifndef RADIUMENGINE_RENDERER_PASS_HIGHPASS_HPP
#define RADIUMENGINE_RENDERER_PASS_HIGHPASS_HPP

#include <Engine/RadiumEngine.hpp>
#include <Engine/Renderer/Renderer.hpp>

#include <Engine/Renderer/Passes/Pass.hpp>
#include <Engine/Renderer/OpenGL/FBO.hpp>
#include <Engine/Renderer/Texture/Texture.hpp>
#include <Engine/Renderer/Mesh/Mesh.hpp>
#include <Engine/Renderer/RenderTechnique/ShaderProgram.hpp>
#include <Engine/Renderer/RenderTechnique/ShaderProgramManager.hpp>


namespace Ra
{
    namespace Engine
    {

        class RA_ENGINE_API PassHighpass : public Pass
        {
        public:
            PassHighpass(const std::string& name, uint w, uint h, uint nTexIn, uint nTexOut, uint priority);
            virtual ~PassHighpass();

            void renderPass();
            void renderPass(Scalar min, Scalar max, Scalar mean);
            void resizePass(uint w, uint h);
            void resizePass();
            void init();

            virtual std::shared_ptr<Texture> getInternTextures(uint i);

        private:
            enum FboTags
            {
                FBO_COLOR = 0,
                FBO_COUNT,
            };

            enum TextureInTags
            {
                TEX_LIT = 0,
                TEX_LUM,
                TEX_IN_COUNT,
            };

            enum TextureOutTags
            {
                TEX_HPASS = 0,
                TEX_OUT_COUNT,
            };

            std::array<std::unique_ptr<FBO>,FBO_COUNT> m_fbo;
            const ShaderProgram* m_shader;

            uint m_pingPongSize;
        };

    }
}


#endif // RADIUMENGINE_RENDERER_PASS_HIGHPASS_HPP
