#include <MainApplication/Gui/MainWindow.hpp>

#include <QSettings>
#include <QFileDialog>
#include <QToolButton>
#include <QTimer>
#include <QDir>
#include <QPluginLoader>

#include <assimp/Importer.hpp>

#include <Engine/RadiumEngine.hpp>
#include <Engine/Component/Component.hpp>
#include <Engine/Entity/Entity.hpp>
#include <Engine/Renderer/Renderer.hpp>
#include <Engine/Renderer/RenderTechnique/RenderTechnique.hpp>
#include <Engine/Renderer/RenderTechnique/ShaderProgram.hpp>
#include <Engine/Renderer/RenderTechnique/ShaderConfigFactory.hpp>
#include <Engine/Managers/SystemDisplay/SystemDisplay.hpp>

#include <GuiBase/TreeModel/EntityTreeModel.hpp>

#include <MainApplication/Gui/MaterialEditor.hpp>
#include <MainApplication/Viewer/CameraInterface.hpp>

#include <MainApplication/Version.hpp>

#include <MainApplication/PluginBase/RadiumPluginInterface.hpp>

using Ra::Engine::ItemEntry;

namespace Ra
{
    namespace Gui
    {
        MainWindow::MainWindow(uint fps, std::string pluginsPath, uint numFrames, std::string fileToLoad, QWidget* parent)
            : QMainWindow(parent)
            , m_targetFPS(fps)
            , m_pluginsPath(pluginsPath)
            , m_numFrames(numFrames)
            , m_frameCounter(0)
            , m_frameCountBeforeUpdate(0)
        {
            // Note : at this point most of the components (including the Engine) are
            // not initialized. Listen to the "started" signal.

            setupUi(this);

            setWindowIcon(QPixmap(":/Assets/Images/RadiumIcon.png"));
            setWindowTitle(QString("Radium Engine"));

            // Boilerplate print.
            LOG(logINFO) << "*** Radium Engine Main App  ***";
            std::stringstream config;
#if defined (CORE_DEBUG)
            config << "(Debug Build) -- ";
#else
            config << "(Release Build) -- ";
#endif

#if defined (ARCH_X86)
            config << " 32 bits x86";
#elif defined (ARCH_X64)
            config << " 64 bits x64";
#endif
            LOG(logINFO) << config.str();

            config.str(std::string());
            config << "Floating point format : ";
#if defined(CORE_USE_DOUBLE)
            config << "double precision";
#else
            config << "single precision";
#endif

            LOG(logINFO) << config.str();

            config.str(std::string());
            config<<"build: "<<Version::compiler<<" - "<<Version::compileDate<<" "<<Version::compileTime;

            LOG(logINFO) << config.str();
            LOG(logINFO) << "Qt Version: " << qVersion();

            m_engine.reset(Engine::RadiumEngine::createInstance());
            m_engine->initialize();

            QStringList headers;
            headers << tr("Entities -> Components");
            m_itemModel = new GuiBase::ItemModel(m_engine.get(), this);
            m_entitiesTreeView->setModel(m_itemModel);
            m_materialEditor = new MaterialEditor();
            m_selectionManager = new GuiBase::SelectionManager(m_itemModel, this);
            m_entitiesTreeView->setSelectionModel(m_selectionManager);

            addBasicShaders();

            if (!loadPlugins())
            {
                LOG(logERROR) << "An error occured while trying to load plugins.";
            }

            m_taskQueue.reset(new Core::TaskQueue(std::thread::hardware_concurrency() - 1));

            m_timer = new QTimer(this);
            m_timer->setTimerType(Qt::PreciseTimer);

            createConnections();
            setupScene();

            if (!fileToLoad.empty())
            {
                loadFile(fileToLoad);
            }

            m_frameCountBeforeUpdate = m_avgFramesCount->value();

            uint msec = 0;
            if (m_targetFPS > 0)
            {
                double sec = 1.0 / (double)m_targetFPS;
                msec = (uint)(1000 * sec);
            }
            m_timer->start(msec);

            //mainApp->framesCountForStatsChanged((uint)m_avgFramesCount->value());
        }

