#include <Engine/Component/Component.hpp>

namespace Ra
{
    namespace Engine
    {
        inline void Component::setEntity(Core::Index entity)
        {
            m_entity = entity;
        }

        inline Core::Index Component::getEntity() const
        {
            return m_entity;
        }

        inline const std::string& Component::getName() const
        {
            return m_name;
        }
        
        inline void Component::addRenderObject(Core::Index roIdx)
        {
            m_renderObjects.push_back(ItemEntry{m_entity, idx, roIdx});
        }

        inline void Component::setSystem( System* system )
        {
            m_system = system;
        }

        inline System* Component::getSystem() const
        {
            return m_system;
        }
    }
} // namespace Ra
