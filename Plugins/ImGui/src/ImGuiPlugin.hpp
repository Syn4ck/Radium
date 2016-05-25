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

    class ImGuiPlugin : public QObject, Ra::Plugins::RadiumPluginInterface
    {
        Q_OBJECT
        Q_PLUGIN_METADATA( IID "RadiumEngine.PluginInterface" )
        Q_INTERFACES( Ra::Plugins::RadiumPluginInterface )

    public:
        ImGuiPlugin() {}
        virtual ~ImGuiPlugin() {}

        virtual void registerPlugin( Ra::Engine::RadiumEngine* engine ) override;

        virtual bool doAddWidget( QString& name ) override;
        virtual QWidget* getWidget() override;

        virtual bool doAddMenu() override;
        virtual QMenu* getMenu() override;

    public slots:
        void togglePassesEditor( bool on );
        void toggleDemoUI( bool on );
        // TODO(hugo) some load/save options, maybe

    private:
        ImGuiSystem*    m_system;
        ImGuiComponent* m_component;
    };

} // namespace

#endif // IMGUIPLUGIN_PLUGIN_HPP_
