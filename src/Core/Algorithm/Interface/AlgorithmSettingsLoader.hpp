#ifndef RADIUMENGINE_ALGORITHM_SETTINGS_LOADER_DEFINITION_HPP
#define RADIUMENGINE_ALGORITHM_SETTINGS_LOADER_DEFINITION_HPP

#include <string>
#include <fstream>
#include <sstream>
#include <iostream>
#include <Core/Algorithm/Interface/AlgorithmSettings.hpp>

namespace Ra {
namespace Core {

void loadAlgorithmSettings( const std::string& filename,
                            AlgorithmSettings& settings ) {
    const std::string header = "SETTINGS";
    std::string line;

    std::ifstream myfile( filename );
    CORE_ASSERT(myfile.is_open(), "FILE PROBLEM" );

    /// Header
    if( !std::getline( myfile, line ) ) {
        return;
    }

    if( line != header ) {
        return;
    }

    // Reading the data
    const uint size = 12;
    uint data[size];
    for( uint i = 0; i < size; ++i ) {
        std::getline( myfile, line );
        std::string ID;
        std::stringstream stream( line );
        stream >> ID >> data[i];
    }


    /// Priority
    settings.setPriority( data[0] );

    /// Flow
    // Setup
    settings.toggleSetup( ( data[1] == 0 ) ? false : true );

    // Config check
    settings.toggleConfigCheck( ( data[2] == 0 ) ? false : true );

    // PreProcessing
    settings.togglePreProcessing( ( data[3] == 0 ) ? false : true );

    // Processing
    settings.toggleProcessing( ( data[4] == 0 ) ? false : true );

    // PostProcessing
    settings.togglePostProcessing( ( data[5] == 0 ) ? false : true );

    // Force setup
    settings.toggleForceSetup( ( data[6] == 0 ) ? false : true );

    // Force config check
    settings.toggleForceConfigCheck( ( data[7] == 0 ) ? false : true );

    /// Step
    settings.toggleStoreMidResults( ( data[8] == 0 ) ? false : true );

    /// Multi-thread
    settings.toggleMultiThread( ( data[9] == 0 ) ? false : true );

    /// Verbose
    settings.toggleVerbose( ( data[10] == 0 ) ? false : true );

    /// Assert
    settings.toggleFailOnAssert( ( data[11] == 0 ) ? false : true );


    // Close file
    myfile.close();
}

} // namespace Core
} // namespace Ra

#endif // RADIUMENGINE_ALGORITHM_SETTINGS_LOADER_DEFINITION_HPP
