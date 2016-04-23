#include <Gui/Gizmo/GizmoManager.hpp>

#include <Core/Math/ColorPresets.hpp>

#include <Engine/Renderer/Camera/Camera.hpp>

#include <Gui/Camera/CameraInterface.hpp>
#include <Gui/Gizmo/TranslateGizmo.hpp>
#include <Gui/Gizmo/RotateGizmo.hpp>
#include <Gui/Viewer.hpp>

namespace Ra
{
    namespace Guibase
    {
        GizmoManager::GizmoManager(const Viewer* viewer)
                : m_viewer(viewer)
                , m_currentEdit(nullptr)
                , m_currentGizmo(nullptr)
                , m_currentGizmoType(NONE)
                , m_mode(Gizmo::GLOBAL)
        {}


        GizmoManager::~GizmoManager() { }

        void GizmoManager::setEditable(Engine::EditableInterface* edit)
        {
            m_currentEdit = edit;
            getTransform();
            spawnGizmo();
        }

        void GizmoManager::spawnGizmo()
        {
            m_currentGizmo.reset(nullptr);
            if (m_currentEdit)
            {
                switch (m_currentGizmoType)
                {
                    case NONE:
                    {
                        break;
                    }
                    case TRANSLATION:
                    {
                        // FIXME DebugCMP
                        m_currentGizmo.reset(new TranslateGizmo(Engine::SystemEntity::uiCmp(), m_currentEdit->getWorldTransform(), m_transform, m_mode));
                        break;
                    }
                    case ROTATION:
                    {
                        m_currentGizmo.reset(new RotateGizmo(Engine::SystemEntity::uiCmp(), m_currentEdit->getWorldTransform(), m_transform, m_mode));
                        break;
                    }
                    case SCALE:
                        break ; // not implemented yet...
                }
            }
        }

        void GizmoManager::setLocal(bool useLocal)
        {
            m_mode = useLocal ? Gizmo::LOCAL : Gizmo::GLOBAL;
            spawnGizmo();
        }

        void GizmoManager::changeGizmoType(GizmoManager::GizmoType type)
        {
            m_currentGizmoType = type;
            spawnGizmo();
        }

        // Todo (val) : this method is common with the transform edit widget and should be factored.
        void GizmoManager::getTransform()
        {
            if (m_currentEdit)
            {
                Core::AlignedStdVector<Engine::EditableProperty> props;
                m_currentEdit->getProperties(props);
                int transformFound = -1;
                for (uint i = 0; i < props.size(); ++i)
                {
                    if (props[i].type == Engine::EditableProperty::TRANSFORM)
                    {
                        transformFound = i;
                        break;
                    }
                }
                if (transformFound >= 0)
                {
                    m_transformProperty = props[transformFound];
                    Core::Transform transform = Core::Transform::Identity();
                    // Grab translation and rotation..
                    for (const auto& p : m_transformProperty.primitives)
                    {
                        if (p.primitive.getType() == Engine::EditablePrimitive::POSITION)
                        {
                            transform.translation() = p.primitive.asPosition();
                        }

                        if (p.primitive.getType() == Engine::EditablePrimitive::ROTATION)
                        {
                            transform.linear() = p.primitive.asRotation().toRotationMatrix();
                        }
                    }
                    m_transform = transform;
                }
            }
        }

        void GizmoManager::updateValues()
        {
            getTransform();
            if(m_currentGizmo)
            {
                m_currentGizmo->updateTransform(m_currentEdit->getWorldTransform(), m_transform);
            }
        }

        bool GizmoManager::handleMouseEvent(const Core::MouseEvent* event)
        {
            switch (event->event)
            {
                case Core::MouseEvent_Pressed:
                {
                    if (event->buttons != Core::MouseButton_Left || !m_currentEdit || m_currentGizmoType == NONE)
                    {
                        return false;
                    }

                    // If we are there it means that we should have a valid gizmo.
                    CORE_ASSERT(m_currentGizmo, "Gizmo is not there !");

                    // Access the camera from the viewer. (TODO : a cleaner way to access the camera).
                    const Engine::Camera& cam = *m_viewer->getCurrentCamera()->getCamera();
                    m_currentGizmo->setInitialState(cam, Core::Vector2(event->absoluteXPosition, event->absoluteYPosition));

                    return true;
                }

                case Core::MouseEvent_Released:
                {
                    if ( event->buttons == Core::MouseButton_Left && m_currentGizmo)
                    {
                        m_currentGizmo->selectConstraint(-1);
                    }

                    return (m_currentGizmo != nullptr);
                }

                case Core::MouseEvent_Moved:
                {
                    if (event->buttons & Core::MouseButton_Left && m_currentGizmo )
                    {
                        Core::Vector2 currentXY(event->absoluteXPosition, event->absoluteYPosition);
                        const Engine::Camera& cam = *m_viewer->getCurrentCamera()->getCamera();
                        Core::Transform newTransform = m_currentGizmo->mouseMove(cam, currentXY);

                        for (auto& prim : m_transformProperty.primitives)
                        {
                            switch (prim.primitive.getType())
                            {
                                case Engine::EditablePrimitive::POSITION:
                                {
                                    prim.primitive.asPosition() = newTransform.translation();
                                    break;
                                }
                                case Engine::EditablePrimitive::ROTATION:
                                {
                                    prim.primitive.asRotation() = newTransform.rotation();
                                    break;
                                }
                                default:; // do nothing;
                            }
                        }

                        CORE_ASSERT(m_currentEdit, "Nothing to edit ");
                        m_currentEdit->setProperty(m_transformProperty);
                    }
                    return (m_currentGizmo != nullptr);
                }
            }

            return false;
        }

        void GizmoManager::handlePickingResult(int drawableId)
        {
            if (m_currentGizmo)
            {
                m_currentGizmo->selectConstraint( drawableId );
            }
        }
    }
}
