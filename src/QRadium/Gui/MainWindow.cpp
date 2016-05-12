#include <QRadium/Gui/MainWindow.hpp>

#include <QFileDialog>
#include <QToolButton>
#include <QCloseEvent>

#include <Engine/RadiumEngine.hpp>
#include <Engine/Component/Component.hpp>
#include <Engine/Entity/Entity.hpp>
#include <Engine/Renderer/Renderer.hpp>
#include <Engine/Renderer/RenderTechnique/RenderTechnique.hpp>
#include <Engine/Renderer/RenderTechnique/ShaderProgram.hpp>
#include <Engine/Renderer/RenderTechnique/ShaderConfigFactory.hpp>

#include <Gui/Gizmo/GizmoManager.hpp>
#include <Gui/PluginBase/RadiumPluginInterface.hpp>

#include <QRadium/MainApplication.hpp>
#include <QRadium/Gui/EntityTreeModel.hpp>
#include <QRadium/Gui/EntityTreeItem.hpp>
#include <QRadium/Gui/MaterialEditor.hpp>

#include <assimp/Importer.hpp>

namespace QRadium
{
    MainWindow::MainWindow( QWidget* parent )
        : QMainWindow( parent )
    {
        // Note : at this point most of the components (including the Engine) are
        // not initialized. Listen to the "started" signal.

        setupUi( this );

        setWindowIcon( QPixmap( ":/Assets/Images/RadiumIcon.png" ) );
        setWindowTitle( QString( "Radium Engine" ) );

        QStringList headers;
        headers << tr( "Entities -> Components" );
        m_entityTreeModel = new EntityTreeModel( headers );
        m_entitiesTreeView->setModel( m_entityTreeModel );

        m_materialEditor = new MaterialEditor();

        m_lastSelectedRO = -1;
    }

    MainWindow::~MainWindow()
    {
        // Child QObjects will automatically be deleted
    }


    void MainWindow::cleanup()
    {
        gizmoShowNone();
    }

    void MainWindow::initialize()
    {
        connect( actionOpenMesh, &QAction::triggered, this, &MainWindow::loadFile );
        connect( actionReload_Shaders, &QAction::triggered, m_viewer, &Viewer::reloadShaders );
        connect( actionOpen_Material_Editor, &QAction::triggered, this, &MainWindow::openMaterialEditor );

        // Toolbox setup
        connect( actionToggle_Local_Global, &QAction::toggled, m_viewer, &Viewer::setGizmoLocal );
        connect( actionGizmoOff,            &QAction::triggered, this, &MainWindow::gizmoShowNone );
        connect( actionGizmoTranslate,      &QAction::triggered, this, &MainWindow::gizmoShowTranslate );
        connect( actionGizmoRotate,         &QAction::triggered, this, &MainWindow::gizmoShowRotate );

        // Loading setup.
        connect(this, &MainWindow::loadComplete, this, &MainWindow::onEntitiesUpdated);

        // Side menu setup.
        connect( m_entityTreeModel, &EntityTreeModel::dataChanged, m_entityTreeModel, &EntityTreeModel::handleRename );

        connect( m_entitiesTreeView->selectionModel(), &QItemSelectionModel::selectionChanged,
            this, &MainWindow::onSelectionChanged );

        // Connect picking results (TODO Val : use events to dispatch picking directly)
        connect(m_viewer, &Viewer::rightClickPicking, this, &MainWindow::handlePicking);
        connect(m_viewer, &Viewer::leftClickPicking, m_viewer, &Viewer::handlePickingResult);

        // Update entities when the engine starts.
        connect(m_avgFramesCount, static_cast<void (QSpinBox::*)(int)>(&QSpinBox::valueChanged),
            this, &MainWindow::framesCountForStatsChanged);

        // Inform property editors of new selections
        connect(this, &MainWindow::selectedEntity, tab_edition, &TransformEditorWidget::setEditable);
        connect(this, &MainWindow::selectedEntity, m_viewer, &Viewer::setGizmoEditable);
        connect(this, &MainWindow::selectedComponent, m_viewer, &Viewer::setGizmoEditable);

        connect( this, &MainWindow::selectedEntity, this, &MainWindow::displayEntityRenderObjects );
        connect( this, &MainWindow::selectedComponent, this, &MainWindow::displayComponentRenderObjects );

        // Enable changing shaders
        connect( m_renderObjectsListView, &QListWidget::currentRowChanged, this, &MainWindow::renderObjectListItemClicked );
        connect( m_currentShaderBox, static_cast<void (QComboBox::*)(const QString&)>( &QComboBox::currentIndexChanged ),
                 this, &MainWindow::changeRenderObjectShader );

        // RO Stuff
        connect( m_toggleRenderObjectButton, &QPushButton::clicked, this, &MainWindow::toggleVisisbleRO );
        connect( m_removeRenderObjectButton, &QPushButton::clicked, this, &MainWindow::toggleXRayRO );
        connect( m_editRenderObjectButton, &QPushButton::clicked, this, &MainWindow::editRO );

        // Renderer stuff
        connect( m_displayedTextureCombo, static_cast<void (QComboBox::*)(const QString&)>( &QComboBox::currentIndexChanged ),
                 m_viewer, &Viewer::displayTexture );

        connect(m_enablePostProcess, &QCheckBox::stateChanged, m_viewer, &Viewer::enablePostProcess);

        // Connect engine signals to the appropriate callbacks
        std::function<void(void)> f =  std::bind(&MainWindow::onEntitiesUpdated, this);
        mainApp->m_engine->getSignalManager()->m_entityCreatedCallbacks.push_back(f);
        mainApp->m_engine->getSignalManager()->m_entityDestroyedCallbacks.push_back(f);
        mainApp->m_engine->getSignalManager()->m_componentAddedCallbacks.push_back(f);
        mainApp->m_engine->getSignalManager()->m_componentRemovedCallbacks.push_back(f);

        {
            m_viewer->getViewer()->getCurrentCamera()->resetCamera();
            QSignalBlocker blockTextures( m_displayedTextureCombo );

            auto texs = m_viewer->getRenderer()->getAvailableTextures();
            for ( const auto& tex : texs )
            {
                m_displayedTextureCombo->addItem( tex.c_str() );
            }
        }
    }

