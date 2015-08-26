#ifndef FANCYMESHPLUGIN_FANCYMESHSYSTEM_HPP
#define FANCYMESHPLUGIN_FANCYMESHSYSTEM_HPP

#include <Plugins/FancyMeshPlugin/FancyMeshPlugin.hpp>
#include <Engine/Entity/System.hpp>


namespace Ra
{
    namespace Core
    {
        struct TriangleMesh;
    }
}

namespace Ra
{
    namespace Engine
    {
        class Entity;
        struct RenderTechnique;
        class Component;
    }
}

namespace FancyMeshPlugin
{
    class FancyMeshComponent;
}

namespace FancyMeshPlugin
{
    class FM_PLUGIN_API FancyMeshSystem : public Ra::Engine::System
    {
    public:
        FancyMeshSystem();
        virtual ~FancyMeshSystem();

        virtual void initialize() override;
        virtual void handleDataLoading( Ra::Engine::Entity* entity, const std::string& rootFolder,
                                        const std::map<std::string, Ra::Core::Any>& data ) override;

        virtual Ra::Engine::Component* addComponentToEntity( Ra::Engine::Entity* entity ) override;

        virtual void generateTasks( Ra::Core::TaskQueue* taskQueue, const Ra::Engine::FrameInfo& frameInfo ) override;

        // Specialized factory methods for this systems.
        FancyMeshComponent* addFancyMeshToEntity( Ra::Engine::Entity* entity, const Ra::Core::TriangleMesh& mesh );
        FancyMeshComponent* addFancyMeshToEntity( Ra::Engine::Entity* entity,
                                                  const Ra::Core::TriangleMesh& mesh,
                                                  Ra::Engine::RenderTechnique* technique );
    };

} // namespace FancyMeshPlugin

#endif // FANCYMESHPLUGIN_FANCYMESHSYSTEM_HPP