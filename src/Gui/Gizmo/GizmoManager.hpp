#ifndef RADIUMENGINE_GIZMO_MANAGER_HPP_
#define RADIUMENGINE_GIZMO_MANAGER_HPP_

#include <Engine/Component/Component.hpp>
#include <Engine/Managers/SystemDisplay/SystemDisplay.hpp>

#include <Gui/Gizmo/Gizmo.hpp>

namespace Ra
{
    namespace Gui
    {
        class Viewer;

        /// This class interfaces the gizmos with the ui commands.
        /// It allows to change the gizmo type when editing an editable transform property.
        class RA_GUIBASE_API GizmoManager
        {
        public:

            RA_CORE_ALIGNED_NEW;
            enum GizmoType
            {
                NONE,
                TRANSLATION,
                ROTATION,
                SCALE,
            };

        public:
            explicit GizmoManager(const Viewer* viewer);
            ~GizmoManager();

            /// Receive mouse events and transmit them to the gizmos.
            void handleEvents();

            /// Change the current editable object,
            void setEditable(Engine::EditableInterface* edit);

            /// Callback when a drawable is picked.
            void handlePickingResult( int drawableId );

            /// Change mode from local axis to global
            void setLocal(bool useLocal);

            /// Change gizmo type (rotation or translation)
            void changeGizmoType( GizmoType type );

            /// Retreive the transform from the editable and update the gizmos.
            void updateValues();

        private:
            // Helper to get the transform property from the editable.
            void getTransform();

            // Helper factory method to create the right gizmo.
            void spawnGizmo();

        private:
            const Viewer* m_viewer;
            Core::Transform m_transform; //! The transform being edited.

            Engine::EditableInterface* m_currentEdit; //! The current editable being edited.
            Engine::EditableProperty m_transformProperty; //! A copy of the Transform property

            std::unique_ptr<Gizmo> m_currentGizmo;  //! Owning pointer to the gizmo
            GizmoType m_currentGizmoType;           //! Type of the gizmo
            Gizmo::Mode m_mode;                     //! Local/global axis mode.
        };
    }
}


#endif // RADIUMENGINE_GIZMO_MANAGER_HPP_
