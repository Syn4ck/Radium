/*
#ifndef RADIUMENGINE_RENDERER_PASS_REDUX_HPP
#define RADIUMENGINE_RENDERER_PASS_REDUX_HPP

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
            PassRedux(const std::string& name, uint w, uint h, uint nTexIn, uint nTexOut, uint ratio,
                      const std::string& shader);
            virtual ~PassRedux();

            virtual void renderPass();
            virtual void resizePass(uint w, uint h);
            virtual void resizePass();
            virtual void init();

            virtual Texture* getOut(uint slot) const override;

            uint m_ratio;       // the ratio by which the size is divided each iteration

        private:
            enum TextureOutTags
            {
                TEX_PING = 0,    // both are equally prone to be the output texture
                TEX_PONG,
                TEX_OUT_COUNT,
            };

            // internal fbo
            std::unique_ptr<FBO> m_fbo;

            // external fbo and attachment offset to know where to write back
            FBO* m_parentFbo;
            uint m_parentAttachOffset;

            // parameters for redux to know what to do
            std::string      m_shadername;
            const ShaderProgram* m_shader;
            //RenderParameters     m_params;

            // ping-pong specific variable
            uint m_pingpong;
            uint m_pingPongSize;
        };

    }
}


#endif // RADIUMENGINE_RENDERER_REDUX_HPP
*/
