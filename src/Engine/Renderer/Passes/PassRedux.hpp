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

        class RA_ENGINE_API PassRedux : public Pass
        {
        public:
            PassRedux(const std::string& name, uint w, uint h, uint nTexIn, uint nTexOut, Mesh* canvas, uint priority, uint ratio);
            virtual ~PassRedux();

            void renderPass();
            void resizePass(uint w, uint h);
            void resizePass();
            void init();

            void setShader(const ShaderProgram* shader, RenderParameters* params);

            virtual std::shared_ptr<Texture> getInternTextures(uint i);
            virtual Texture* getOut(uint slot) override;

            uint m_ratio;        // the ratio by which the size is divided each iteration

        private:
            enum TextureOutTags
            {
                TEX_PING = 0,    // both are equally prone to be the output texture
                TEX_PONG,
                TEX_OUT_COUNT,
            };

            std::unique_ptr<FBO> m_fbo;

            const ShaderProgram* m_shader;
            RenderParameters*    m_params;

            uint m_pingpong;     // tells what texture to return
            uint m_pingPongSize;
        };

    }
}

#endif // RADIUMENGINE_RENDERER_PASS_PINGPONG_HPP