    void MainWindow::onEntitiesUpdated()
    {
        m_entityTreeModel->entitiesUpdated();
    }

    void MainWindow::loadFile()
    {
        // Filter the files
        aiString extList;
        Assimp::Importer importer;
        importer.GetExtensionList(extList);
        std::string extListStd(extList.C_Str());
        std::replace(extListStd.begin(), extListStd.end(), ';', ' ');
        QString filter = QString::fromStdString(extListStd);

        QString path = QFileDialog::getOpenFileName( this, "Open File", "..", filter);
        if ( path.size() > 0 )
        {
            mainApp->loadFile(path.toLocal8Bit().toStdString());
            emit loadComplete();
        }
    }

    void MainWindow::framesCountForStatsChanged(int count)
    {
        mainApp->m_frameCountBeforeUpdate = count;
    }

    void MainWindow::onUpdateFramestats(const std::vector<Ra::FrameTimerData>& stats)
    {
        QString framesA2B = QString( "Frames #%1 to #%2 stats :" )
                            .arg( stats.front().numFrame ).arg( stats.back().numFrame );
        m_frameA2BLabel->setText( framesA2B );

        long sumEvents = 0;
        long sumRender = 0;
        long sumTasks = 0;
        long sumFrame = 0;
        long sumInterFrame = 0;

        for ( uint i = 0; i < stats.size(); ++i )
        {
            sumEvents += Ra::Core::Timer::getIntervalMicro(stats[i].eventsStart, stats[i].eventsEnd);
            sumRender += Ra::Core::Timer::getIntervalMicro(stats[i].renderData.renderStart, stats[i].renderData.renderEnd );
            sumTasks  += Ra::Core::Timer::getIntervalMicro(stats[i].tasksStart, stats[i].tasksEnd );
            sumFrame  += Ra::Core::Timer::getIntervalMicro(stats[i].frameStart, stats[i].frameEnd );

            if ( i > 0 )
            {
                sumInterFrame += Ra::Core::Timer::getIntervalMicro(stats[i - 1].frameEnd, stats[i].frameEnd );
            }
        }

        const uint N = stats.size();
        const Scalar T( N * 1000000.0 );

        m_eventsTime->setValue(sumEvents / N);
        m_eventsUpdates->setValue(T / Scalar(sumEvents));
        m_renderTime->setValue( sumRender / N );
        m_renderUpdates->setValue( T / Scalar( sumRender ) );
        m_tasksTime->setValue( sumTasks / N );
        m_tasksUpdates->setValue( T / Scalar( sumTasks ) );
        m_frameTime->setValue( sumFrame / N );
        m_frameUpdates->setValue( T / Scalar( sumFrame ) );
        m_avgFramerate->setValue( ( N - 1 ) * Scalar( 1000000.0 / sumInterFrame ) );
    }

    void MainWindow::keyPressEvent( QKeyEvent* event )
    {
        QMainWindow::keyPressEvent( event );
    }

    void MainWindow::keyReleaseEvent(QKeyEvent* event)
    {
        QMainWindow::keyReleaseEvent(event);
    }

