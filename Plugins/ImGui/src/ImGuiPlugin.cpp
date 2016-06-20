#include <ImGuiPlugin.hpp>

#include <QBoxLayout>
#include <QCheckBox>
#include <QToolBar>
#include <QIcon>

#include <Engine/RadiumEngine.hpp>
#include <Engine/Managers/EntityManager/EntityManager.hpp>
#include <Engine/System/System.hpp>

#include <ImGuiSystem.hpp>
#include <ImGuiComponent.hpp>

namespace ImGuiPlugin {

    void ImGuiPlugin::registerPlugin( Ra::Engine::RadiumEngine *engine )
    {
        m_system    = new ImGuiSystem();
        m_component = new ImGuiComponent( "HUD" );  // TODO(hugo) find a better name

        m_component->m_engine = engine;
        m_component->initialize();

        // attach display entity to the UI component
        Ra::Engine::Entity* ent = engine->getEntityManager()->getEntity( "System Display Entity" );
        m_component->setEntity( ent );
        m_system->registerComponent( ent, m_component );
    }

    bool ImGuiPlugin::doAddWidget( QString &name )
    {
        name = "On-display UI";
        return true;
    }

    QWidget* ImGuiPlugin::getWidget()
    {
        QWidget* widget = new QWidget();

        QCheckBox* check_passEditor = new QCheckBox("Pass Graph UI", widget);
        {
            // configure checkbox
            check_passEditor->setCheckable(true);
            check_passEditor->setChecked(false);     // TODO(hugo) this behavior is not the wanted one in production

            connect( check_passEditor, &QCheckBox::stateChanged, this, &ImGuiPlugin::togglePassesEditor );
        }

        QCheckBox* check_demoUI = new QCheckBox("Demo UI", widget);
        {
            // configure checkbox
            check_demoUI->setCheckable(true);
            check_demoUI->setChecked(true);

            connect( check_demoUI, &QCheckBox::stateChanged, this, &ImGuiPlugin::toggleDemoUI );
        }

        // ... more widgets

        QVBoxLayout *layout = new QVBoxLayout;
        layout->addWidget(check_passEditor);
        layout->addWidget(check_demoUI);

        widget->setLayout(layout);

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

    void ImGuiPlugin::toggleDemoUI(bool on)
    {
        m_component->setDemoUI( on );
    }

}
