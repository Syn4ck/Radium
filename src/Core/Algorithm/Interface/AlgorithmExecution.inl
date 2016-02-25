#include <Core/Algorithm/Interface/AlgorithmExecution.hpp>

namespace Ra {
namespace Core {

/// RUN
inline bool AlgorithmExecution::enabled( const AlgorithmStage& stage ) const {
    return m_run[stage];
}

inline void AlgorithmExecution::setEnable( const AlgorithmStage& stage, const bool status ) {
    m_run[stage] = status;
}

/// FORCE
inline bool AlgorithmExecution::forced( const AlgorithmStage& stage ) const {
    return m_force[stage];
}

inline void AlgorithmExecution::force( const AlgorithmStage& stage, const bool status ) {
    m_foce[stage] = status;
}


/// SETUP
inline bool AlgorithmExecution::runSetup() const {
    return enabled( SETUP );
}

inline void AlgorithmExecution::toggleSetup( const bool status ) {
    setEnable( SETUP, status );
}

inline bool AlgorithmExecution::forceSetup() const {
    forced( SETUP );
}

inline void AlgorithmExecution::toggleForceSetup( const bool status ) {
    force( SETUP, status );
}



/// CONFIGURATION CHECK
inline bool AlgorithmExecution::runConfigCheck() const {
    return enabled( CONFIG_CHECK );
}

inline void AlgorithmExecution::toggleConfigCheck( const bool status ) {
    setEnable( CONFIG_CHECK, status );
}

inline bool AlgorithmExecution::forceConfigCheck() const {
    forced( CONFIG_CHECK );
}

inline void AlgorithmExecution::toggleForceConfigCheck( const bool status ) {
    force( CONFIG_CHECK, status );
}



/// PREPROCESSING
inline bool AlgorithmExecution::runPreProcessing() const {
    return enabled( PREPROCESSING );
}

inline void AlgorithmExecution::togglePreProcessing( const bool status ) {
    setEnable( PREPROCESSING, status );
}



/// PROCESSING
inline bool AlgorithmExecution::runProcessing() const {
    return enabled( PROCESSING );
}

inline void AlgorithmExecution::toggleProcessing( const bool status ) {
    setEnable( PROCESSING, status );
}



/// POSTPROCESSING
inline bool AlgorithmExecution::runPostProcessing() const {
    return enabled( POSTPROCESSING );
}

inline void AlgorithmExecution::togglePostProcessing( const bool status ) {
    setEnable( POSTPROCESSING, status );
}



} // namespace Core
} // namespace Ra
