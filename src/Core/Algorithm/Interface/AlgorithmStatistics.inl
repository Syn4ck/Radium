#include <Core/Algorithm/Interface/AlgorithmStatistics.hpp>

namespace Ra {
namespace Core {

/// TIMING
inline Scalar AlgorithmStatistics::getTiming( const StageType stage ) const {
    return m_timing[stage];
}

inline void AlgorithmStatistics::setTiming( const StageType stage,
                                            const Scalar    timing ) {
    m_timing[stage] = ( timing < 0.0 ) ? 0.0 : timing;
}

inline Scalar AlgorithmStatistics::getStatistic( const StageType stage ) const {
    return m_statistic[stage];
}



/// PERCENTAGE
inline Scalar AlgorithmStatistics::getPercentage( const StageType stage ) const {
    return m_percentage[stage];
}

inline void AlgorithmStatistics::setPercentage( const StageType stage,
                                                const Scalar    percentage ) {
    m_percentage[stage] = ( percentage < 0.0 ) ? 0.0 : ( percentage > 1.0 ) ? 1.0 : percentage;
}

inline void AlgorithmStatistics::addPercentage( const StageType stage,
                                                const Scalar    percentage ) {
    setPercentage( stage, ( m_percentage[stage] + ( ( percentage < 0.0 ) ? 0.0 : ( percentage > 1.0 ) ? 1.0 : percentage ) ) );
}



/// SETUP
inline Scalar AlgorithmStatistics::getSetupTiming() const {
    return m_timing[SETUP];
}

inline void AlgorithmStatistics::setSetupTiming( const Scalar timing ) {
    setTiming( SETUP, timing );
}

inline Scalar AlgorithmStatistics::getSetupPercentage() const {
    return m_percentage[SETUP];
}

inline void AlgorithmStatistics::setSetupPercentage( const Scalar percentage ) {
    setPercentage( SETUP, percentage );
}

inline void AlgorithmStatistics::addSetupPercentage( const Scalar percentage ) {
    addPercentage( SETUP, percentage );
}

inline Scalar AlgorithmStatistics::getSetupStatistic() const {
    return m_statistic[SETUP];
}



/// CONFIGURATION CHECK
inline Scalar AlgorithmStatistics::getConfigCheckTiming() const {
    return m_timing[CONFIG_CHECK];
}

inline void AlgorithmStatistics::setConfigCheckTiming( const Scalar timing ) {
    setTiming( CONFIG_CHECK, timing );
}

inline Scalar AlgorithmStatistics::getConfigCheckPercentage() const {
    return m_percentage[CONFIG_CHECK];
}

inline void AlgorithmStatistics::setConfigCheckPercentage( const Scalar percentage ) {
    setPercentage( CONFIG_CHECK, percentage );
}

inline void AlgorithmStatistics::addConfigCheckPercentage( const Scalar percentage ) {
    addPercentage( CONFIG_CHECK, percentage );
}

inline Scalar AlgorithmStatistics::getConfigCheckStatistic() const {
    return m_statistic[CONFIG_CHECK];
}



/// PREPROCESSING
inline Scalar AlgorithmStatistics::getPreProcessingTiming() const {
    return m_timing[PREPROCESSING];
}

inline void AlgorithmStatistics::setPreProcessingTiming( const Scalar timing ) {
    setTiming( PREPROCESSING, timing );
}

inline Scalar AlgorithmStatistics::getPreProcessingPercentage() const {
    return m_percentage[PREPROCESSING];
}

inline void AlgorithmStatistics::setPreProcessingPercentage( const Scalar percentage ) {
    setPercentage( PREPROCESSING, percentage );
}

inline void AlgorithmStatistics::addPreProcessingPercentage( const Scalar percentage ) {
    addPercentage( PREPROCESSING, percentage );
}

inline Scalar AlgorithmStatistics::getPreProcessingStatistic() const {
    return m_statistic[PREPROCESSING];
}



/// PROCESSING
inline Scalar AlgorithmStatistics::getProcessingTiming() const {
    return m_timing[PROCESSING];
}

inline void AlgorithmStatistics::setProcessingTiming( const Scalar timing ) {
    setTiming( PROCESSING, timing );
}

inline Scalar AlgorithmStatistics::getProcessingPercentage() const {
    return m_percentage[PROCESSING];
}

inline void AlgorithmStatistics::setProcessingPercentage( const Scalar percentage ) {
    setPercentage( PROCESSING, percentage );
}

inline void AlgorithmStatistics::addProcessingPercentage( const Scalar percentage ) {
    addPercentage( PROCESSING, percentage );
}

inline Scalar AlgorithmStatistics::getProcessingStatistic() const {
    return m_statistic[PROCESSING];
}



/// POSTPROCESSING
inline Scalar AlgorithmStatistics::getPostProcessingTiming() const {
    return m_timing[POSTPROCESSING];
}

inline void AlgorithmStatistics::setPostProcessingTiming( const Scalar timing ) {
    setTiming( POSTPROCESSING, timing );
}

inline Scalar AlgorithmStatistics::getPostProcessingPercentage() const {
    return m_percentage[POSTPROCESSING];
}

inline void AlgorithmStatistics::setPostProcessingPercentage( const Scalar percentage ) {
    setPercentage( POSTPROCESSING, percentage );
}

inline void AlgorithmStatistics::addPostProcessingPercentage( const Scalar percentage ) {
    addPercentage( POSTPROCESSING, percentage );
}

inline Scalar AlgorithmStatistics::getPostProcessingStatistic() const {
    return m_statistic[POSTPROCESSING];
}



/// OVERALL
inline Scalar AlgorithmStatistics::getOverallTiming() const {
    return m_timing[OVERALL];
}

inline void AlgorithmStatistics::setOverallTiming( const Scalar timing ) {
    setTiming( OVERALL, timing );
}

inline Scalar AlgorithmStatistics::getOverallPercentage() const {
    return m_percentage[OVERALL];
}

inline void AlgorithmStatistics::setOverallPercentage( const Scalar percentage ) {
    setPercentage( OVERALL, percentage );
}

inline void AlgorithmStatistics::addOverallPercentage( const Scalar percentage ) {
    addPercentage( OVERALL, percentage );
}

inline Scalar AlgorithmStatistics::getOverallStatistic() const {
    return m_statistic[OVERALL];
}

inline void AlgorithmStatistics::computeOverallTiming() {
    m_timing[OVERALL] = 0.0;
    for( uint i = 0; i < 5; ++i ) {
        m_timing[OVERALL] += m_timing[i];
    }
}

inline void AlgorithmStatistics::computeOverallPercentage() {
    Scalar total   = 0.0;
    Scalar current = 0.0;
    for( uint i = 0; i < 5; ++i ) {
        total += 1.0;
        current += m_percentage[i];
    }
    m_percentage[OVERALL] = current / total;
}

inline void AlgorithmStatistics::computeStatistics() {
    if( m_timing[OVERALL] != 0.0 ) {
        for( uint i = 0; i < 5; ++i ) {
             m_statistic[i] = m_timing[i] / m_timing[OVERALL];
        }
        m_statistic[OVERALL] = 1.0;
    }
}


/// RESET
inline void AlgorithmStatistics::reset() {
    for( uint i = 0; i < 6; ++i ) {
            m_timing[i] = 0.0;
        m_percentage[i] = 0.0;
         m_statistic[i] = 1.0;
    }
}

} // namespace Core
} // namespace Ra
