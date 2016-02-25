#include <Core/Algorithm/Interface/AlgorithmInfo.hpp>

#include <string>

namespace Ra {
namespace Core {

/// CONSTRUCTOR
AlgorithmInfo::AlgorithmInfo( const std::string& name,
                              const std::string& acronym,
                              const std::string& description ) :
    m_name( name ),
    m_acronym( acronym ),
    m_description( description ) {
    if( m_acronym == "" ) {
        m_acronym = m_name;
    }
}

/// DESTRUCTOR
AlgorithmInfo::~AlgorithmInfo();


} // namespace Core
} // namespace Ra
