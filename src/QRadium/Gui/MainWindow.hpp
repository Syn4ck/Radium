#ifndef RADIUMENGINE_MAINWINDOW_HPP
#define RADIUMENGINE_MAINWINDOW_HPP

#include <QMainWindow>
#include "ui_MainWindow.h"

#include <qdebug.h>
#include <QEvent>

#include <QRadium/TimerData/FrameTimerData.hpp>

namespace Ra
{
    namespace Engine
    {
        class Entity;
    }
}

namespace Ra
{
    namespace Plugins
    {
        class RadiumPluginInterface;
    }
}

namespace QRadium
{
    class EntityTreeModel;
    class Viewer;
    class MaterialEditor;

    // Just forwards everything to the viewer.
    class MainWindow : public QMainWindow, private Ui::MainWindow
    {
        Q_OBJECT

    public:
        explicit MainWindow( QWidget* parent = nullptr );
        virtual ~MainWindow();

        void initialize();

        QRadium::Viewer* getViewer();

        inline const std::vector<Ra::Core::KeyEvent>& getKeyEvents() const;
        inline const std::vector<Ra::Core::MouseEvent>& getMouseEvents() const;

        void updateUi( Ra::Plugins::RadiumPluginInterface* plugin );

        void onFrameComplete();

    private slots:
        void loadFile();
        void openMaterialEditor();

        void handlePicking( int drawableIndex );

    public slots:
        void onEntitiesUpdated();

        void onRendererReady();

        // Frame timers ui slots
        void onUpdateFramestats( const std::vector<Ra::FrameTimerData>& stats );
        void framesCountForStatsChanged(int count);

        // Selection tools
        void onSelectionChanged( const QItemSelection& selected, const QItemSelection& deselected );

        // Gizmo buttons slots
        void gizmoShowNone();
        void gizmoShowTranslate();
        void gizmoShowRotate();
        //void gizmoShowScale();

        void displayEntityRenderObjects( Ra::Engine::Entity* entity );
        void displayComponentRenderObjects( Ra::Engine::Component* component );

        void renderObjectListItemClicked( int item );
        void changeRenderObjectShader( const QString& shaderName );

        void toggleVisisbleRO();
        void toggleXRayRO();
        void editRO();

        void cleanup();

    signals:
        void closed();
        void fileLoading( const QString path );
        void loadComplete();
        void framescountForStatsChanged( int count );

        void selectedEntity( Ra::Engine::Entity* selectedEntity );
        void selectedComponent( Ra::Engine::Component* selectedComponent);

    private:
        // Basic I/O management
        // Intercept key events from Qt

        virtual void keyPressEvent( QKeyEvent* event ) override;
        virtual void keyReleaseEvent( QKeyEvent* event ) override;

        virtual void closeEvent( QCloseEvent* event ) override;

        void displayRenderObjects( Ra::Engine::Component* component );

        std::shared_ptr<Ra::Engine::RenderObject> getSelectedRO();

    private:
        EntityTreeModel* m_entityTreeModel;
        MaterialEditor* m_materialEditor;

        int m_lastSelectedRO;
    };

} // namespace QRadium

#endif // RADIUMENGINE_MAINWINDOW_HPP
