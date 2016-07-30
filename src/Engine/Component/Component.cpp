#include <Engine/Component/Component.hpp>

#include <Engine/RadiumEngine.hpp>
#include <Engine/Entity/Entity.hpp>
#include <Engine/System/System.hpp>
#include <Engine/Renderer/RenderObject/RenderObject.hpp>
#include <Engine/Managers/ObjectsManager/ObjectsManager.hpp>

namespace Ra
{
    namespace Engine
    {
        Component::Component( const std::string& name )
                : m_name( name )
                , m_entity( nullptr )
                , m_system( nullptr )
        {
        }

        Component::~Component()
        {
            auto mgr = RadiumEngine::getInstance()->getObjectsManager();            

            for (auto entry : m_renderObjects)
            {
                mgr->removeRenderObject(entry);
            }

            m_renderObjects.clear();
        }

        void Component::removeRenderObject(Core::Index roIdx)
        {          
        }

        void Component::deactivate()
        {
//            auto mgr = 
        }
        
    }
} // namespace Ra
