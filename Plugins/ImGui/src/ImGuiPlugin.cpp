#include <ImGuiPlugin.hpp>

#include <QCheckBox>
#include <QAction>
#include <QIcon>
#include <QToolBar>

#include <Engine/RadiumEngine.hpp>
#include <ImGuiSystem.hpp>

namespace ImGuiPlugin {

    /// @brief registration to the plugin system
    void ImGuiPlugin::registerPlugin( Ra::Engine::RadiumEngine *engine )
    {
        m_system = new ImGuiSystem();
        engine->registerSystem( "ImGuiSystem", m_system );
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
            check->setChecked(false);

            // create actions for this checkbox
            QAction* togglePassesEditor = new QAction("Toggle On-display UI", widget);
            connect( togglePassesEditor, &QAction::toggled, this, &ImGuiPlugin::togglePassesEditor );

            // bind action to checkbox
            check->addAction(togglePassesEditor);
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
        m_system->setPassesEditor( on );
    }

}
