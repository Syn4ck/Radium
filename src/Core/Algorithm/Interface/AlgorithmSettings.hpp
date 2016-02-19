#ifndef RADIUMENGINE_ALGORITHM_SETTINGS_DEFINITION_HPP
#define RADIUMENGINE_ALGORITHM_SETTINGS_DEFINITION_HPP

#include <Core/CoreMacros.hpp>

namespace Ra {
namespace Core {

class AlgorithmSettings {
public:
    /// CONSTRUCTOR
    AlgorithmSettings( const uchar priority = 0 );

    /// DESTRUCTOR
    ~AlgorithmSettings();

    /// PRIORITY
    inline unsigned char getPriority() const;
    inline void          setPriority( const uchar priority );

    /// SETUP
    inline bool runSetup() const;
    inline void toggleSetup( const bool status );
    inline bool forceSetup() const;
    inline void toggleForceSetup( const bool status );

    /// CONFIGURATION CHECK
    inline bool runConfigCheck() const;
    inline void toggleConfigCheck( const bool status );
    inline bool forceConfigCheck() const;
    inline void toggleForceConfigCheck( const bool status );

    /// PREPROCESSING
    inline bool runPreProcessing() const;
    inline void togglePreProcessing( const bool status );

    /// PROCESSING
    inline bool runProcessing() const;
    inline void toggleProcessing( const bool status );

    /// POSTPROCESSING
    inline bool runPostProcessing() const;
    inline void togglePostProcessing( const bool status );

    /// MID RESULTS
    inline bool storeMidResults() const;
    inline void toggleStoreMidResults( const bool status );

    /// MULTI THREAD
    inline bool isMultiThread() const;
    inline void toggleMultiThread( const bool status );

    /// VERBOSITY
    inline bool isVerbose() const;
    inline void toggleVerbose( const bool status );

    /// ASSERT
    inline bool isFailOnAssert() const;
    inline void toggleFailOnAssert( const bool status );

protected:
    /// VARIABLE
    // Priority
    uchar m_priority;

    // Flow variables
    bool m_runSetup;
    bool m_runConfigCheck;
    bool m_runPreProcessing;
    bool m_runProcessing;
    bool m_runPostProcessing;
    bool m_forceSetup;
    bool m_forceConfigCheck;

    // Mid-results
    bool m_step;

    // Multi-thread
    bool m_multiThread;

    // Verbose
    bool m_verbose;

    // Check
    bool m_failAssert;
};

} // namespace Core
} // namespace Ra

#include <Core/Algorithm/Interface/AlgorithmSettings.inl>

#endif // RADIUMENGINE_ALGORITHM_SETTINGS_DEFINITION_HPP
