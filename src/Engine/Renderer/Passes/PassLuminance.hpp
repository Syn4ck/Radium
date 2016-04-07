#ifndef RADIUMENGINE_RENDERER_PASS_LUM_HPP
#define RADIUMENGINE_RENDERER_PASS_LUM_HPP

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

        class RA_ENGINE_API PassLuminance : public Pass
        {
        public:
            PassLuminance(const std::string& name, int w, int h, int nTexIn, int nTexOut);
            virtual ~PassLuminance();

            void initFbos();
            void renderPass(ShaderProgramManager* shaderMgr, Mesh* screen);

        private:
            enum FboTags
            {
                FBO_MAIN = 0,
                FBO_PING_PONG,
                FBO_COUNT
            };

            enum TextureInternalTags
            {
                TEX_PING = 0,
                TEX_PONG,
                TEX_INTERNAL_COUNT,
            };

            enum TextureInTags
            {
                TEX_LIT = 0
            };

            enum TextureOutTags
            {
                TEX_LUM = 0
            };

            uint m_pingPongSize;
            std::array<std::unique_ptr<FBO>,2> m_fbo;
            std::array<std::shared_ptr<Texture>,TEX_INTERNAL_COUNT> m_internalTextures;
        };

    }
}


#endif // RADIUMENGINE_RENDERER_PASS_LUM_HPP
