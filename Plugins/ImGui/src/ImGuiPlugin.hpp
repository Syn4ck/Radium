#ifndef IMGUIPLUGIN_PLUGIN_HPP_
#define IMGUIPLUGIN_PLUGIN_HPP_

#include <Core/CoreMacros.hpp>
/// Defines the correct macro to export dll symbols.
#if defined  Imgui_EXPORTS
    #define IMGUI_PLUGIN_API DLL_EXPORT
#elif defined Imgui_IMPORTS
    #define IMGUI_PLUGIN_API DLL_IMPORT
#else
    #define IMGUI_PLUGIN_API
#endif

#include <QObject>
#include <QtPlugin>

#include <Gui/PluginBase/RadiumPluginInterface.hpp>

class QLabel;

namespace Ra
{
    namespace Engine
    {
        class RadiumEngine;
    }
}

namespace ImGuiPlugin
{
    class ImGuiSystem;
    class ImGuiComponent;

    /**
     * \class ImGuiPlugin
     *
     * @brief The ImGuiPlugin class exports all the widgets to show in
     * the Qt toolbox and register the plugin to the Radium.
     *
     */

    class IMGUI_PLUGIN_API ImGuiPlugin :
            public QObject,
            Ra::Plugins::RadiumPluginInterface
    {
        Q_OBJECT
        Q_PLUGIN_METADATA( IID "RadiumEngine.PluginInterface" )
        Q_INTERFACES( Ra::Plugins::RadiumPluginInterface )

    public:
        ImGuiPlugin() {}
        virtual ~ImGuiPlugin() {}

        /// Registration to the plugin system.
        virtual void registerPlugin( Ra::Engine::RadiumEngine* engine ) override;

        /// Tells wether the plugin add a tab or not and set its name.
        virtual bool doAddWidget( QString& name ) override;

        /// Create the widget with this magnificient checkbox...
        /// @note On the long run, this could host a QList of options.
        virtual QWidget* getWidget() override;

        /// Tells wether the plugin add a menu entry or not.
        virtual bool doAddMenu() override;

        /// Create the QMenu.
        virtual QMenu* getMenu() override;

    public slots:
        /// Toggle the display of post-processing node editor.
        void togglePassesEditor( bool on );

        /// Toggle the display of the ImGui demo.
        void toggleDemoUI( bool on );

        // TODO(hugo) some load/save options, maybe

    private:
        ImGuiSystem*    m_system;
        ImGuiComponent* m_component;
    };

} // namespace

#endif // IMGUIPLUGIN_PLUGIN_HPP_
