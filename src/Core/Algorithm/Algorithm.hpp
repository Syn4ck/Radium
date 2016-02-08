#ifndef RADIUMENGINE_ALGORITHM_DEFINITION_HPP
#define RADIUMENGINE_ALGORITHM_DEFINITION_HPP

#include <Core/CoreMacros.hpp>
#include <string>

namespace Ra {
namespace Core {

template < typename PARAMETERS >
class Algorithm {
public:
    /// ENUM
    enum class AlgorithmState {
        READY,
        RUNNING,
        NOT_CONFIGURED,
        PREPROCESSING_FAILED,
        PROCESSING_FAILED,
        POSTPROCESSING_FAILED,
        COMPLETED
    };

    enum AlgorithmStageTiming {
        PREPROCESSING,
        PROCESSING,
        POSTPROCESSING,
        OVERALL,
        TOTAL_STAGE
    };

    /// CONSTRUCTOR
    Algorithm( const PARAMETERS&  param,
               const std::string& name      = "",
               const bool         verbosity = false );

    /// DESTRUCTOR
    ~Algorithm();

    /// PARAMETER
    inline PARAMETERS getParameters() const;
    inline void       setParameters( const PARAMETERS& param );

    /// NAME
    inline std::string getName() const;

    /// STATE
    inline AlgorithmState getState() const;
    inline bool isCompleted() const;
    inline bool isFailed() const;

    /// EXIT STATUS
    inline uint getExitStatus() const;

    /// TIMING
    inline Scalar getTiming( const AlgorithmStageTiming& stage ) const;

    /// VERBOSITY
    inline bool isVerbose() const;
    inline void setVerbosity( const bool verbosity );

    /// RUN
    inline uint run();

private:
    /// ALGORITHM STAGE
    inline bool run_configCheck   ( uint& exitStatus );
    inline bool run_preprocessing ( uint& exitStatus );
    inline bool run_processing    ( uint& exitStatus );
    inline bool run_postprocessing( uint& exitStatus );

    /// VARIABLE
    std::string    m_name;
    AlgorithmState m_state;
    uint           m_exitStatus;
    Scalar         m_time[TOTAL_STAGE];

protected:
    /// ALGORITHM STAGE
    virtual bool    configCheck( uint& exitStatus ) = 0;
    virtual bool  preprocessing( uint& exitStatus ) = 0;
    virtual bool     processing( uint& exitStatus ) = 0;
    virtual bool postprocessing( uint& exitStatus ) = 0;

    /// VARIABLE
    PARAMETERS m_param;
    bool       m_verbosity;
};

} // namespace Core
} // namespace Ra

#include <Core/Algorithm/Algorithm.inl>

#endif // RADIUMENGINE_ALGORITHM_DEFINITION_HPP
