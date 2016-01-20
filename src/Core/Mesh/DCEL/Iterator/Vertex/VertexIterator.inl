#include <Core/Mesh/DCEL/Iterator/Vertex/VertexIterator.hpp>

#include <Core/Mesh/DCEL/Vertex.hpp>

namespace Ra {
namespace Core {

/*
/// CONSTRUCTOR
template < typename OBJECT >
VIterator< OBJECT >::VIterator( Vertex_ptr& v ) :
    Iterator< OBJECT >(),
    m_v( v ) {
    this->m_he = m_v->HE();
}






/// DESTRUCTOR
template < typename OBJECT >
VIterator< OBJECT >::~VIterator() { }






/// SIZE
template < typename OBJECT >
inline uint VIterator< OBJECT >::size() const {
    uint i = 0;
    HalfEdge_ptr it = m_v->HE();
    do {
        it = it->Prev()->Twin();
        ++i;
    } while( it != m_v->HE() );
    return i;
}






/// RESET
template < typename OBJECT >
inline void VIterator< OBJECT >::reset() {
    this->m_he = m_v->HE();
}






/// OPERATOR
template < typename OBJECT >
inline VIterator< OBJECT >& VIterator< OBJECT >::operator=( const VIterator& it ) {
    m_v = it.m_v;
    this->m_he = it.m_he;
    return *this;
}



template < typename OBJECT >
inline VIterator< OBJECT >& VIterator< OBJECT >::operator++() {
    this->m_he = this->m_he->Prev()->Twin();
    return *this;
}



template < typename OBJECT >
inline VIterator< OBJECT >& VIterator< OBJECT >::operator--() {
    this->m_he = this->m_he->Twin()->Next();
    return *this;
}



template < typename OBJECT >
inline bool VIterator< OBJECT >::operator==( const VIterator& it ) const {
    return ( ( m_v == it.m_v ) && ( this->m_he == it.m_he ) );
}
*/


/// CONSTRUCTOR
template < typename TO_OBJECT >
VIterator< TO_OBJECT >::VIterator( Vertex_ptr& v ) :
    DCEL_Iterator< Vertex_ptr, TO_OBJECT >( v ) { }






/// DESTRUCTOR
template < typename TO_OBJECT >
VIterator< TO_OBJECT >::~VIterator() { }






/// SIZE
template < typename TO_OBJECT >
inline uint VIterator< TO_OBJECT >::size() const {
    uint i = 0;
    HalfEdge_ptr it = m_object->HE();
    do {
        it = it->Prev()->Twin();
        ++i;
    } while( it != m_object->HE() );
    return i;
}






/// RESET
template < typename TO_OBJECT >
inline void VIterator< TO_OBJECT >::reset() {
    this->m_he = m_object->HE();
}






/// OPERATOR
template < typename TO_OBJECT >
inline VIterator< TO_OBJECT >& VIterator< TO_OBJECT >::operator=( const VIterator& it ) {
    m_object = it.m_object;
    this->m_he = it.m_he;
    return *this;
}



template < typename TO_OBJECT >
inline VIterator< TO_OBJECT >& VIterator< TO_OBJECT >::operator++() {
    this->m_he = this->m_he->Prev()->Twin();
    return *this;
}



template < typename TO_OBJECT >
inline VIterator< TO_OBJECT >& VIterator< TO_OBJECT >::operator--() {
    this->m_he = this->m_he->Twin()->Next();
    return *this;
}



template < typename TO_OBJECT >
inline bool VIterator< TO_OBJECT >::operator==( const VIterator& it ) const {
    return ( ( m_object == it.m_object ) && ( this->m_he == it.m_he ) );
}


} // namespace Core
} // namespace Ra
