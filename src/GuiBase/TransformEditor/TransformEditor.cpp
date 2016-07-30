#include <GuiBase/TransformEditor/TransformEditor.hpp>

#include <Engine/Entity/Entity.hpp>
#include <Engine/Component/Component.hpp>

// FIXME(charly): This is not doing anything for now
namespace Ra {
    namespace GuiBase
    {
        void TransformEditor::setEditable( const Engine::ItemEntry& ent )
        {
            m_currentEdit = ent;
            getTransform();
        }

        void TransformEditor::getTransform()
        {
#if 0
            if (canEdit())
            {
                if (m_currentEdit.isEntityNode())
                {
                    m_transform = m_currentEdit.m_entity->getTransform();
                }
                else if (m_currentEdit.isComponentNode() || m_currentEdit.isRoNode())
                {
                    m_transform = m_currentEdit.m_component->getTransform(m_currentEdit.m_roIndex);
                }
            }
#endif
        }

        bool TransformEditor::canEdit() const
        {
            return false;
#if 0
            return m_currentEdit.isValid() && m_currentEdit.isSelectable() && ( m_currentEdit.isEntityNode() || m_currentEdit.m_component->canEdit(m_currentEdit.m_roIndex));
#endif
        }

        void TransformEditor::setTransform( const Ra::Core::Transform& tr )
        {
#if 0
            if ( canEdit() )
            {
                if (m_currentEdit.isEntityNode())
                {
                    m_currentEdit.m_entity->setTransform( tr );
                }
                else
                {
                    m_currentEdit.m_component->setTransform(m_currentEdit.m_roIndex, tr );
                }
            }
#endif
        }

        Core::Transform TransformEditor::getWorldTransform() const
        {
            return Core::Transform();
#if 0
            return m_currentEdit.isEntityNode() ?
                   Ra::Core::Transform::Identity() :
                   m_currentEdit.m_entity->getTransform();
#endif
        }

        TransformEditor::~TransformEditor()
        {
#if 0
            setEditable( Engine::ItemEntry() );
#endif
        }
    }
}

