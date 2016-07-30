#include <Engine/System/System.hpp>

#include <Core/String/StringUtils.hpp>
#include <Engine/Component/Component.hpp>
#include <Engine/Entity/Entity.hpp>
#include <Engine/Managers/ObjectsManager/ObjectsManager.hpp>

namespace Ra
{
    namespace Engine
    {
        System::System()
        {
        }

        System::~System()
        {
            auto mgr = RadiumEngine::getInstance()->getObjectsManager();
            for (auto entry : m_components)
            {
                getMgr()->getComponent(entry)->deactivate();
            }
        }

        void System::registerComponent(ItemEntry component)
        {
            // Perform checks on debug
            CORE_ASSERT(m_components.find(component) == m_components.end(), "Component already registered");

            m_components.push_back(component);
            getMgr()->getComponent(component)->setSystem(this);
        }

        void System::unregisterComponent(ItemEntry component)
        {
            auto pos = m_components.find(component);
            CORE_ASSERT( pos != m_components.end(), "Component is not registered." );

            getMgr()->getComponent(component)->deactivate();
            
            m_components.erase(pos);
        }


        void System::unregisterAllComponents(ItemEntry entity)
        {
            for (auto entry : getMgr()->getEntity(entity)->getComponents())
            {
                auto it = m_components.find(entry);
                if (it != m_components.end())
                {
                    unregisterComponent(*it);
                }
            }
            
            while ( (pos = std::find_if( m_components.begin(), m_components.end(),
                [entity]( const auto& pair ) {return pair.first == entity; } )) != m_components.end())
            {
                m_components.erase( pos );
            }
        }
    }
} // namespace Ra