    Viewer* MainWindow::getViewer()
    {
        return m_viewer;
    }

    void MainWindow::handlePicking( int drawableIndex )
    {
        if (m_lastSelectedRO >= 0)
        {

            if ( mainApp->m_engine->getRenderObjectManager()->exists( m_lastSelectedRO ))
            {
                const std::shared_ptr<Ra::Engine::RenderObject>& ro =
                    Ra::Engine::RadiumEngine::getInstance()->getRenderObjectManager()->getRenderObject( m_lastSelectedRO );

                Ra::Engine::Component* comp = ro->getComponent();
                Ra::Engine::Entity* ent = comp->getEntity();
                comp->picked( -1 );
                ent->picked(-1);
            }
        }

        if ( drawableIndex >= 0 )
        {
            const std::shared_ptr<Ra::Engine::RenderObject>& ro =
                Ra::Engine::RadiumEngine::getInstance()->getRenderObjectManager()->getRenderObject( drawableIndex );

            // Ignore UI render objects.
            if(ro->getType() == Ra::Engine::RenderObjectType::UI)
            {
                return;
            }

            LOG( logDEBUG ) << "Picked RO: " << ro->idx.getValue();
            LOG( logDEBUG ) << "RO Name  : " << ro->getName();

            Ra::Engine::Component* comp = ro->getComponent();
            const Ra::Engine::Entity* ent = comp->getEntity();
            int compIdx = -1;
            int i = 0;
            for ( const auto& c : ent->getComponents() )
            {
                if ( c.get() == comp )
                {
                    compIdx = i;
                    break;
                }
                ++i;
            }
            CORE_ASSERT( compIdx >= 0, "Component is not in entity" );
            Ra::Core::Index entIdx = ent->idx;
            QModelIndex entityIdx = m_entityTreeModel->index( entIdx, 0 );
            QModelIndex treeIdx = entityIdx;
            if ( comp->picked(drawableIndex)) // select component.
            {
                treeIdx = entityIdx.child(compIdx, 0);
            }
            m_entitiesTreeView->setExpanded( entityIdx, true );
            m_entitiesTreeView->selectionModel()->select( treeIdx, QItemSelectionModel::SelectCurrent );

            auto foundItems = m_renderObjectsListView->findItems( QString( ro->getName().c_str() ), Qt::MatchExactly );
            if  ( foundItems.size() > 0 )
            {
                m_renderObjectsListView->setCurrentItem( foundItems.at( 0 ) );
            }
            else
            {
                LOG( logERROR ) << "Not found";
            }
        }
        else
        {
            m_entitiesTreeView->selectionModel()->clear();
        }
        m_lastSelectedRO = drawableIndex;
    }

    void MainWindow::onSelectionChanged( const QItemSelection& selected, const QItemSelection& deselected )
    {
        if ( selected.size() > 0 )
        {
            QModelIndex selIdx = selected.indexes()[0];

            Ra::Engine::Entity* entity = m_entityTreeModel->getItem(selIdx)->getData(0).entity;
            if (entity)
            {
                // Debug entity and objects are not selectable
                if (entity != Ra::Engine::SystemEntity::getInstance())
                {
                    emit selectedEntity(entity);
                }
            }
            else
            {
                Ra::Engine::Component* comp = m_entityTreeModel->getItem(selIdx)->getData(0).component;
                emit selectedComponent(comp);
            }
        }
        else
        {
            emit selectedEntity( nullptr );
            emit selectedComponent( nullptr );
        }
    }

    void MainWindow::closeEvent( QCloseEvent *event )
    {
        emit closed();
        event->accept();
    }

    void MainWindow::gizmoShowNone()
    {
        m_viewer->getViewer()->getGizmoManager()->changeGizmoType(Ra::Gui::GizmoManager::NONE);
    }

    void MainWindow::gizmoShowTranslate()
    {
        m_viewer->getViewer()->getGizmoManager()->changeGizmoType(Ra::Gui::GizmoManager::TRANSLATION);
    }

    void MainWindow::gizmoShowRotate()
    {
        m_viewer->getViewer()->getGizmoManager()->changeGizmoType(Ra::Gui::GizmoManager::ROTATION);
    }

    void MainWindow::displayEntityRenderObjects(Ra::Engine::Entity* entity)
    {
        m_renderObjectsListView->clear();
        m_currentShaderBox->setCurrentText( "" );

        // NOTE(Charly): When clicking on UI stuff, the returned entity is null
        if ( nullptr == entity )
        {
            m_selectedStuffName->clear();
            return;
        }

        m_renderObjectsListView->clear();

        QString text( "Entity : " );
        text.append( entity->getName().c_str() );
        m_selectedStuffName->setText( text );


        for ( const auto& comp : entity->getComponents())
        {
            displayRenderObjects( comp.get() );
        }
    }

