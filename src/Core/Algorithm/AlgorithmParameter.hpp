#ifndef RADIUMENGINE_ALGORITHM_PARAMETER_DEFINITION_HPP
#define RADIUMENGINE_ALGORITHM_PARAMETER_DEFINITION_HPP

#include <vector>
#include <string>
#include <Core/CoreMacros.hpp>

namespace Ra {
namespace Core {

class AlgorithmParameter {
public:
    /// CONSTRUCTOR
    AlgorithmParameter();

    /// DESTRUCTOR
    ~AlgorithmParameter();

    /// INTERFACE
    virtual void setDefault() = 0;
};


class AlgorithmError {
public:
    /// CONSTRUCTOR

    /// DESTRUCTOR

protected:
    /// VARIABLE
};





template < typename INPUT, typename OUTPUT, typename PARAMETER >
class Algorithm {
public:
    /// ENUM
    enum class AlgorithmState {
        READY,
        RUNNING,
        SETUP_FAILED,
        NOT_CONFIGURED,
        PREPROCESSING_FAILED,
        PROCESSING_FAILED,
        POSTPROCESSING_FAILED,
        COMPLETED
    };

    /// CONSTRUCTOR
    Algorithm( const std::string name      = "",
               INPUT*  const     input     = nullptr,
               OUTPUT* const     output    = nullptr,
               const PARAMETER&  parameter = PARAMETER() );

    /// DESTRUCTOR
    ~Algolrithm() {
        for( const auto& entry : m_step ) {
            delete entry;
        }
    }

    /// INPUT
    inline INPUT* getInput() const {
        return m_input;
    }

    inline void   setInput( INPUT* const input ) {
        m_input = input;
    }

    /// OUTPUT
    inline OUTPUT* getOutput() const {
        return m_output;
    }
    inline void    setOutput( OUTPUT* const output ) {
        m_output = output;
    }

    /// PARAMETER
    inline PARAMETER getParameter() const {
        return m_param;
    }

    inline void      setParameter( const PARAMETER& parameter ) {
        m_param = parameter;
    }

    /// SETTING
    inline AlgorithmSetting getSetting() const {
        return m_setting;
    }

    inline AlgorithmSetting& getSetting() {
        return m_setting;
    }

    inline void             setSetting( const AlgorithmSetting& setting ) {
        m_setting = setting;
    }

    /// STEP
    inline uint          getStepSize() const {
        return m_step.size();
    }

    inline const OUTPUT& getStep( const uint i ) const {
        return *m_step.at( i );
    }

    /// NAME
    inline std::string getName() const {
        return m_name;
    }

    /// STATE
    inline AlgorithmState getState()    const {
        return m_state;
    }

    inline bool           isCompleted() const {
        return ( m_state == COMPLETED );
    }

    inline bool           isFailed()    const {
        return ( ( m_state == READY ) || ( m_state != RUNNING ) || ( m_state != COMPLETED ) );
    }

    /// EXIT STATUS
    inline uint getExitStatus() const {
        return m_exitStatus;
    }

protected:
    /// STEP
    inline void addStep( OUTPUT* step ) {
        if( step != nullptr ) {
            m_step.push_back( step );
        }
    }

