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
VIterator< TO_OBJECT >::VIterator( const Vertex_ptr& v ) :
    DCEL_Iterator< Vertex_ptr, TO_OBJECT >( v ) { }






/// DESTRUCTOR
template < typename TO_OBJECT >
VIterator< TO_OBJECT >::~VIterator() { }






/// SIZE
template < typename TO_OBJECT >
uint VIterator< TO_OBJECT >::size() const {
    uint i = 0;
    const HalfEdge_ptr& start = this->m_object->HE();
    HalfEdge_ptr        it    = start;
    if( it != nullptr ) {
        do {
            if( it->Prev() == nullptr ) break;
            it = it->Prev();
            if( it->Twin() == nullptr ) break;
            it = it->Twin();
            ++i;
        } while( ( it != nullptr ) && ( it != start ) );
    }
    return i;
}






/// OPERATOR
template < typename TO_OBJECT >
inline VIterator< TO_OBJECT >& VIterator< TO_OBJECT >::operator=( const VIterator& it ) {
    this->m_object = it.m_object;
    this->m_he = it.m_he;
    return *this;
}



template < typename TO_OBJECT >
VIterator< TO_OBJECT >& VIterator< TO_OBJECT >::operator++() {
    HalfEdge_ptr ptr = this->m_he;
    if( ptr == nullptr ) return *this;
    if( ptr->Prev() == nullptr ) return *this;
    ptr = ptr->Prev();
    if( ptr->Twin() == nullptr ) return *this;
    ptr = ptr->Twin();
    this->m_he = ptr;
    return *this;
}



template < typename TO_OBJECT >
VIterator< TO_OBJECT >& VIterator< TO_OBJECT >::operator--() {
    HalfEdge_ptr ptr = this->m_he;
    if( ptr == nullptr ) return *this;
    if( ptr->Twin() == nullptr ) return *this;
    ptr = ptr->Twin();
    if( ptr->Next() == nullptr ) return *this;
    ptr = ptr->Next();
    this->m_he = ptr;
    return *this;
}



template < typename TO_OBJECT >
inline bool VIterator< TO_OBJECT >::operator==( const VIterator& it ) const {
    return ( ( this->m_object == it.m_object ) && ( this->m_he == it.m_he ) );
}


} // namespace Core
} // namespace Ra
