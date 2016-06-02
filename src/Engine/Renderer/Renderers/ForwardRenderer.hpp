#ifndef RADIUMENGINE_FORWARDRENDERER_HPP
#define RADIUMENGINE_FORWARDRENDERER_HPP

#include <Engine/RadiumEngine.hpp>
#include <Engine/Renderer/Renderer.hpp>

#include <Engine/Renderer/Passes/Passes.hpp>

namespace Ra
{
    namespace Engine
    {
        class RA_ENGINE_API ForwardRenderer : public Renderer
        {
        public:
            ForwardRenderer( uint width, uint height );
            virtual ~ForwardRenderer();

            virtual std::string getRendererName() const override { return "Forward Renderer"; }
            Ra::Core::MultiGraph<Pass>* getPassGraph() { return &m_passgraph; }

        protected:

            virtual void initializeInternal() override;
            virtual void resizeInternal() override;

            virtual void updateStepInternal( const RenderData& renderData ) override;

            virtual void postProcessInternal( const RenderData& renderData ) override;
            virtual void renderInternal( const RenderData& renderData ) override;
            virtual void debugInternal( const RenderData& renderData ) override;
            virtual void uiInternal( const RenderData& renderData ) override;

            virtual Core::MultiGraph<Pass>* getPassGraphRw() override;

            /// @brief check the usability of the graph and render it only if ok
            /// @note here are some conditions used to control this
            ///     - no pass have unconnected parameters
            ///     - no forest, only one graph
            bool checkPassGraph();

        private:
            void initShaders();
            void initBuffers();
            void initPasses();
            void initGraph();

        private:
            enum RendererTextures
            {
                TEX_DEPTH = 0,
                TEX_NORMAL,
                TEX_LIT,
                TEX_COUNT
            };

            // Default renderer logic here, no need to be accessed by overriding renderers.
            std::unique_ptr<FBO> m_fbo;

            uint m_pingPongSize;

            std::array<std::unique_ptr<Texture>, TEX_COUNT> m_textures;

            std::map<std::string, Pass*>       m_passmap;
            std::vector<std::shared_ptr<Pass>> m_passes;

            Ra::Core::MultiGraph<Pass> m_passgraph;
        };

    } // namespace Engine
} // namespace Ra

#endif // RADIUMENGINE_FORWARDRENDERER_HPP