        MainWindow::~MainWindow()
        {
            // Child QObjects will automatically be deleted
            LOG(logINFO) << "About to quit... Cleaning Radium Engine memory.";
            cleanup();
            m_engine->cleanup();
        }

        void MainWindow::cleanup()
        {
            gizmoShowNone();
        }

        void MainWindow::createConnections()
        {
            connect(actionOpenMesh, &QAction::triggered, this, &MainWindow::openMesh);
            connect(actionReload_Shaders, &QAction::triggered, m_viewer, &Viewer::reloadShaders);
            connect(actionOpen_Material_Editor, &QAction::triggered, this, &MainWindow::openMaterialEditor);

            // Toolbox setup
            connect(actionToggle_Local_Global, &QAction::toggled, m_viewer->getGizmoManager(), &GizmoManager::setLocal);
            connect(actionGizmoOff, &QAction::triggered, this, &MainWindow::gizmoShowNone);
            connect(actionGizmoTranslate, &QAction::triggered, this, &MainWindow::gizmoShowTranslate);
            connect(actionGizmoRotate, &QAction::triggered, this, &MainWindow::gizmoShowRotate);

            // Connect picking results (TODO Val : use events to dispatch picking directly)
            connect(m_viewer, &Viewer::rightClickPicking, this, &MainWindow::handlePicking);
            connect(m_viewer, &Viewer::leftClickPicking, m_viewer->getGizmoManager(), &GizmoManager::handlePickingResult);

            connect(m_avgFramesCount, static_cast<void (QSpinBox::*)(int)>(&QSpinBox::valueChanged),
                    this, &MainWindow::framesCountForStatsChanged);

            // Inform property editors of new selections
            connect(m_selectionManager, &GuiBase::SelectionManager::selectionChanged, this, &MainWindow::onSelectionChanged);
            //connect(this, &MainWindow::selectedItem, tab_edition, &TransformEditorWidget::setEditable);
            connect(this, &MainWindow::selectedItem, m_viewer->getGizmoManager(), &GizmoManager::setEditable);
            connect(this, &MainWindow::selectedItem, m_viewer->getGizmoManager(), &GizmoManager::setEditable);

            // Enable changing shaders
            connect(m_currentShaderBox, static_cast<void (QComboBox::*)(const QString&)>(&QComboBox::currentIndexChanged),
                    this, &MainWindow::changeRenderObjectShader);

            // RO Stuff
            connect(m_toggleRenderObjectButton, &QPushButton::clicked, this, &MainWindow::toggleVisisbleRO);
            connect(m_editRenderObjectButton, &QPushButton::clicked, this, &MainWindow::editRO);

            // Renderer stuff
            connect(m_viewer, &Viewer::rendererReady, this, &MainWindow::onRendererReady);

            connect(m_displayedTextureCombo,
                    static_cast<void (QComboBox::*)(const QString&)>(&QComboBox::currentIndexChanged),
                    m_viewer, &Viewer::displayTexture);

            connect(m_enablePostProcess, &QCheckBox::stateChanged, m_viewer, &Viewer::enablePostProcess);
            connect(m_enableDebugDraw, &QCheckBox::stateChanged, m_viewer, &Viewer::enableDebugDraw);
            connect(m_realFrameRate, &QCheckBox::stateChanged, this, &MainWindow::setRealFrameRate);

            connect(m_timer, &QTimer::timeout, this, &MainWindow::radiumFrame);

            // Connect engine signals to the appropriate callbacks
            std::function<void(const Engine::ItemEntry&)> add = std::bind(&MainWindow::onItemAdded, this, std::placeholders::_1);
            std::function<void(const Engine::ItemEntry&)> del = std::bind(&MainWindow::onItemRemoved, this, std::placeholders::_1);
            m_engine->getSignalManager()->m_entityCreatedCallbacks.push_back(add);
            m_engine->getSignalManager()->m_entityDestroyedCallbacks.push_back(del);

            m_engine->getSignalManager()->m_componentAddedCallbacks.push_back(add);
            m_engine->getSignalManager()->m_componentRemovedCallbacks.push_back(del);

            m_engine->getSignalManager()->m_roAddedCallbacks.push_back(add);
            m_engine->getSignalManager()->m_roRemovedCallbacks.push_back(del);
        }

