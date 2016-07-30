#ifndef RADIUMENGINE_OBJECTSMANAGER_HPP
#define RADIUMENGINE_OBJECTSMANAGER_HPP

#include <Engine/ItemModel/ItemEntry.hpp>

namespace Ra
{
    namespace Engine
    {
        class ObjectsManagerImpl;
        class Entity;
        class Component;
        class RenderObject;
        
        class RA_ENGINE_API ObjectsManager
        {
        public:
            // FIXME(charly): These values might very well change if needed
            static constexpr size_t MAX_ENTITIES      = 10000;
            static constexpr size_t MAX_COMPONENTS    = 10000;
            static constexpr size_t MAX_RENDEROBJECTS = 10000;
            
        public:
            ObjectsManager();
            ~ObjectsManager();

            Entity* getEntity(const ItemEntry& entry) const;
            Component* getComponent(const ItemEntry& entry) const;
            RenderObject* getRenderObject(const ItemEntry& entry) const;

            ItemEntry createEntity(const std::string& name);
            template <typename T, typename... Args>
            ItemEntry createComponent(const ItemEntry& entity, const std::string& name, Args... args);
            template <typename T, typename... Args>
            ItemEntry createRenderObject(const ItemEntry& component, const std::string& name,
                                         const RenderObjectType& type, Args... args);

            void deleteEntity(const ItemEntry& entry);
            void deleteComponent(const ItemEntry& entry);
            void deleteRenderObject(const ItemEntry& entry);
            
        private:
            ObjectsManager(const ObjectsManager&) = delete;
            void operator=(const ObjectsManager&) = delete;
            
        private:
            /// Defer everything to the Impl
            ObjectsManagerImpl* p_impl;
        };
    }
}

#endif // RADIUMENGINE_OBJECTSMANAGER_HPP

