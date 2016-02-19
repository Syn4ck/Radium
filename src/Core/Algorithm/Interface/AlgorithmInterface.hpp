#ifndef RADIUMENGINE_ALGORITHM_INTERFACE_DEFINITION_HPP
#define RADIUMENGINE_ALGORITHM_INTERFACE_DEFINITION_HPP

#include <vector>
#include <string>
#include <Core/CoreMacros.hpp>

#include <Core/Algorithm/Interface/AlgorithmStep.hpp>
#include <Core/Algorithm/Interface/AlgorithmSettings.hpp>
#include <Core/Algorithm/Interface/AlgorithmStatistics.hpp>

namespace Ra {
namespace Core {

template < typename INPUT, typename OUTPUT, typename PARAMETER >
class AlgorithmInterface {
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
    AlgorithmInterface( const std::string  name      = "",
                        const INPUT* const input     = nullptr,
                        OUTPUT* const      output    = nullptr,
                        const PARAMETER&   parameter = PARAMETER() );

    /// DESTRUCTOR
    ~AlgorithmInterface();

    /// INPUT
    inline INPUT* getInput() const;
    inline void   setInput( INPUT* const input );

    /// OUTPUT
    inline OUTPUT* getOutput() const;
    inline void    setOutput( OUTPUT* const output );

    /// PARAMETER
    inline PARAMETER getParameter() const;
    inline void      setParameter( const PARAMETER& parameter );

    /// SETTINGS
    inline AlgorithmSettings  getSettings() const;
    inline AlgorithmSettings& getSettings();
    inline void               setSettings( const AlgorithmSettings& settings );

    /// STATISTICS
    inline AlgorithmStatistics getStatistics() const;

    /// STEP
    inline uint                          getStepSize() const;
    inline const AlgorithmStep< OUTPUT > getStep( const uint i ) const;

    /// NAME
    inline std::string getName() const;

    /// STATE
    inline AlgorithmState getState()    const;
    inline bool           isCompleted() const;
    inline bool           isFailed()    const;

    /// EXIT STATUS
    inline uint getExitStatus() const;

    /// RUN
    inline uint run();

protected:
    /// STEP
    inline OUTPUT* addStep( const std::string& name );

    /// ALGORITHM STAGE
    virtual bool          setup( uint& exitStatus ) = 0;
    virtual bool    configCheck( uint& exitStatus ) = 0;
    virtual bool  preprocessing( uint& exitStatus ) = 0;
    virtual bool     processing( uint& exitStatus ) = 0;
    virtual bool postprocessing( uint& exitStatus ) = 0;

private:
    /// STEP
    inline void deleteStep();

    /// ALGORITHM STAGE
    inline bool run_setup         ( uint& exitStatus );
    inline bool run_configCheck   ( uint& exitStatus );
    inline bool run_preprocessing ( uint& exitStatus );
    inline bool run_processing    ( uint& exitStatus );
    inline bool run_postprocessing( uint& exitStatus );

protected:
    /// VARIABLE
    const INPUT*        m_input;
    OUTPUT*             m_output;
    PARAMETER           m_param;
    AlgorithmSettings   m_settings;
    AlgorithmStatistics m_statistics;

private:
    /// VARIABLE
    std::vector< AlgorithmStep< OUTPUT > > m_step;
    std::string                            m_name;
    AlgorithmState                         m_state;
    uint                                   m_exitStatus;
};



} // namespace Core
} // namespace Ra

#include <Core/Algorithm/Interface/AlgorithmInterface.inl>

#endif // RADIUMENGINE_ALGORITHM_INTERFACE_DEFINITION_HPP
