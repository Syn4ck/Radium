#ifndef RADIUMENGINE_ALGORITHM_EXECUTION_DEFINITION_HPP
#define RADIUMENGINE_ALGORITHM_EXECUTION_DEFINITION_HPP

#include <Core/CoreMacros.hpp>
#include <Core/Algorithm/Interface/AlgorithmStage.hpp>

namespace Ra {
namespace Core {

class AlgorithmExecution {
public:
    /// CONSTRUCTOR
    AlgorithmExecution( const uchar priority = 0 );

    /// DESTRUCTOR
    ~AlgorithmExecution();

    /// RUN
    inline bool   enabled( const AlgorithmStage& stage ) const;
    inline void setEnable( const AlgorithmStage& stage, const bool status );

    /// FORCE
    inline bool forced( const AlgorithmStage& stage ) const;
    inline void  force( const AlgorithmStage& stage, const bool status );

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
    bool m_run[OVERALL];
    bool m_force[OVERALL]; // PREPROCESSING, PROCESSING and POSTPROCESSING unusued;
};

} // namespace Core
} // namespace Ra

#include <Core/Algorithm/Interface/AlgorithmExecution.inl>

#endif // RADIUMENGINE_ALGORITHM_EXECUTION_DEFINITION_HPP