        void MainWindow::radiumFrame()
        {
            using Ra::Core::Timer::Clock;
            using Ra::Core::Timer::getIntervalSeconds;

            FrameTimerData timerData;
            timerData.frameStart = Clock::now();

            // 0. Compute time since last frame
            const Scalar dt = m_realFrameRate ?
                getIntervalSeconds(m_lastFrameStart, timerData.frameStart) :
                1. / Scalar(m_targetFPS);
            m_lastFrameStart = timerData.frameStart;

            // ---------------------------
            // 1. Picking
            m_viewer->processPicking();

            // ---------------------------
            // 2. Tasks
            timerData.tasksStart = Clock::now();

            // Fill the task queue
            m_engine->getTasks(m_taskQueue.get(), dt);

            // Rune one frame of tasks
            m_taskQueue->startTasks();
            m_taskQueue->waitForTasks();

            timerData.taskData = m_taskQueue->getTimerData();
            m_taskQueue->flushTaskQueue();

            timerData.tasksEnd = Clock::now();

            // ---------------------------
            // 3. Rendering
            timerData.renderData = m_viewer->getRenderer()->getTimerData();

            // Add a new Qt paint event
            m_viewer->updateGL();

            m_engine->endFrameSync();

            timerData.frameEnd = Clock::now();
            timerData.numFrame = m_frameCounter;

            m_timerData.push_back(timerData);
            ++m_frameCounter;

            if (m_numFrames > 0 && m_frameCounter > m_numFrames)
            {
                qApp->quit();
            }

            if (m_frameCounter % m_frameCountBeforeUpdate == 0)
            {
                updateFrameStats(m_timerData);
                m_timerData.clear();
            }

            onFrameComplete();
        }

        void MainWindow::openMesh()
        {
            // Filter the files
            aiString extList;
            Assimp::Importer importer;
            importer.GetExtensionList(extList);
            std::string extListStd(extList.C_Str());
            std::replace(extListStd.begin(), extListStd.end(), ';', ' ');
            QString filter = QString::fromStdString(extListStd);

            QSettings settings;
            QString path = settings.value("files/load", QDir::homePath()).toString();
            path = QFileDialog::getOpenFileName(this, "Open File", path, filter);
            if (path.size() > 0)
            {
                settings.setValue("files/load", path);
                loadFile(path.toStdString());
            }
        }

        void MainWindow::loadFile(std::string file)
        {
            LOG(logINFO) << "Loading file " << file << "...";
            bool res = m_engine->loadFile(file);
            CORE_UNUSED(res);

            m_viewer->handleFileLoading(file);

            Core::Aabb aabb;
            std::vector<std::shared_ptr<Ra::Engine::RenderObject>> ros;
            m_engine->getRenderObjectManager()->getRenderObjects(ros);

            for (const auto& ro : ros)
            {
                auto mesh = ro->getMesh();
                auto pos = mesh->getGeometry().m_vertices;

                for (auto& p : pos)
                {
                    p = ro->getLocalTransform() * p;
                }

                Ra::Core::Vector3 bmin = pos.getMap().rowwise().minCoeff().head<3>();
                Ra::Core::Vector3 bmax = pos.getMap().rowwise().maxCoeff().head<3>();

                aabb.extend(bmin);
                aabb.extend(bmax);
            }

            m_viewer->fitCameraToScene(aabb);
        }

