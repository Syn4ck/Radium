#ifndef RADIUMENGINE_DEFERREDRENDERER_HPP
#define RADIUMENGINE_DEFERREDRENDERER_HPP

#include <Engine/RadiumEngine.hpp>
#include <Engine/Renderer/Renderer.hpp>

namespace Ra
{
    namespace Engine
    {
        class RA_ENGINE_API DeferredRenderer : public Renderer
        {
        public:
            DeferredRenderer(uint width, uint height);
            virtual ~DeferredRenderer();

            virtual std::string getRendererName() const override
            {
                return "Deferred Renderer";
            }

        protected:
            virtual void initializeInternal() override;
            virtual void resizeInternal() override;

            virtual void updateStepInternal(const RenderData& renderData) override;
            virtual void renderInternal(const RenderData& renderData) override;
            virtual void postProcessInternal(const RenderData& renderData) override;

        private:
            void initShaders();
            void initBuffers();

        private:
            // FIXME(charly): Move Normal, Albedo, Specular to (Normal + Roughness), (Albedo + Metalness)
            enum RendererTextures
            {
                TEX_DEPTH = 0,
                TEX_POSITION,
                TEX_NORMAL,
                TEX_ALBEDO,
                TEX_SPECULAR,
                TEX_LIT,
                TEX_COUNT
            };

            // Default renderer logic here, no need to be accessed by overriding renderers.
            std::unique_ptr<FBO> m_fbo;
            std::unique_ptr<FBO> m_postprocessFbo;
            std::array<std::unique_ptr<Texture>, TEX_COUNT> m_textures;
        };

    } // namespace Engine
} // namespace Ra

#endif // RADIUMENGINE_DEFERREDRENDERER_HPP
