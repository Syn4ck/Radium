#include <Engine/Renderer/RaytracingRenderer.hpp>

#include <Engine/Renderer/Renderer.hpp>
#include <Engine/RadiumEngine.hpp>
#include <Engine/Renderer/Mesh/Mesh.hpp>
#include <Core/RaCore.hpp>
#include <Core/Containers/VectorArray.hpp>

namespace Ra
{
    namespace Engine
    {
        RaytracingRenderer::RaytracingRenderer( uint width, uint height )
            : Renderer(width, height) {}

        void RaytracingRenderer::initialize()
        {
            Renderer::initialize();
        }

        void RaytracingRenderer::render( const RenderData& data )
        {
            CORE_ASSERT( RadiumEngine::getInstance() != nullptr, "Engine is not initialized." );

            std::lock_guard<std::mutex> renderLock( m_renderMutex );

            m_timerData.renderStart = Core::Timer::Clock::now();

            // 1. Gather render objects and update them
            std::vector<RenderObjectPtr> renderObjects;
            RadiumEngine::getInstance()->getRenderObjectManager()->getRenderObjects( renderObjects );
            updateRenderObjectsInternal( data, renderObjects );
            m_timerData.updateEnd = Core::Timer::Clock::now();

            // 4. Do the rendering.
            renderInternal( data );
            m_timerData.mainRenderEnd = Core::Timer::Clock::now();

            // 5. Post processing
            //postProcessInternal( data );
            //m_timerData.postProcessEnd = Core::Timer::Clock::now();

            // 6. write image to framebuffer.
            drawScreenInternal();
            m_timerData.renderEnd = Core::Timer::Clock::now();
        }

        void RaytracingRenderer::renderInternal( const RenderData& renderData )
        {

        }

        void RaytracingRenderer::updateRenderObjectsInternal( const RenderData& renderData,
                                              const std::vector<RenderObjectPtr>& renderObjects )
        {
            if (m_kdtree) {
                m_kdtree.release();
            }

            m_kdtree.reset(new Core::KDTree()) ;

            Core::VectorArray<Core::Vector4> data ;
            for ( auto& ro : renderObjects )
            {
                const Mesh * mesh = ro->getMesh();
                const Core::Vector4Array& prims = mesh->getData(Mesh::VERTEX_POSITION);
                data.insert(data.end(), prims.begin(), prims.end());
            }

            m_kdtree->build(data);
        }
    }
}
