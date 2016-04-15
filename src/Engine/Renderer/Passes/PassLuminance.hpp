#ifndef RADIUMENGINE_RENDERER_PASS_LUM_HPP
#define RADIUMENGINE_RENDERER_PASS_LUM_HPP

#include <Engine/RadiumEngine.hpp>
#include <Engine/Renderer/Renderer.hpp>

#include <Engine/Renderer/Passes/Pass.hpp>
#include <Engine/Renderer/Passes/PassRedux.hpp>
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
            PassLuminance(const std::string& name, uint w, uint h, uint nTexIn, uint nTexOut,
                          Mesh* canvas, uint priority);
            virtual ~PassLuminance();

            void renderPass();
            void resizePass(uint w, uint h);
            void resizePass();
            void init();

            virtual std::shared_ptr<Texture> getInternTextures(uint i);

        private:
            enum FboTags
            {
                FBO_MAIN = 0,
                FBO_COUNT,
            };

            enum TextureInTags
            {
                TEX_HDR = 0,
                TEX_IN_COUNT,
            };

            enum TextureOutTags
            {
                TEX_LUM = 0,
                TEX_VALUES,
                TEX_INTERNAL_COUNT,
            };

            std::array<std::unique_ptr<FBO>,FBO_COUNT> m_fbo;

            enum Shaders
            {
                SHADER_LUMINANCE = 0,
                SHADER_MIN_MAX,
                SHADER_COUNT,
            };

            const ShaderProgram *m_shader[SHADER_COUNT];

            PassRedux        m_redux;
            RenderParameters m_params;
        };

    }
}


#endif // RADIUMENGINE_RENDERER_PASS_LUM_HPP
