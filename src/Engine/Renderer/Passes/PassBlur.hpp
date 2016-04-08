#ifndef RADIUMENGINE_RENDERER_PASS_BLUR_HPP
#define RADIUMENGINE_RENDERER_PASS_BLUR_HPP

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

        class RA_ENGINE_API PassBlur : public Pass
        {
        public:
            PassBlur(const std::string& name, uint w, uint h, uint nTexIn, uint nTexOut);
            virtual ~PassBlur();

            void renderPass(ShaderProgramManager* shaderMgr, Mesh* screen);
            void resizePass(uint w, uint h);
            void resizePass();
            void initFbos();

            virtual std::shared_ptr<Texture> getInternTextures(uint i);

        private:
            enum FboTags
            {
                FBO_MAIN = 0,
                FBO_BLUR,
                FBO_COUNT,
            };

            enum TextureInternalTags
            {
                TEX_BLUR_PING = 0,
                TEX_BLUR_PONG,
                TEX_INTERNAL_COUNT,
            };

            enum TextureInTags
            {
                TEX_COLOR = 0,
                TEX_IN_COUNT,
            };

            enum TextureOutTags
            {
                TEX_BLURRED = 0,
                TEX_OUT_COUNT,
            };

            uint m_pingPongSize;
            std::array<std::unique_ptr<FBO>,FBO_COUNT> m_fbo;
            std::array<std::shared_ptr<Texture>,TEX_INTERNAL_COUNT> m_internalTextures;
        };

    }
}


#endif // RADIUMENGINE_RENDERER_PASS_BLUR_HPP
