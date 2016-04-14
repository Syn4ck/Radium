#ifndef RADIUMENGINE_RENDERER_PASS_PINGPONG_HPP
#define RADIUMENGINE_RENDERER_PASS_PINGPONG_HPP

#include <Engine/RadiumEngine.hpp>
#include <Engine/Renderer/Renderer.hpp>

#include <Engine/Renderer/Mesh/Mesh.hpp>
#include <Engine/Renderer/OpenGL/FBO.hpp>
#include <Engine/Renderer/Passes/Pass.hpp>
#include <Engine/Renderer/Texture/Texture.hpp>
#include <Engine/Renderer/RenderTechnique/ShaderProgram.hpp>
#include <Engine/Renderer/RenderTechnique/RenderParameters.hpp>
#include <Engine/Renderer/RenderTechnique/ShaderProgramManager.hpp>

namespace Ra
{
    namespace Engine
    {

        class PassPingPong : public Pass
        {
        public:
            PassPingPong(const std::string& name, uint w, uint h, uint nTexIn, uint nTexOut, Mesh* canvas, uint priority, uint loop);
            virtual ~PassPingPong();

            void renderPass();
            void resizePass(uint w, uint h);
            void resizePass();
            void init();

            void setPingPongShader(const ShaderProgram* shader, RenderParameters* params);
            void setPongPingShader(const ShaderProgram* shader, RenderParameters* params);

            void setPingPongShader(const ShaderProgram* shader);
            void setPongPingShader(const ShaderProgram* shader);

            virtual std::shared_ptr<Texture> getInternTextures(uint i);

        protected:
            enum TextureInternalTags
            {
                TEX_PING = 0,
                TEX_INTERNAL_COUNT,
            };

            enum TextureOutTags
            {
                TEX_PONG = 0,    // the pong texture is always the out texture
                TEX_OUT_COUNT,
            };

            uint m_loop;

            std::unique_ptr<FBO> m_fbo;
            std::array<std::shared_ptr<Texture>,TEX_INTERNAL_COUNT> m_texIntern;

            enum Shaders
            {
                SHADER_PINGPONG = 0,
                SHADER_PONGPING,
                SHADER_COUNT,
            };

            const ShaderProgram* m_shader[SHADER_COUNT];
            RenderParameters*    m_params[SHADER_COUNT];
        };

    }
}

#endif // RADIUMENGINE_RENDERER_PASS_PINGPONG_HPP
