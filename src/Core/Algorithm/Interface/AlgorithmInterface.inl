#include <Core/Algorithm/Interface/AlgorithmInterface.hpp>

#include <ctime>
#include <Core/Log/Log.hpp>


#include <algorithm>

namespace Ra {
namespace Core {

//=====================================================================
//=====================================================================

/// CONSTRUCTOR
template < typename INPUT, typename OUTPUT, typename PARAMETER >
AlgorithmInterface< INPUT, OUTPUT, PARAMETER >::AlgorithmInterface( const std::string   name,
                                                                    const  INPUT* const input,
                                                                    OUTPUT* const       output,
                                                                    const PARAMETER&    parameter) :
    m_input( input ),
    m_output( output ),
    m_param( parameter ),
    m_settings(),
    m_statistics(),
    m_step(),
    m_name( name ),
    m_state( AlgorithmState::READY ),
    m_exitStatus( 0 ) {

}

//=====================================================================
//=====================================================================

/// DESTRUCTOR
template < typename INPUT, typename OUTPUT, typename PARAMETER >
AlgorithmInterface< INPUT, OUTPUT, PARAMETER >::~AlgorithmInterface() {
    deleteStep();
}

//=====================================================================
//=====================================================================

/// INPUT
template < typename INPUT, typename OUTPUT, typename PARAMETER >
inline INPUT* AlgorithmInterface< INPUT, OUTPUT, PARAMETER >::getInput() const {
    return m_input;
}

template < typename INPUT, typename OUTPUT, typename PARAMETER >
inline void AlgorithmInterface< INPUT, OUTPUT, PARAMETER >::setInput( INPUT* const input ) {
    m_input = input;
    m_settings.toggleConfigCheck( true );
}

//=====================================================================
//=====================================================================

/// OUTPUT
template < typename INPUT, typename OUTPUT, typename PARAMETER >
inline OUTPUT* AlgorithmInterface< INPUT, OUTPUT, PARAMETER >::getOutput() const {
    return m_output;
}

template < typename INPUT, typename OUTPUT, typename PARAMETER >
inline void AlgorithmInterface< INPUT, OUTPUT, PARAMETER >::setOutput( OUTPUT* const output ) {
    m_output = output;
    m_settings.toggleConfigCheck( true );
}

//=====================================================================
//=====================================================================

/// PARAMETER
template < typename INPUT, typename OUTPUT, typename PARAMETER >
inline PARAMETER AlgorithmInterface< INPUT, OUTPUT, PARAMETER >::getParameter() const {
    return m_param;
}

template < typename INPUT, typename OUTPUT, typename PARAMETER >
inline void AlgorithmInterface< INPUT, OUTPUT, PARAMETER >::setParameter( const PARAMETER& parameter ) {
    m_param = parameter;
    m_settings.toggleConfigCheck( true );
}

//=====================================================================
//=====================================================================

/// SETTING
template < typename INPUT, typename OUTPUT, typename PARAMETER >
inline AlgorithmSettings AlgorithmInterface< INPUT, OUTPUT, PARAMETER >::getSettings() const {
    return m_settings;
}

template < typename INPUT, typename OUTPUT, typename PARAMETER >
inline AlgorithmSettings& AlgorithmInterface< INPUT, OUTPUT, PARAMETER >::getSettings() {
    return m_settings;
}

template < typename INPUT, typename OUTPUT, typename PARAMETER >
inline void AlgorithmInterface< INPUT, OUTPUT, PARAMETER >::setSettings( const AlgorithmSettings& settings ) {
    m_settings = settings;
}

//=====================================================================
//=====================================================================

/// STATISTICS
template < typename INPUT, typename OUTPUT, typename PARAMETER >
inline AlgorithmStatistics AlgorithmInterface< INPUT, OUTPUT, PARAMETER >::getStatistics() const {
    return m_statistics;
}

//=====================================================================
//=====================================================================

/// STEP
template < typename INPUT, typename OUTPUT, typename PARAMETER >
inline uint AlgorithmInterface< INPUT, OUTPUT, PARAMETER >::getStepSize() const {
    return m_step.size();
}

template < typename INPUT, typename OUTPUT, typename PARAMETER >
inline const AlgorithmStep< OUTPUT > AlgorithmInterface< INPUT, OUTPUT, PARAMETER >::getStep( const uint i ) const {
    return m_step.at( i );
}

template < typename INPUT, typename OUTPUT, typename PARAMETER >
inline OUTPUT* AlgorithmInterface< INPUT, OUTPUT, PARAMETER >::addStep( const std::string& name ) {
    OUTPUT* data = new OUTPUT();
    m_step.push_back( AlgorithmStep< OUTPUT >( ( m_name + "_" + name + "_" + std::to_string( m_step.size() ) ), data ) );
    return data;
}

template < typename INPUT, typename OUTPUT, typename PARAMETER >
inline void AlgorithmInterface< INPUT, OUTPUT, PARAMETER >::deleteStep() {
    m_step.clear();
}

//=====================================================================
//=====================================================================

/// NAME
template < typename INPUT, typename OUTPUT, typename PARAMETER >
inline std::string AlgorithmInterface< INPUT, OUTPUT, PARAMETER >::getName() const {
    return m_name;
}

//=====================================================================
//=====================================================================

/// STATE
template < typename INPUT, typename OUTPUT, typename PARAMETER >
inline typename AlgorithmInterface< INPUT, OUTPUT, PARAMETER >::AlgorithmState AlgorithmInterface< INPUT, OUTPUT, PARAMETER >::getState() const {
    return m_state;
}

template < typename INPUT, typename OUTPUT, typename PARAMETER >
inline bool AlgorithmInterface< INPUT, OUTPUT, PARAMETER >::isCompleted() const {
    return ( m_state == AlgorithmState::COMPLETED );
}

template < typename INPUT, typename OUTPUT, typename PARAMETER >
inline bool AlgorithmInterface< INPUT, OUTPUT, PARAMETER >::isFailed() const {
    return ( ( m_state != AlgorithmState::READY     ) &&
             ( m_state != AlgorithmState::RUNNING   ) &&
             ( m_state != AlgorithmState::COMPLETED ) );
}

//=====================================================================
//=====================================================================

/// EXIT STATUS
template < typename INPUT, typename OUTPUT, typename PARAMETER >
inline uint AlgorithmInterface< INPUT, OUTPUT, PARAMETER >::getExitStatus() const {
    return m_exitStatus;
}

//=====================================================================
//=====================================================================

/// RUN
template < typename INPUT, typename OUTPUT, typename PARAMETER >
inline uint AlgorithmInterface< INPUT, OUTPUT, PARAMETER >::run() {
    m_statistics.reset();
    deleteStep();

    m_state = AlgorithmState::RUNNING;
    m_exitStatus = 0;

    if( m_settings.isVerbose() ) {
        LOG( logINFO ) << "======= ALGORITHM INFO =======";
        LOG( logINFO ) << "Starting " << m_name << "...";
    }

    std::clock_t startTime;
    startTime = std::clock();
    while( true ) {
        if( !run_setup         ( m_exitStatus ) ) break;
        if( !run_configCheck   ( m_exitStatus ) ) break;
        if( !run_preprocessing ( m_exitStatus ) ) break;
        if( !run_processing    ( m_exitStatus ) ) break;
        if( !run_postprocessing( m_exitStatus ) ) break;
        break;
    }
    m_statistics.setOverallTiming( ( std::clock() - startTime ) / Scalar( CLOCKS_PER_SEC ) );

    m_statistics.computeOverallPercentage();
    m_statistics.computeStatistics();

    if( m_state == AlgorithmState::RUNNING ) {
        m_state = AlgorithmState::COMPLETED;
        if( m_settings.isVerbose() ) {
            LOG( logINFO ) << m_name << " completed in " << std::to_string( m_statistics.getOverallTiming() ) << "\n";
        }
    } else {
        if( m_settings.isVerbose() ) {
            LOG( logINFO ) << m_name << " aborted.";
            LOG( logINFO ) << "Exit status : " << m_exitStatus << "\n";
        }
    }

    return m_exitStatus;
}
//=====================================================================
//=====================================================================

/// ALGORITHM STAGE
template < typename INPUT, typename OUTPUT, typename PARAMETER >
inline bool AlgorithmInterface< INPUT, OUTPUT, PARAMETER >::run_setup( uint& exitStatus ) {
    if( !m_settings.runSetup() && !m_settings.forceSetup() ) {
        m_statistics.setSetupPercentage( 1.0 );
        m_statistics.computeOverallPercentage();
        return true;
    }

    bool status;

    if( m_settings.isVerbose() ) {
        LOG( logINFO ) << "Starting setup stage...";
    }

    std::clock_t startTime;
    startTime = std::clock();
    if( !( status = setup( exitStatus ) ) ) {
        m_state = AlgorithmState::SETUP_FAILED;
    }
    m_statistics.setSetupTiming( ( std::clock() - startTime ) / Scalar( CLOCKS_PER_SEC ) );

    if( m_settings.isVerbose() ) {
        if( status ) {
            m_settings.toggleSetup( false );
            m_statistics.setSetupPercentage( 1.0 );
            m_statistics.computeOverallPercentage();
            LOG( logINFO ) << "Setup completed in " << std::to_string( m_statistics.getSetupTiming() );
        } else {
            LOG( logINFO ) << "### SETUP STAGE FAILED ###";
        }
    }

    return status;
}

template < typename INPUT, typename OUTPUT, typename PARAMETER >
inline bool AlgorithmInterface< INPUT, OUTPUT, PARAMETER >::run_configCheck( uint& exitStatus ) {
    if( !m_settings.runConfigCheck() && !m_settings.forceConfigCheck() ) {
        m_statistics.setConfigCheckPercentage( 1.0 );
        m_statistics.computeOverallPercentage();
        return true;
    }

    bool status;

    if( m_settings.isVerbose() ) {
        LOG( logINFO ) << "Checking configuration...";
    }

    std::clock_t startTime;
    startTime = std::clock();
    if( !( status = configCheck( exitStatus ) ) ) {
        m_state = AlgorithmState::NOT_CONFIGURED;
    }
    m_statistics.setConfigCheckTiming( ( std::clock() - startTime ) / Scalar( CLOCKS_PER_SEC ) );

    if( m_settings.isVerbose() ) {
        if( status ) {
            m_settings.toggleConfigCheck( false );
            m_statistics.setConfigCheckPercentage( 1.0 );
            m_statistics.computeOverallPercentage();
            LOG( logINFO ) << "Checking completed in " << std::to_string( m_statistics.getConfigCheckTiming() );
        } else {
            LOG( logINFO ) << "### ALGORITHM NOT CONFIGURED ###";
        }
    }

    return status;
}

template < typename INPUT, typename OUTPUT, typename PARAMETER >
inline bool AlgorithmInterface< INPUT, OUTPUT, PARAMETER >::run_preprocessing( uint& exitStatus ) {
    if( !m_settings.runPreProcessing() ) {
        m_statistics.setPreProcessingPercentage( 1.0 );
        m_statistics.computeOverallPercentage();
        return true;
    }

    bool status;

    if( m_settings.isVerbose() ) {
        LOG( logINFO ) << "Starting preprocessing stage...";
    }

    std::clock_t startTime;
    startTime = std::clock();
    if( !( status = preprocessing( exitStatus ) ) ) {
        m_state = AlgorithmState::PREPROCESSING_FAILED;
    }
    m_statistics.setPreProcessingTiming( ( std::clock() - startTime ) / Scalar( CLOCKS_PER_SEC ) );

    if( m_settings.isVerbose() ) {
        if( status ) {
            m_statistics.setPreProcessingPercentage( 1.0 );
            m_statistics.computeOverallPercentage();
            LOG( logINFO ) << "Preprocessing completed in " << std::to_string( m_statistics.getPreProcessingTiming() );
        } else {
            LOG( logINFO ) << "### PREPROCESSING STAGE FAILED ###";
        }
    }

    return status;
}

template < typename INPUT, typename OUTPUT, typename PARAMETER >
inline bool AlgorithmInterface< INPUT, OUTPUT, PARAMETER >::run_processing( uint& exitStatus ) {
    if( !m_settings.runProcessing() ) {
        m_statistics.setProcessingPercentage( 1.0 );
        m_statistics.computeOverallPercentage();
        return true;
    }

    bool status;

    if( m_settings.isVerbose() ) {
        LOG( logINFO ) << "Starting processing stage...";
    }

    std::clock_t startTime;
    startTime = std::clock();
    if( !( status = processing( exitStatus ) ) ) {
        m_state = AlgorithmState::PROCESSING_FAILED;
    }
    m_statistics.setProcessingTiming( ( std::clock() - startTime ) / Scalar( CLOCKS_PER_SEC ) );

    if( m_settings.isVerbose() ) {
        if( status ) {
            m_statistics.setProcessingPercentage( 1.0 );
            m_statistics.computeOverallPercentage();
            LOG( logINFO ) << "Processing completed in " << std::to_string( m_statistics.getProcessingTiming() );
        } else {
            LOG( logINFO ) << "### PROCESSING STAGE FAILED ###";
        }
    }

    return status;
}

template < typename INPUT, typename OUTPUT, typename PARAMETER >
inline bool AlgorithmInterface< INPUT, OUTPUT, PARAMETER >::run_postprocessing( uint& exitStatus ) {
    if( !m_settings.runPostProcessing() ) {
        m_statistics.setPostProcessingPercentage( 1.0 );
        m_statistics.computeOverallPercentage();
        return true;
    }

    bool status;

    if( m_settings.isVerbose() ) {
        LOG( logINFO ) << "Starting postprocessing stage...";
    }

    std::clock_t startTime;
    startTime = std::clock();
    if( !( status = postprocessing( exitStatus ) ) ) {
        m_state = AlgorithmState::POSTPROCESSING_FAILED;
    }
    m_statistics.setPostProcessingTiming( ( std::clock() - startTime ) / Scalar( CLOCKS_PER_SEC ) );

    if( m_settings.isVerbose() ) {
        if( status ) {
            m_statistics.setPostProcessingPercentage( 1.0 );
            m_statistics.computeOverallPercentage();
            LOG( logINFO ) << "Postprocessing completed in " << std::to_string( m_statistics.getPostProcessingTiming() );
        } else {
            LOG( logINFO ) << "### POSTPROCESSING STAGE FAILED ###";
        }
    }

    return status;
}



} // namespace Core
} // namespace Ra

