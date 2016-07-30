#include <Engine/Managers/ObjectsManager/ObjectsManager.hpp>

#include <vector>
#include <array>

#include <Engine/Entity/Entity.hpp>
#include <Engine/Component/Component.hpp>
#include <Engine/Renderer/RenderObject/RenderObject.hpp>
#include <Engine/ItemModel/ItemEntry.hpp>

#include "ObjectsManagerImpl.cpp"

namespace Ra
{
    namespace Engine
    {
        template <typename T, size_t MAX_ITEMS>
        struct Array
        {            
            std::array<std::unique_ptr<T>, MAX_ITEMS> m_items;
            std::vector<int> m_occupiedList;
            std::vector<int> m_freeList;

            Array()
            {
                m_freeList.reserve(MAX_ITEMS);
                m_occupiedList.reserve(MAX_ITEMS);
                
                for (int i = 0; i < MAX_ITEMS; ++i)
                {
                    m_freeList.push_back(i);
                }
            }

            bool isValid(int index) const
            {
                bool result;

                auto it = std::find(m_occupiedList.begin(), m_occupiedList.end(), index);
                result = (it != m_occupiedList.end());
                
                return result;
            }

            template <typename... Args>
            T* addItem(Args... args)
            {
                int index = *(m_freeList.begin());
                T* result;
                
                m_freeList.erase(m_freeList.begin());
                m_occupiedList.push_back(result);
                std::sort(m_occupiedList.begin(), m_occupiedList.end());

                m_items[index].reset(new T(args...));
                result = m_items[index].get();
                
                return result;
            }

            void removeItem(int index)
            {
                CORE_ASSERT(m_occupiedList.find(index) != m_occupiedList.end(), "Index not existing");

                m_occupiedList.erase(index);
                m_freeList.push_back(index);
            }
        };

        using EntitiesArray     = Array<Entity, ObjectsManager::MAX_ENTITIES>;
        using ComponentsArray   = Array<Component, ObjectsManager::MAX_COMPONENTS>;
        using RenderObjectArray = Array<RenderObject, ObjectsManager::MAX_RENDEROBJECTS>;
        
        class ObjectsManagerImpl
        {
        public:
            ObjectsManagerImpl() = default;
            ~ObjectsManagerImpl() = default;

            ItemEntry createEntity();
            template <typename T, typename... Args>
            ItemEntry createComponent(const ItemEntry& entity, Args... args);
            template <typename T = RenderObject, typename... Args>
            ItemEntry createRenderObject(const ItemEntry& component, Args... args);

            Entity* getEntity(const ItemEntry& entry) const;
            Component* getComponent(const ItemEntry& entry) const;
            RenderObject* getRenderObject(const ItemEntry& entry) const;

            void deleteEntity(const ItemEntry& entry) const;
            void deleteComponent(const ItemEntry& entry) const;
            void deleteRenderObject(const ItemEntry& entry) const;

        private:
            EntitiesArray m_entities;
            ComponentsArray m_components;
            RenderObjectArray m_renderObjects;
        };

        ItemEntry ObjectsManagerImpl::createEntity(const std::string& name)
        {
            ItemEntry result;
            
            Core::Index index = m_entities.addItem();
            m_entities.m_items[entity_index].reset(new Entity(name));
            

            entity->idx = index;
            
            return ItemEntry{index, , };
        }

        template <typename T, typename... Args>
        ItemEntry ObjectsManagerImpl::createComponent(const ItemEntry& entity, Args... args)
        {
            return ItemEntry();
        }

        template <typename T, typename... Args>
        ItemEntry ObjectsManagerImpl::createRenderObject(const ItemEntry& component, Args... args)
        {
            return ItemEntry();
        }

        Entity* ObjectsManagerImpl::getEntity(const ItemEntry& entry) const
        {
            CORE_ASSERT(m_entities.isValid(entry.m_entity), "Entity does not exist");
            Entity* result;

            return result;
        }
        
        Component* ObjectsManagerImpl::getComponent(const ItemEntry& entry) const
        {
            CORE_ASSERT(m_entities.isValid(entry.m_component), "Component does not exist");
            Component* result;

            return result;
        }
        
        RenderObject* ObjectsManagerImpl::getRenderObject(const ItemEntry& entry) const
        {
            CORE_ASSERT(m_renderObjects.isValid(entry.m_renderObject), "RenderObject does not exist");
            RenderObject* result;

            return result;
        }
        
        void ObjectsManagerImpl::deleteEntity(const ItemEntry& entry) const
        {
        }
        
        void ObjectsManagerImpl::deleteComponent(const ItemEntry& entry) const
        {
        }
        
        void ObjectsManagerImpl::deleteRenderObject(const ItemEntry& entry) const
        {
        }

    }
}

namespace Ra
{
    namespace Engine
    {
                
        ObjectsManager::ObjectsManager()
        {
            p_impl = new ObjectsManagerImpl;
        }

        ObjectsManager::~ObjectsManager()
        {
            delete p_impl;
        }

        Entity* ObjectsManager::getEntity(const ItemEntry& entry) const
        {
            Entity* result = p_impl->getEntity(entry);
            return result;
        }

        Component* ObjectsManager::getComponent(const ItemEntry& entry) const
        {
            Component* result = p_impl->getComponent(entry);
            return result;
        }

        RenderObject* ObjectsManager::getRenderObject(const ItemEntry& entry) const
        {
            RenderObject* result = p_impl->getRenderObject(entry);
            return result;
        }
        
        ItemEntry ObjectsManager::createEntity()
        {
            ItemEntry result = p_impl->createEntity();
            return result;
        }

        template <typename T, typename... Args>
        ItemEntry ObjectsManager::createComponent(const ItemEntry& entity, Args... args)
        {
            ItemEntry result = p_impl->createComponent<T>(entity, args);
            return result;
        }

        template <typename T, typename... Args>
        ItemEntry ObjectsManager::createRenderObject(const ItemEntry& component, Args... args)
        {
            ItemEntry result = p_impl->createRenderObject<T>(component, args);
            return result;
        }
        
        void ObjectsManager::deleteEntity(const ItemEntry& entry)
        {
            p_impl->deleteEntity(entry);
        }
        
        void ObjectsManager::deleteComponent(const ItemEntry& entry)
        {
            p_impl->deleteComponent(entry);
        }
        
        void ObjectsManager::deleteRenderObject(const ItemEntry& entry)
        {
            p_impl->deleteRenderObject(entry);
        }
        
    }
}