        void MainWindow::updateFrameStats(const std::vector<FrameTimerData>& stats)
        {
            using Core::Timer::getIntervalMicro;
            using Core::Timer::getIntervalSeconds;

            QString framesA2B = QString("Frames #%1 to #%2 stats :")
                .arg(stats.front().numFrame).arg(stats.back().numFrame);
            m_frameA2BLabel->setText(framesA2B);

            long sumEvents = 0;
            long sumRender = 0;
            long sumTasks = 0;
            long sumFrame = 0;
            long sumInterFrame = 0;

            double renderUpdate = 0;
            double renderFeedQueue = 0;
            double renderMainRender = 0;
            double renderPostProcess = 0;
            double renderEnd = 0;

            for (uint i = 0; i < stats.size(); ++i)
            {
                sumEvents       += getIntervalMicro(stats[i].eventsStart, stats[i].eventsEnd);
                sumRender       += getIntervalMicro(stats[i].renderData.renderStart, stats[i].renderData.renderEnd);
                sumTasks        += getIntervalMicro(stats[i].tasksStart, stats[i].tasksEnd);
                sumFrame        += getIntervalMicro(stats[i].frameStart, stats[i].frameEnd);

                renderUpdate        += getIntervalMicro(stats[i].renderData.renderStart, stats[i].renderData.updateEnd);
                renderFeedQueue     += getIntervalMicro(stats[i].renderData.updateEnd, stats[i].renderData.feedRenderQueuesEnd);
                renderMainRender    += getIntervalMicro(stats[i].renderData.feedRenderQueuesEnd, stats[i].renderData.mainRenderEnd);
                renderPostProcess   += getIntervalMicro(stats[i].renderData.mainRenderEnd, stats[i].renderData.postProcessEnd);
                renderEnd           += getIntervalMicro(stats[i].renderData.postProcessEnd, stats[i].renderData.renderEnd);

                if (i > 0)
                {
                    sumInterFrame += Core::Timer::getIntervalMicro(stats[i - 1].frameEnd, stats[i].frameEnd);
                }
            }

            const uint N(stats.size());
            const Scalar T(N * 1000000.f);

            m_eventsTime->setValue(sumEvents / N);
            m_eventsUpdates->setValue(T / Scalar(sumEvents));
            m_renderTime->setValue(sumRender / N);
            m_renderUpdates->setValue(T / Scalar(sumRender));
            m_tasksTime->setValue(sumTasks / N);
            m_tasksUpdates->setValue(T / Scalar(sumTasks));
            m_frameTime->setValue(sumFrame / N);
            m_frameUpdates->setValue(T / Scalar(sumFrame));
            m_avgFramerate->setValue((N - 1) * Scalar(1000000.0 / sumInterFrame));

            LOG(logINFO) << "Rendering stats : ";
            LOG(logINFO) << "\tUpdate             : " << (double)renderUpdate / N;
            LOG(logINFO) << "\tFeed render queues : " << (double)renderFeedQueue / N;
            LOG(logINFO) << "\tMain rendering     : " << (double)renderMainRender / N;
            LOG(logINFO) << "\tPost-process       : " << (double)renderPostProcess / N;
            LOG(logINFO) << "\tFinal rendering    : " << (double)renderEnd / N;
        }

        Viewer* MainWindow::getViewer()
        {
            return m_viewer;
        }

        GuiBase::SelectionManager* MainWindow::getSelectionManager()
        {
            return m_selectionManager;
        }

        void MainWindow::handlePicking(int pickingResult)
        {
            Ra::Core::Index roIndex(pickingResult);
            Ra::Engine::RadiumEngine* engine = Ra::Engine::RadiumEngine::getInstance();
            if (roIndex.isValid())
            {
                Ra::Engine::Component* comp = engine->getRenderObjectManager()->getRenderObject(roIndex)->getComponent();
                Ra::Engine::Entity* ent = comp->getEntity();

                // For now we don't enable group selection.
                m_selectionManager->setCurrentEntry(ItemEntry(ent, comp, roIndex), QItemSelectionModel::ClearAndSelect | QItemSelectionModel::Current);
            }
            else
            {
                m_selectionManager->clear();
            }
        }

        void MainWindow::onSelectionChanged(const QItemSelection& selected, const QItemSelection& deselected)
        {
            if (m_selectionManager->hasSelection())
            {
                const ItemEntry& ent = m_selectionManager->currentItem();
                emit selectedItem(ent);
                m_selectedItemName->setText(QString::fromStdString(getEntryName(m_engine.get(), ent)));
                m_editRenderObjectButton->setEnabled(false);

                if (ent.isRoNode())
                {
                    m_editRenderObjectButton->setEnabled(true);

                    m_materialEditor->changeRenderObject(ent.m_roIndex);
                    const std::string& shaderName = m_engine->getRenderObjectManager()->getRenderObject(
                        ent.m_roIndex)
                        ->getRenderTechnique()->shader->getBasicConfiguration().m_name;

                    if (m_currentShaderBox->findText(shaderName.c_str()) == -1)
                    {
                        m_currentShaderBox->addItem(QString(shaderName.c_str()));
                        m_currentShaderBox->setCurrentText(shaderName.c_str());
                    }
                    else
                    {
                        m_currentShaderBox->setCurrentText(shaderName.c_str());
                    }
                }
            }
            else
            {
                emit selectedItem(ItemEntry());
                m_selectedItemName->setText("");
                m_editRenderObjectButton->setEnabled(false);
                m_materialEditor->hide();
            }
        }

