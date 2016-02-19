#ifndef RADIUMENGINE_ALGORITHM_STATISTICS_DEFINITION_HPP
#define RADIUMENGINE_ALGORITHM_STATISTICS_DEFINITION_HPP

#include <Core/CoreMacros.hpp>

namespace Ra {
namespace Core {

class AlgorithmStatistics {
public:
    /// ENUM
    enum StageType {
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
    ~AlgorithmStatistics();

    /// TIMING
    inline Scalar getTiming( const StageType stage ) const;
    inline void   setTiming( const StageType stage, const Scalar timing );
    inline Scalar getStatistic( const StageType stage ) const;

    /// PERCENTAGE
    inline Scalar getPercentage( const StageType stage ) const;
    inline void   setPercentage( const StageType stage, const Scalar percentage );
    inline void   addPercentage( const StageType stage, const Scalar percentage );

    /// SETUP
    inline Scalar getSetupTiming() const;
    inline void   setSetupTiming( const Scalar timing );
    inline Scalar getSetupPercentage() const;
    inline void   setSetupPercentage( const Scalar percentage );
    inline void   addSetupPercentage( const Scalar percentage );
    inline Scalar getSetupStatistic() const;

    /// CONFIGURATION CHECK
    inline Scalar getConfigCheckTiming() const;
    inline void   setConfigCheckTiming( const Scalar timing );
    inline Scalar getConfigCheckPercentage() const;
    inline void   setConfigCheckPercentage( const Scalar percentage );
    inline void   addConfigCheckPercentage( const Scalar percentage );
    inline Scalar getConfigCheckStatistic() const;

    /// PREPROCESSING
    inline Scalar getPreProcessingTiming() const;
    inline void   setPreProcessingTiming( const Scalar timing );
    inline Scalar getPreProcessingPercentage() const;
    inline void   setPreProcessingPercentage( const Scalar percentage );
    inline void   addPreProcessingPercentage( const Scalar percentage );
    inline Scalar getPreProcessingStatistic() const;

    /// PROCESSING
    inline Scalar getProcessingTiming() const;
    inline void   setProcessingTiming( const Scalar timing );
    inline Scalar getProcessingPercentage() const;
    inline void   setProcessingPercentage( const Scalar percentage );
    inline void   addProcessingPercentage( const Scalar percentage );
    inline Scalar getProcessingStatistic() const;

    /// POSTPROCESSING
    inline Scalar getPostProcessingTiming() const;
    inline void   setPostProcessingTiming( const Scalar timing );
    inline Scalar getPostProcessingPercentage() const;
    inline void   setPostProcessingPercentage( const Scalar percentage );
    inline void   addPostProcessingPercentage( const Scalar percentage );
    inline Scalar getPostProcessingStatistic() const;

    /// OVERALL
    inline Scalar getOverallTiming() const;
    inline void   setOverallTiming( const Scalar timing );
    inline Scalar getOverallPercentage() const;
    inline void   setOverallPercentage( const Scalar percentage );
    inline void   addOverallPercentage( const Scalar percentage );
    inline Scalar getOverallStatistic() const;
    inline void   computeOverallTiming();
    inline void   computeOverallPercentage();
    inline void   computeStatistics();

    /// RESET
    inline void reset();

protected:
    /// VARIABLE
    Scalar m_timing[6];
    Scalar m_percentage[6];
    Scalar m_statistic[6];
};

} // namespace Core
} // namespace Ra

#include <Core/Algorithm/Interface/AlgorithmStatistics.inl>

#endif // RADIUMENGINE_ALGORITHM_STATISTICS_DEFINITION_HPP