    void MainWindow::displayComponentRenderObjects(Ra::Engine::Component* component)
    {
        // NOTE(Charly): When clicking on UI stuff, or on nothing, the returned component is null
        m_renderObjectsListView->clear();
        m_currentShaderBox->setCurrentText( "" );

        if ( nullptr == component )
        {
            m_selectedStuffName->clear();
            return;
        }

        QString text( "Entity : " );
        text.append( component->getName().c_str() );
        m_selectedStuffName->setText( text );

        displayRenderObjects( component );
    }

    void MainWindow::displayRenderObjects(Ra::Engine::Component* component)
    {
        auto roMgr = Ra::Engine::RadiumEngine::getInstance()->getRenderObjectManager();
        for (Ra::Core::Index idx : component->m_renderObjects)
        {
            QString name = roMgr->getRenderObject(idx)->getName().c_str();

            QListWidgetItem* item = new QListWidgetItem(name, m_renderObjectsListView);
            item->setData(1, QVariant(idx.getValue()));
        }
    }

    void MainWindow::renderObjectListItemClicked(int idx)
    {
        if ( idx < 0 )
        {
            // Got out of scope
            return;
        }

        QListWidgetItem* item = m_renderObjectsListView->item( idx );
        Ra::Core::Index itemIdx( item->data( 1 ).toInt() );

        auto roMgr = Ra::Engine::RadiumEngine::getInstance()->getRenderObjectManager();
        auto ro = roMgr->getRenderObject( itemIdx );

        auto shaderName = ro->getRenderTechnique()->shader->getBasicConfiguration().m_name;

        m_materialEditor->changeRenderObject( ro->idx );

        if ( m_currentShaderBox->findText( shaderName.c_str() ) == -1 )
        {
            m_currentShaderBox->addItem( QString( shaderName.c_str() ) );
            m_currentShaderBox->setCurrentText( shaderName.c_str() );
        }
        else
        {
            m_currentShaderBox->setCurrentText( shaderName.c_str() );
        }
    }

    void MainWindow::changeRenderObjectShader( const QString& shaderName )
    {
        std::string name = shaderName.toStdString();
        if ( name == "" )
        {
            return;
        }

        auto ro = getSelectedRO();
        if ( ro == nullptr )
        {
            return;
        }

        if (ro->getRenderTechnique()->shader->getBasicConfiguration().m_name == name)
        {
            return;
        }

        Ra::Engine::ShaderConfiguration config = Ra::Engine::ShaderConfigurationFactory::getConfiguration(name);
        ro->getRenderTechnique()->changeShader(config);
    }

    void MainWindow::toggleVisisbleRO()
    {
        auto ro = getSelectedRO();

        if ( ro == nullptr )
        {
            return;
        }

        ro->toggleVisible();
    }

    void MainWindow::toggleXRayRO()
    {
        auto ro = getSelectedRO();

        if ( ro == nullptr )
        {
            return;
        }

        ro->toggleXRay();
    }

    void MainWindow::editRO()
    {
        auto ro = getSelectedRO();
        m_materialEditor->changeRenderObject( ro ? ro->idx : Ra::Core::Index::INVALID_IDX() );
        m_materialEditor->show();
    }

    std::shared_ptr<Ra::Engine::RenderObject> MainWindow::getSelectedRO()
    {
        QListWidgetItem* item = m_renderObjectsListView->currentItem();

        if ( nullptr == item )
        {
            return nullptr;
        }

        Ra::Core::Index itemIdx( item->data( 1 ).toInt() );

        auto roMgr = Ra::Engine::RadiumEngine::getInstance()->getRenderObjectManager();
        auto ro = roMgr->getRenderObject( itemIdx );

        return ro;
    }

    void MainWindow::openMaterialEditor()
    {
        m_materialEditor->show();
    }

    void MainWindow::updateUi( Ra::Plugins::RadiumPluginInterface *plugin )
    {
        QString tabName;

        if ( plugin->doAddMenu() )
        {
            QMainWindow::menuBar()->addMenu( plugin->getMenu() );
            // Add menu
        }

        if ( plugin->doAddWidget( tabName ) )
        {
            toolBox->addItem( plugin->getWidget(), tabName );
        }
    }

    void MainWindow::onRendererReady()
    {

    }

    void MainWindow::onFrameComplete()
    {
        tab_edition->updateValues();
        m_viewer->getViewer()->getGizmoManager()->updateValues();
    }
} // namespace QRadium
