#include "ItemEntry.hpp"

namespace Ra
{
    namespace Engine
    {       
        inline bool ItemEntry::isEntityNode() const
        {
            ON_DEBUG(checkConsistency());
            return (m_entity.isValid() && m_component.isInvalid());
        }

        inline bool ItemEntry::isComponentNode() const
        {
            ON_DEBUG(checkConsistency());
            return (m_entity.isValid() && m_component.isValid() && m_renderObject.isInvalid());
        }

        inline bool ItemEntry::isRoNode() const
        {
            ON_DEBUG(checkConsistency());
            return (m_entity.isValid() && m_component.isValid() && m_renderObject.isValid());
        }


        inline void ItemEntry::checkConsistency() const
        {
            CORE_ASSERT(m_entity.isValid() || (m_component.isInvalid() && m_renderObject.isInvalid()),
                        "Component or RO  is set while entity is not");
            CORE_ASSERT(m_component.isValid() || m_renderObject.isInvalid(),
                        "RO  is set while component is not");
        }

        inline bool ItemEntry::operator==( const ItemEntry& rhs ) const
        {
            return m_entity == rhs.m_entity
                && m_component == rhs.m_component
                && m_renderObject == rhs.m_renderObject;
        }



    }
}
