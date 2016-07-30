#include <Engine/ItemModel/ItemEntry.hpp>
#include <Engine/RadiumEngine.hpp>
#include <Engine/Entity/Entity.hpp>
#include <Engine/Component/Component.hpp>
#include <Engine/Managers/SystemDisplay/SystemDisplay.hpp>
#include <Engine/Managers/ObjectsManager/ObjectsManager.hpp>

namespace Ra
{
    namespace Engine
    {

        std::string getEntryName( const Engine::RadiumEngine* engine, const ItemEntry& ent )
        {
            std::string result = "Invalid Entry";
            
            if ( ent.isValid() )
            {
                auto mgr = engine->getObjectsManager();
                
                if ( ent.isRoNode() )
                {
                    result = mgr->getRenderObject(ent)->getName();
                }
                else if ( ent.isComponentNode() )
                {
                    result = mgr->getComponent(ent)->getName();
                }
                else if ( ent.isEntityNode() )
                {
                    result = mgr->getEntity(ent)->getName();
                }
            }
            
            return result;
        }

        std::vector<Ra::Core::Index> getItemROs(const Engine::RadiumEngine* engine, const ItemEntry& ent)
        {
            std::vector< Ra::Core::Index> result;
            if (ent.isValid())
            {
                auto mgr = engine->getObjectsManager();
                
                if ( ent.isRoNode() )                    
                {
                    result.push_back(ent.m_renderObject);
                }
                else if ( ent.isComponentNode() )
                {
                    result = mgr->getComponent(ent)->m_renderObjects;
                }
                else if ( ent.isEntityNode() )
                {                    
                    for ( const auto& c : mgr->getEntity(ent)->getComponents() )
                    {
                        result.insert(result.end(), c->m_renderObjects.begin(), c->m_renderObjects.end());
                    }
                }
            }
            
            return result;
        }

        bool ItemEntry::isValid() const
        {
            ON_DEBUG(checkConsistency());
            Engine::RadiumEngine* engine = Engine::RadiumEngine::getInstance();

            bool result = true;
            
            /*
              return m_entity != nullptr // It has an entity
              && engine->getEntityManager()->entityExists( m_entity->getName() ) // The entity exists
              && ((!isRoNode() || engine->getRenderObjectManager()->exists(m_roIndex))); // The RO exists
            */

            return result;
        }

        bool ItemEntry::isSelectable() const
        {
            bool result;
            
            Engine::RadiumEngine* engine = Engine::RadiumEngine::getInstance();
            auto mgr = engine->getObjectsManager();            
            
            if ( isRoNode() )
            {
                const bool isUI = mgr->getRenderObject(*this)->getType() == Engine::RenderObjectType::UI;
                const bool isDebug = mgr->getRenderObject(*this)->getType() == Engine::RenderObjectType::Debug;
                result = (!(isUI || isDebug));
            }
            else
            {
                result = m_entity != Engine::SystemEntity::getInstance()->idx;
            }

            return result;
        }

    }
}
