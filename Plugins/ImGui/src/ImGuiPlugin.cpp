#include <ImGuiPlugin.hpp>

#include <QCheckBox>
#include <QToolBar>
#include <QIcon>

#include <Engine/RadiumEngine.hpp>
#include <Engine/Managers/EntityManager/EntityManager.hpp>
#include <Engine/System/System.hpp>

#include <ImGuiSystem.hpp>
#include <ImGuiComponent.hpp>

namespace ImGuiPlugin {

    /// @brief registration to the plugin system
    void ImGuiPlugin::registerPlugin( Ra::Engine::RadiumEngine *engine )
    {
        m_system    = new ImGuiSystem();
        m_component = new ImGuiComponent( "HUD" );  // TODO(hugo) find a better name

        // attach display entity to the UI component
        Ra::Engine::Entity* ent = engine->getEntityManager()->getEntity( "System Display Entity" );
        m_component->setEntity( ent );
        m_system->registerComponent( ent, m_component );
    }

    /// @brief tells wether the plugin add a tab or not and set its name
    bool ImGuiPlugin::doAddWidget( QString &name )
    {
        name = "On-display UI";
        return true;
    }

    /// @brief create the widget with this magnificient checkbox...
    /// @note on the long run, this could host a QList with multiple ImGui widgets
    QWidget* ImGuiPlugin::getWidget()
    {
        QWidget* widget = new QWidget();
        QCheckBox* check = new QCheckBox("Pass Graph UI", widget);
        {
            // configure checkbox
            check->setCheckable(true);
            check->setChecked(true);     // TODO(hugo) this behavior is not the wanted one in production

            connect( check, &QCheckBox::stateChanged, this, &ImGuiPlugin::togglePassesEditor );
        }

        // ... more widgets

        return widget;
    }

    bool ImGuiPlugin::doAddMenu()
    {
        return false;
    }

    QMenu* ImGuiPlugin::getMenu()
    {
        return nullptr;
    }

    void ImGuiPlugin::togglePassesEditor(bool on)
    {
        m_component->setPassesEditor( on );
    }

}
