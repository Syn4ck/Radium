#ifndef RADIUMENGINE_ENGINE_HPP
#define RADIUMENGINE_ENGINE_HPP

#include <Engine/RaEngine.hpp>

#include <mutex>
#include <map>
#include <string>
#include <memory>
#include <vector>

#include <Core/Log/Log.hpp>
#include <Core/Event/Key.hpp>
#include <Core/Event/EventEnums.hpp>

#include <Engine/Renderer/RenderObject/RenderObjectManager.hpp>
#include <Engine/Managers/EntityManager/EntityManager.hpp>
#include <Engine/Managers/SignalManager/SignalManager.hpp>

namespace Ra
{
    namespace Core
    {
        class TaskQueue;
        struct MouseEvent;
        struct KeyEvent;
    }
}
namespace Ra
{
    namespace Engine
    {
        class System;
        class Entity;
        class Component;
    }
}

namespace Ra
{
    namespace Engine
    {
        struct InputStatus
        {
            std::array<int, Core::Key_Count> keyIsPressed   = {{ false }};
            std::array<int, Core::Key_Count> keyWasPressed  = {{ false }};
            std::array<int, Core::Key_Count> keyWasReleased = {{ false }};

            std::array<int, Core::MouseButton_Count> buttonIsPressed   = {{ false }};
            std::array<int, Core::MouseButton_Count> buttonWasPressed  = {{ false }};
            std::array<int, Core::MouseButton_Count> buttonWasReleased = {{ false }};

            Core::Vector2 mousePosition;
            Core::Vector2 wheelDelta;
            int modifiers = 0;

#define FILL_ARRAY(array, value) { std::fill(array.begin(), array.end(), value); }
            void resetFrameEvents()
            {
                FILL_ARRAY(keyWasPressed, 0);
                FILL_ARRAY(keyWasReleased, 0);
                FILL_ARRAY(buttonWasPressed, 0);
                FILL_ARRAY(buttonWasReleased, 0);

                wheelDelta = Core::Vector2::Zero();
            }
#undef FILL_ARRAY
        };

        class RA_ENGINE_API RadiumEngine
        {
            RA_SINGLETON_INTERFACE(RadiumEngine);
        public:
            RadiumEngine();
            ~RadiumEngine();

            void initialize();
            void cleanup();

            void getTasks( Core::TaskQueue* taskQueue, Scalar dt );

            void registerSystem( const std::string& name,
                                 System* system );
            System* getSystem( const std::string& system ) const;

            bool loadFile( const std::string& file );

            /// Is called at the end of the frame to synchronize any data
            /// that may have been updated during the frame's multithreaded processing.
            void endFrameSync();

            /// Manager getters
            RenderObjectManager*  getRenderObjectManager()  const;
            EntityManager*        getEntityManager()        const;
            SignalManager*        getSignalManager()        const;

            inline const InputStatus& getInputStatus() const { return m_inputStatus; }
            inline InputStatus& getInputStatus() { return m_inputStatus; }

        private:
            std::map<std::string, std::shared_ptr<System>> m_systems;

            std::unique_ptr<RenderObjectManager> m_renderObjectManager;
            std::unique_ptr<EntityManager>       m_entityManager;
            std::unique_ptr<SignalManager>       m_signalManager;

            InputStatus m_inputStatus;
        };

    } // namespace Engine
} // namespace Ra

#endif // RADIUMENGINE_ENGINE_HPP
