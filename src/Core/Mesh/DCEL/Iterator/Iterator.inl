#include <Core/Mesh/DCEL/Iterator/Iterator.hpp>

#include <Core/Mesh/DCEL/HalfEdge.hpp>

namespace Ra {
namespace Core {


/*
/// CONSTRUCTOR
template < typename OBJECT >
Iterator< OBJECT >::Iterator() : m_he( nullptr ) { }






/// DESTRUCTOR
template < typename OBJECT >
Iterator< OBJECT >::~Iterator() { }






/// OPERATOR
template < typename OBJECT >
inline Iterator< OBJECT >& Iterator< OBJECT >::operator+=( const uint n ) const {
    for( uint i = 0; i < n; ++i ) {
        ++( *this );
    }
    return *this;
}



template < typename OBJECT >
inline Iterator< OBJECT >& Iterator< OBJECT >::operator-=( const uint n ) const {
    for( uint i = 0; i < n; ++i ) {
        --( *this );
    }
    return *this;
}
*/


/// CONSTRUCTOR
template < typename FROM_OBJECT, typename TO_OBJECT >
DCEL_Iterator< FROM_OBJECT, TO_OBJECT >::DCEL_Iterator( const FROM_OBJECT& object ) :
    m_object( object ),
    m_he( nullptr ) {
    m_he = m_object->HE();
}






/// DESTRUCTOR
template < typename FROM_OBJECT, typename TO_OBJECT >
DCEL_Iterator< FROM_OBJECT, TO_OBJECT >::~DCEL_Iterator() { }






/// RESET
template < typename FROM_OBJECT, typename TO_OBJECT >
inline void DCEL_Iterator< FROM_OBJECT, TO_OBJECT >::reset() {
    this->m_he = this->m_object->HE();
}






/// OPERATOR
template < typename FROM_OBJECT, typename TO_OBJECT >
inline DCEL_Iterator< FROM_OBJECT, TO_OBJECT >& DCEL_Iterator< FROM_OBJECT, TO_OBJECT >::operator+=( const uint n ) {
    for( uint i = 0; i < n; ++i ) {
        ++( *this );
    }
    return *this;
}



template < typename FROM_OBJECT, typename TO_OBJECT >
inline DCEL_Iterator< FROM_OBJECT, TO_OBJECT >& DCEL_Iterator< FROM_OBJECT, TO_OBJECT >::operator-=( const uint n ) {
    for( uint i = 0; i < n; ++i ) {
        --( *this );
    }
    return *this;
}



} // namespace Core
} // namespace Ra