        void MainWindow::closeEvent(QCloseEvent* event)
        {
            emit closed();
            event->accept();
        }

        void MainWindow::gizmoShowNone()
        {
            m_viewer->getGizmoManager()->changeGizmoType(GizmoManager::NONE);
        }

        void MainWindow::gizmoShowTranslate()
        {
            m_viewer->getGizmoManager()->changeGizmoType(GizmoManager::TRANSLATION);
        }

        void MainWindow::gizmoShowRotate()
        {
            m_viewer->getGizmoManager()->changeGizmoType(GizmoManager::ROTATION);
        }

        void MainWindow::changeRenderObjectShader(const QString& shaderName)
        {
            std::string name = shaderName.toStdString();
            if (name == "")
            {
                return;
            }

            const ItemEntry& item = m_selectionManager->currentItem();
            if (!item.isRoNode())
            {
                return;
            }

            const auto& ro = m_engine->getRenderObjectManager()->getRenderObject(item.m_roIndex);
            if (ro->getRenderTechnique()->shader->getBasicConfiguration().m_name == name)
            {
                return;
            }
            Engine::ShaderConfiguration config = Ra::Engine::ShaderConfigurationFactory::getConfiguration(name);
            ro->getRenderTechnique()->changeShader(config);
        }

        void MainWindow::toggleVisisbleRO()
        {
            const ItemEntry& item = m_selectionManager->currentItem();
            // If at least one RO is visible, turn them off.
            bool hasVisible = false;
            for (auto roIdx : getItemROs(m_engine.get(), item))
            {
                if (m_engine->getRenderObjectManager()->getRenderObject(roIdx)->isVisible())
                {
                    hasVisible = true;
                    break;
                }
            }
            for (auto roIdx : getItemROs(m_engine.get(), item))
            {
                m_engine->getRenderObjectManager()->getRenderObject(roIdx)->setVisible(!hasVisible);
            }
        }

        void MainWindow::editRO()
        {
            ItemEntry item = m_selectionManager->currentItem();
            if (item.isRoNode())
            {
                m_materialEditor->changeRenderObject(item.m_roIndex);
                m_materialEditor->show();
            }
        }

        void MainWindow::openMaterialEditor()
        {
            m_materialEditor->show();
        }

        void MainWindow::updateUi(Plugins::RadiumPluginInterface* plugin)
        {
            QString tabName;

            if (plugin->doAddMenu())
            {
                QMainWindow::menuBar()->addMenu(plugin->getMenu());
                // Add menu
            }

            if (plugin->doAddWidget(tabName))
            {
                toolBox->addItem(plugin->getWidget(), tabName);
            }
        }

        void MainWindow::onRendererReady()
        {
            m_viewer->getCameraInterface()->resetCamera();

            QSignalBlocker blockTextures(m_displayedTextureCombo);

            auto texs = m_viewer->getRenderer()->getAvailableTextures();
            for (const auto& tex : texs)
            {
                m_displayedTextureCombo->addItem(tex.c_str());
            }
        }

        void MainWindow::onFrameComplete()
        {
            tab_edition->updateValues();
            m_viewer->getGizmoManager()->updateValues();
        }

        void MainWindow::onItemAdded(const Engine::ItemEntry& ent)
        {
            m_itemModel->addItem(ent);
        }

        void MainWindow::onItemRemoved(const Engine::ItemEntry& ent)
        {
            m_itemModel->removeItem(ent);
        }

        void MainWindow::setupScene()
        {
            using namespace Engine::DrawPrimitives;

            Engine::SystemEntity::uiCmp()->addRenderObject(
                Primitive(Engine::SystemEntity::uiCmp(), Grid(
                Core::Vector3::Zero(), Core::Vector3::UnitX(),
                Core::Vector3::UnitZ(), Core::Colors::Grey(0.6f))));

            Engine::SystemEntity::uiCmp()->addRenderObject(
                Primitive(Engine::SystemEntity::uiCmp(), Frame(Ra::Core::Transform::Identity(), 0.05f)));
        }

