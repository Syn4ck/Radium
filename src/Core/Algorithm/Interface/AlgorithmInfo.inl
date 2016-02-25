#include <Core/Algorithm/Interface/AlgorithmInfo.hpp>

namespace Ra {
namespace Core {

/// NAME
inline std::string AlgorithmInfo::getName() const {
    return m_name;
}

inline void AlgorithmInfo::setName( const std::string& name ) {
    m_name = name;
}

/// ACRONYM
inline std::string AlgorithmInfo::getAcronym() const {
    return m_acronym;
}

inline void AlgorithmInfo::setAcronym( const std::string& acronym ) {
    m_acronym = acronym;
}

/// DESCRIPTION
inline std::string AlgorithmInfo::getDescription() const {
    return m_description;
}

inline void AlgorithmInfo::setDescription( const std::string& description ) {
    m_description = description;
}

} // namespace Core
} // namespace Ra
