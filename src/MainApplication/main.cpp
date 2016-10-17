#include <QCommandLineParser>

//#include <MainApplication/MainApplication.hpp>
#include <MainApplication/Gui/MainWindow.hpp>

#include <cstdio>

int main( int argc, char** argv )
{
    //Ra::MainApplication app( argc, argv );
    //return app.exec();

    QApplication app(argc, argv);

    // Set application and organization names in order to ensure uniform
    // QSettings configurations.
    // \see http://doc.qt.io/qt-5/qsettings.html#QSettings-4
    QCoreApplication::setOrganizationName("AGGA-IRIT");
    QCoreApplication::setApplicationName("Radium Engine");

    QSurfaceFormat format;
    format.setVersion(4, 4);
    format.setProfile(QSurfaceFormat::CoreProfile);
    format.setDepthBufferSize(24);
    format.setStencilBufferSize(8);
    format.setSamples(16);
    format.setSwapBehavior(QSurfaceFormat::DoubleBuffer);
    // FIXME(charly): Check check swapInterval value
    format.setSwapInterval(0);
    QSurfaceFormat::setDefaultFormat(format);

    QCommandLineParser parser;
    parser.setApplicationDescription("Radium Engine RPZ, TMTC");
    parser.addHelpOption();
    parser.addVersionOption();

    // For any reason, the third parameter must be set if you want to be able to read anything from it (and it cannot be "")
    QCommandLineOption fpsOpt(QStringList{ "r", "framerate", "fps" }, "Control the application framerate, 0 to disable it (and run as fast as possible)", "60");
    QCommandLineOption pluginOpt(QStringList{ "p", "plugins", "pluginsPath" }, "Set the path to the plugin dlls", "../Plugins/bin");
    QCommandLineOption fileOpt(QStringList{ "f", "file", "scene" }, "Open a scene file at startup", "foo.bar");
    QCommandLineOption numFramesOpt(QStringList{ "n", "numframes" }, "Run for a fixed number of frames", "0");
    // NOTE(Charly): Add other options here

    parser.addOptions({ fpsOpt, pluginOpt, fileOpt, numFramesOpt });
    parser.process(app);

    uint fps = 60;
    std::string pluginsPath = "../Plugins/bin";
    uint numFrames = 0;
    std::string fileToLoad = "";

    if (parser.isSet(fpsOpt))       fps = parser.value(fpsOpt).toUInt();
    if (parser.isSet(pluginOpt))    pluginsPath = parser.value(pluginOpt).toStdString();
    if (parser.isSet(numFramesOpt)) numFrames = parser.value(numFramesOpt).toUInt();
    if (parser.isSet(fileOpt))      fileToLoad = parser.value(fileOpt).toStdString();

    Ra::Gui::MainWindow window(fps, pluginsPath, numFrames, fileToLoad);

    window.show();

    return app.exec();

    /*
    const uint& fpsMax = app.m_targetFPS;
    const Scalar deltaTime( fpsMax == 0 ? 0.f : 1.f / Scalar( fpsMax ) );

    Ra::Core::Timer::TimePoint t0, t1;

    while ( app.isRunning() )
    {
        t0 = Ra::Core::Timer::Clock::now();
        // Main loop
        app.radiumFrame();

        // Wait for VSync
        Scalar remaining = deltaTime;
        while (remaining > 0.0)
        {
            t1 = Ra::Core::Timer::Clock::now();
            remaining -= Ra::Core::Timer::getIntervalSeconds(t0, t1);
            t0 = t1;
        }
    }

    app.exit();
    */
}