        void MainWindow::addBasicShaders()
        {
            using namespace Ra::Engine;

            ShaderConfiguration bpConfig("BlinnPhong");
            bpConfig.addShader(ShaderType_VERTEX, "../Shaders/BlinnPhong.vert.glsl");
            bpConfig.addShader(ShaderType_FRAGMENT, "../Shaders/BlinnPhong.frag.glsl");
            ShaderConfigurationFactory::addConfiguration(bpConfig);

            ShaderConfiguration pConfig("Plain");
            pConfig.addShader(ShaderType_VERTEX, "../Shaders/Plain.vert.glsl");
            pConfig.addShader(ShaderType_FRAGMENT, "../Shaders/Plain.frag.glsl");
            ShaderConfigurationFactory::addConfiguration(pConfig);

            ShaderConfiguration lgConfig("LinesGeom");
            lgConfig.addShader(ShaderType_VERTEX, "../Shaders/Lines.vert.glsl");
            lgConfig.addShader(ShaderType_FRAGMENT, "../Shaders/Lines.frag.glsl");
            lgConfig.addShader(ShaderType_GEOMETRY, "../Shaders/Lines.geom.glsl");
            ShaderConfigurationFactory::addConfiguration(lgConfig);

            ShaderConfiguration lConfig("Lines");
            lConfig.addShader(ShaderType_VERTEX, "../Shaders/Lines.vert.glsl");
            lConfig.addShader(ShaderType_FRAGMENT, "../Shaders/Lines.frag.glsl");
            ShaderConfigurationFactory::addConfiguration(lConfig);
        }

        bool MainWindow::loadPlugins()
        {
            LOG(logINFO)<<" *** Loading Plugins ***";
            QDir pluginsDir(qApp->applicationDirPath());
            pluginsDir.cd(m_pluginsPath.c_str());

            bool result = true;
            uint pluginCpt = 0;

            for (const auto& filename : pluginsDir.entryList(QDir::Files))
                //        foreach (QString filename, pluginsDir.entryList( QDir::Files ) )
            {
                PluginContext context;
                context.m_engine = m_engine.get();
                context.m_selectionManager = getSelectionManager();

                std::string ext = Core::StringUtils::getFileExt(filename.toStdString());
#if defined( OS_WINDOWS )
                std::string sysDllExt = "dll";
#elif defined( OS_LINUX )
                std::string sysDllExt = "so";
#elif defined( OS_MACOS )
                std::string sysDllExt = "dylib";
#else
                static_assert(false, "System configuration not handled");
#endif
                if (ext == sysDllExt)
                {
                    QPluginLoader pluginLoader(pluginsDir.absoluteFilePath(filename));
                    // Force symbol resolution at load time.
                    pluginLoader.setLoadHints(QLibrary::ResolveAllSymbolsHint);

                    LOG(logINFO) << "Found plugin " << filename.toStdString();

                    QObject* plugin = pluginLoader.instance();
                    Plugins::RadiumPluginInterface* loadedPlugin;

                    if (plugin)
                    {
                        loadedPlugin = qobject_cast<Plugins::RadiumPluginInterface*>(plugin);
                        if (loadedPlugin)
                        {
                            ++pluginCpt;
                            loadedPlugin->registerPlugin(context);
                            updateUi(loadedPlugin);
                        }
                        else
                        {
                            LOG(logERROR) << "Something went wrong while trying to cast plugin"
                                          << filename.toStdString();
                            result = false;
                        }
                    }
                    else
                    {
                        LOG(logERROR) << "Something went wrong while trying to load plugin "
                                      << filename.toStdString() << " : "
                                      << pluginLoader.errorString().toStdString();
                        result = false;
                    }
                }
            }

            if (pluginCpt == 0)
            {
                LOG(logINFO) << "No plugin found or loaded.";
            }
            else
            {
                LOG(logINFO) << "Loaded " << pluginCpt << " plugins.";
            }

            return result;
        }
    } // namespace Gui
} // namespace Ra