    /// ALGORITHM STAGE
    virtual bool          setup( uint& exitStatus ) = 0;
    virtual bool    configCheck( uint& exitStatus ) = 0;
    virtual bool  preprocessing( uint& exitStatus ) = 0;
    virtual bool     processing( uint& exitStatus ) = 0;
    virtual bool postprocessing( uint& exitStatus ) = 0;

private:
    /// ALGORITHM STAGE
    inline bool run_setup         ( uint& exitStatus );
    inline bool run_configCheck   ( uint& exitStatus );
    inline bool run_preprocessing ( uint& exitStatus );
    inline bool run_processing    ( uint& exitStatus );
    inline bool run_postprocessing( uint& exitStatus );

protected:
    /// VARIABLE
    INPUT*           m_input;
    OUTPUT*          m_output;
    PARAMETER        m_param;
    AlgorithmSetting m_setting;

private:
    /// VARIABLE
    std::vector< OUTPUT* > m_step;
    std::string            m_name;
    AlgorithmState         m_state;
    uint                   m_exitStatus;
};






class AlgorithmSetting {
public:
    /// ENUM
    enum DataType {
        PRIORITY            = 255,
        MULTI_THREAD        = 1 << 8,
        VERBOSE             = 1 << 9,
        FAIL_ON_ASSERT      = 1 << 10,
        RUN_SETUP           = 1 << 11,
        FORCE_SETUP         = 1 << 12,
        RUN_CONFIG_CHECK    = 1 << 13,
        FORCE_CONFIG_CHECK  = 1 << 14,
        RUN_PREPROCESSING   = 1 << 15,
        RUN_PROCESSING      = 1 << 16,
        RUN_POSTPROCESSING  = 1 << 17,
        STORE_STEP          = 1 << 18
    };

    /// CONSTRUCTOR
    AlgorithmSetting( const uint setting = ( RUN_SETUP | FORCE_CONFIG_CHECK | RUN_CONFIG_CHECK | RUN_PREPROCESSING | RUN_PROCESSING | RUN_POSTPROCESSING ) );

    /// DESTRUCTOR
    ~AlgorithmSetting();

    /// DATA

    /// PRIORITY
    inline unsigned char getPriority() const;
    inline void          setPriority( const unsigned char priority );

    /// MULTI THREAD
    inline bool isMultiThread() const;
    inline void toggleMultiThread( const bool status );

    /// VERBOSITY
    inline bool isVerbose() const;
    inline void toggleVerbose( const bool status );

    /// ASSERT
    inline bool isFailOnAssert() const;
    inline void toggleFailOnAssert( const bool status );

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

protected:
    /// VARIABLE
    uint m_state;
};


class AlgorithmStatistics {
public:
    /// ENUM
    enum class StageType {
        SETUP,
        CONFIG_CHECK,
        PREPROCESSING,
        PROCESSING,
        POSTPROCESSING,
        OVERALL
    };

    /// CONSTRUCTOR
    AlgorithmStatistics();

    /// DESTRUCTOR
    AlgorithmStatistics();

    /// TIMING
    inline Scalar getTiming( const StageType stage ) const;
    inline void   setTiming( const StageType stage, const Scalar timing );

    /// PERCENTAGE
    inline Scalar getPercentage( const StageType stage ) const;
    inline void   setPercentage( const StageType stage , const Scalar percentage );

    /// SETUP
    inline Scalar getSetupTiming() const;
    inline void   setSetupTiming( const Scalar timing );

    /// CONFIGURATION CHECK
    inline Scalar getConfigCheckTiming() const;
    inline void   setConfigCheckTiming( const Scalar timing );

    /// PREPROCESSING
    inline Scalar getPreProcessingTiming() const;
    inline void   setPreProcessingTiming( const Scalar timing );

    /// PROCESSING
    inline Scalar getProcessingTiming() const;
    inline void   setProcessingTiming( const Scalar timing );

    /// POSTPROCESSING
    inline Scalar getPostProcessingTiming() const;
    inline void   setPostProcessingTiming( const Scalar timing );

    /// OVERALL
    inline Scalar getOverallTiming() const;
    inline void   setOverallTiming( const Scalar timing );

    /// RESET
    inline void reset();

    /// STRING
    inline std::string to_string() const;


protected:
    /// VARIABLE
    Scalar m_timing[6];
    Scalar m_percentage[6];
    Scalar m_setup;
    Scalar m_confiCheck;
    Scalar m_preProcessing;
    Scalar m_processing;
    Scalar m_postProcessing;
    Scalar m_overall;
};




} // namespace Core
} // namespace Ra

#endif // RADIUMENGINE_ALGORITHM_PARAMETER_DEFINITION_HPP
