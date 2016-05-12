#include <QRadium/MainApplication.hpp>

int main( int argc, char** argv )
{
    QRadium::QRadiumApplication::createInstance(argc, argv);
    mainApp->run();
//    QRadium::QRadiumApplication::destroyInstance();
    return 0;
}
