#include <Core/Algorithm/Interface/AlgorithmSettings.hpp>

namespace Ra {
namespace Core {

/// PRIORITY
inline unsigned char AlgorithmSettings::getPriority() const {
    return m_priority;
}

inline void AlgorithmSettings::setPriority( const uchar priority ) {
    m_priority = priority;
}



/// SETUP
inline bool AlgorithmSettings::runSetup() const {
    return m_runSetup;
}

inline void AlgorithmSettings::toggleSetup( const bool status ) {
    m_runSetup = status;
}

inline bool AlgorithmSettings::forceSetup() const {
    return m_forceSetup;
}

inline void AlgorithmSettings::toggleForceSetup( const bool status ) {
    m_forceSetup = status;
}



/// CONFIGURATION CHECK
inline bool AlgorithmSettings::runConfigCheck() const {
    return m_runConfigCheck;
}

inline void AlgorithmSettings::toggleConfigCheck( const bool status ) {
    m_runConfigCheck = status;
}

inline bool AlgorithmSettings::forceConfigCheck() const {
    return m_forceConfigCheck;
}

inline void AlgorithmSettings::toggleForceConfigCheck( const bool status ) {
    m_forceConfigCheck = status;
}



/// PREPROCESSING
inline bool AlgorithmSettings::runPreProcessing() const {
    return m_runPreProcessing;
}

inline void AlgorithmSettings::togglePreProcessing( const bool status ) {
    m_runPreProcessing = status;
}



/// PROCESSING
inline bool AlgorithmSettings::runProcessing() const {
    return m_runProcessing;
}

inline void AlgorithmSettings::toggleProcessing( const bool status ) {
    m_runProcessing = status;
}



/// POSTPROCESSING
inline bool AlgorithmSettings::runPostProcessing() const {
    return m_runPostProcessing;
}

inline void AlgorithmSettings::togglePostProcessing( const bool status ) {
    m_runPostProcessing = status;
}



/// MID RESULTS
inline bool AlgorithmSettings::storeMidResults() const {
    return m_step;
}

inline void AlgorithmSettings::toggleStoreMidResults( const bool status ) {
    m_step = status;
}



/// MULTI THREAD
inline bool AlgorithmSettings::isMultiThread() const {
    return m_multiThread;
}

inline void AlgorithmSettings::toggleMultiThread( const bool status ) {
    m_multiThread = status;
}



/// VERBOSITY
inline bool AlgorithmSettings::isVerbose() const {
    return m_verbose;
}

inline void AlgorithmSettings::toggleVerbose( const bool status ) {
    m_verbose = status;
}



/// ASSERT
inline bool AlgorithmSettings::isFailOnAssert() const {
    return m_failAssert;
}

inline void AlgorithmSettings::toggleFailOnAssert( const bool status ) {
    m_failAssert = status;
}


} // namespace Core
} // namespace Ra
