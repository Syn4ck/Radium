#include <Core/Algorithm/Algorithm.hpp>

#include <ctime>
#include <Core/Log/Log.hpp>

namespace Ra {
namespace Core {

//=====================================================================
//=====================================================================

/// CONSTRUCTOR
template < typename PARAMETERS >
Algorithm< PARAMETERS >::Algorithm( const PARAMETERS&  param,
                                    const std::string& name,
                                    const bool         verbosity ) :
    m_param( param ),
    m_name( name ),
    m_verbosity( verbosity ),
    m_state( AlgorithmState::READY ),
    m_exitStatus( 0 ) {
    for( uint i = 0; i < TOTAL_STAGE; ++i ) {
        m_time[i] = 0.0;
    }
}

//=====================================================================
//=====================================================================

/// DESTRUCTOR
template < typename PARAMETERS >
Algorithm< PARAMETERS >::~Algorithm() { }

//=====================================================================
//=====================================================================

/// PARAMETER
template < typename PARAMETERS >
inline PARAMETERS Algorithm< PARAMETERS >::getParameters() const {
    return m_param;
}



template < typename PARAMETERS >
virtual void Algorithm< PARAMETERS >::setParameters( const PARAMETERS& param ) {
    m_param = param;
}

//=====================================================================
//=====================================================================

/// NAME
template < typename PARAMETERS >
inline std::string Algorithm< PARAMETERS >::getName() const {
    return m_name;
}

//=====================================================================
//=====================================================================

/// STATE
template < typename PARAMETERS >
inline Algorithm< PARAMETERS >::AlgorithmState Algorithm< PARAMETERS >::getState() const {
    return m_state;
}

//=====================================================================
//=====================================================================

/// EXIT STATUS
template < typename PARAMETERS >
inline uint Algorithm< PARAMETERS >::getExitStatus() const {
    return m_exitStatus;
}

//=====================================================================
//=====================================================================

/// TIMING
template < typename PARAMETERS >
inline Scalar Algorithm< PARAMETERS >::getTiming( const AlgorithmStageTiming& stage ) const {
    if( stage >= TOTAL_STAGE ) {
        return m_time[OVERALL];
    }
    return m_time[stage];
}

//=====================================================================
//=====================================================================

/// VERBOSITY
template < typename PARAMETERS >
inline bool Algorithm< PARAMETERS >::isVerbose() const {
    return m_verbosity;
}



template < typename PARAMETERS >
inline void Algorithm< PARAMETERS >::setVerbosity( const bool verbosity ) {
    m_verbosity = verbosity;
}

//=====================================================================
//=====================================================================

/// RUN
template < typename PARAMETERS >
inline uint Algorithm< PARAMETERS >::run() {
    m_state = AlgorithmState::RUNNING;
    m_exitStatus = 0;

    if( isVerbose() ) {
        LOG( logINFO ) << "======= ALGORITHM INFO =======";
        LOG( logINFO ) << "Starting " << m_name << "...";
    }

    std::clock_t startTime;
    startTime = std::clock();
    while( true ) {
        if( !run_configCheck   ( m_exitStatus ) ) break;
        if( !run_preprocessing ( m_exitStatus ) ) break;
        if( !run_processing    ( m_exitStatus ) ) break;
        if( !run_postprocessing( m_exitStatus ) ) break;
        break;
    }
    m_time[OVERALL] = ( std::clock() - startTime ) / Scalar( CLOCKS_PER_SEC );

    if( m_state == AlgorithmState::RUNNING ) {
        m_state = AlgorithmState::COMPLETED;
        if( isVerbose() ) {
            LOG( logINFO ) << m_name << " completed in " << std::to_string( m_time[OVERALL] ) << "\n";
        }
    } else {
        if( isVerbose() ) {
            LOG( logINFO ) << m_name << " aborted.";
            LOG( logINFO ) << "Exit status : " << m_exitStatus << "\n";
        }
    }

    return m_exitStatus;
}

//=====================================================================
//=====================================================================

/// ALGORITHM STAGE
template < typename PARAMETERS >
inline bool Algorithm< PARAMETERS >::run_configCheck( uint& exitStatus ) const {
    bool status;

    if( isVerbose() ) {
        LOG( logINFO ) << "Checking configuration...";
    }

    if( !( status = isConfigured( exitStatus ) ) ) {
        m_state = AlgorithmState::NOT_CONFIGURED;
    }

    if( isVerbose() ) {
        if( status ) {
            LOG( logINFO ) << "Checking completed.";
        } else {
            LOG( logINFO ) << "### ALGORITHM NOT CONFIGURED ###";
        }
    }

    return status;
}



template < typename PARAMETERS >
inline bool Algorithm< PARAMETERS >::run_preprocessing( uint& exitStatus ) {
    bool status;

    if( isVerbose() ) {
        LOG( logINFO ) << "Starting preprocessing stage...";
    }

    std::clock_t startTime;
    startTime = std::clock();
    if( !( status = preprocessing( exitStatus ) ) ) {
        m_state = AlgorithmState::PREPROCESSING_FAILED;
    }
    m_time[PREPROCESSING] = ( std::clock() - startTime ) / Scalar( CLOCKS_PER_SEC );

    if( isVerbose() ) {
        if( status ) {
            LOG( logINFO ) << "Preprocessing completed in " << std::to_string( m_time[PREPROCESSING] );
        } else {
            LOG( logINFO ) << "### PREPROCESSING STAGE FAILED ###";
        }
    }

    return status;
}



template < typename PARAMETERS >
inline bool Algorithm< PARAMETERS >::run_processing( uint& exitStatus ) {
    bool status;

    if( isVerbose() ) {
        LOG( logINFO ) << "Starting processing stage...";
    }

    std::clock_t startTime;
    startTime = std::clock();
    if( !( status = processing( exitStatus ) ) ) {
        m_state = AlgorithmState::PROCESSING_FAILED;
    }
    m_time[PROCESSING] = ( std::clock() - startTime ) / Scalar( CLOCKS_PER_SEC );

    if( isVerbose() ) {
        if( status ) {
            LOG( logINFO ) << "Processing completed in " << std::to_string( m_time[PROCESSING] );
        } else {
            LOG( logINFO ) << "### PROCESSING STAGE FAILED ###";
        }
    }

    return status;
}



template < typename PARAMETERS >
inline bool Algorithm< PARAMETERS >::run_postprocessing( uint& exitStatus ) {
    bool status;

    if( isVerbose() ) {
        LOG( logINFO ) << "Starting postprocessing stage...";
    }

    std::clock_t startTime;
    startTime = std::clock();
    if( !( status = postprocessing( exitStatus ) ) ) {
        m_state = AlgorithmState::POSTPROCESSING_FAILED;
        return;
    }
    m_time[POSTPROCESSING] = ( std::clock() - startTime ) / Scalar( CLOCKS_PER_SEC );

    if( isVerbose() ) {
        if( status ) {
            LOG( logINFO ) << "Postprocessing completed in " << std::to_string( m_time[POSTPROCESSING] );
        } else {
            LOG( logINFO ) << "### POSTPROCESSING STAGE FAILED ###";
        }
    }

    return status;
}

//=====================================================================
//=====================================================================

/// ALGORITHM STAGE
template < typename PARAMETERS >
virtual bool  Algorithm< PARAMETERS >::preprocessing( uint& exitStatus ) {
    return true;
}

template < typename PARAMETERS >
virtual bool     Algorithm< PARAMETERS >::processing( uint& exitStatus ) {
    return true;
}

template < typename PARAMETERS >
virtual bool Algorithm< PARAMETERS >::postprocessing( uint& exitStatus ) {
    return true;
}



} // namespace Core
} // namespace Ra
