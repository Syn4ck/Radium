#include <Core/Algorithm/Interface/AlgorithmStep.hpp>

namespace Ra {
namespace Core {

/// CONSTRUCTOR
template < typename TYPE >
AlgorithmStep< TYPE >::AlgorithmStep( const std::string& name,
                                      TYPE* const        data ) :
    m_name( name ),
    m_data( data ) { }

/// DESTRUCTOR
template < typename TYPE >
AlgorithmStep< TYPE >::~AlgorithmStep() {
    /*if( m_data != nullptr ) {
        delete m_data;
    }*/
}

/// NAME
template < typename TYPE >
inline std::string AlgorithmStep< TYPE >::getName() const {
    return m_name;
}

/// DATA
template < typename TYPE >
inline TYPE* AlgorithmStep< TYPE >::getData() {
    return m_data;
}


} // namespace Core
} // namespace Ra
