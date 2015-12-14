#ifndef RADIUMENGINE_RAYTRACINGRENDERER_H
#define RADIUMENGINE_RAYTRACINGRENDERER_H

#include <Engine/Renderer/Renderer.hpp>

#include <Core/BSP/KDTree.hpp>

namespace Ra
{
    namespace Engine
    {

        class RaytracingRenderer : public Renderer {
        public:
            RaytracingRenderer( uint width, uint height );

            virtual void initialize() override;

            virtual void render( const RenderData& renderData ) override;

            virtual void renderInternal( const RenderData& renderData ) override;

            virtual void updateRenderObjectsInternal( const RenderData& renderData,
                                                      const std::vector<RenderObjectPtr>& renderObjects ) override;

        protected:

            std::unique_ptr<Core::KDTree> m_kdtree ;
        };
    } // namespace Engine
} // namespace Ra

#endif // RADIUMENGINE_RAYTRACINGRENDERER_H
