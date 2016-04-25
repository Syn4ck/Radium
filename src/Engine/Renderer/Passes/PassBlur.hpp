#ifndef RADIUMENGINE_RENDERER_PASS_BLUR_HPP
#define RADIUMENGINE_RENDERER_PASS_BLUR_HPP

#include <Engine/RadiumEngine.hpp>
#include <Engine/Renderer/Renderer.hpp>

#include <Engine/Renderer/OpenGL/FBO.hpp>
#include <Engine/Renderer/Mesh/Mesh.hpp>
#include <Engine/Renderer/Passes/Pass.hpp>
#include <Engine/Renderer/OpenGL/FBO.hpp>
#include <Engine/Renderer/Texture/Texture.hpp>
#include <Engine/Renderer/Passes/PassPingPong.hpp>
#include <Engine/Renderer/RenderTechnique/ShaderProgram.hpp>
#include <Engine/Renderer/RenderTechnique/ShaderProgramManager.hpp>


namespace Ra
{
    namespace Engine
    {

        class RA_ENGINE_API PassBlur : public Pass
        {
        public:
            PassBlur(const std::string& name, uint w, uint h, uint nTexIn, uint nTexOut,
                     uint priority, uint amount=2, uint box=1);
            virtual ~PassBlur();

            void renderPass();
            void resizePass(uint w, uint h);
            void resizePass();
            void init();

            virtual std::shared_ptr<Texture> getInternTextures(uint i);

            virtual Texture* getOut(uint slot) override;

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

            uint m_boxfactor;
            std::array<std::unique_ptr<FBO>,FBO_COUNT> m_fbo;

            PassPingPong         m_pingpong;
            RenderParameters     m_params[2];

            enum Shaders
            {
                SHADER_BLUR = 0,
                SHADER_COUNT,
            };
            const ShaderProgram *m_shader[SHADER_COUNT];
        };

    }
}


#endif // RADIUMENGINE_RENDERER_PASS_BLUR_HPP
