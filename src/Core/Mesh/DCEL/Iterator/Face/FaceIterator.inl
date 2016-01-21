#include <Core/Mesh/DCEL/Iterator/Face/FaceIterator.hpp>

#include <Core/Mesh/DCEL/HalfEdge.hpp>
#include <Core/Mesh/DCEL/Face.hpp>

namespace Ra {
namespace Core {

/*
/// CONSTRUCTOR
template < typename OBJECT >
FIterator< OBJECT >::FIterator( Face_ptr& f ) :
    Iterator< OBJECT >(),
    m_f( f ) {
    this->m_he = m_f->HE();
}






/// DESTRUCTOR
template < typename OBJECT >
FIterator< OBJECT >::~FIterator() { }






/// SIZE
template < typename OBJECT >
inline uint FIterator< OBJECT >::size() const {
    uint i = 0;
    HalfEdge_ptr it = m_f->HE();
    do {
        it = it->Next();
        ++i;
    } while( it != m_f->HE() );
    return i;
}






/// RESET
template < typename OBJECT >
inline void FIterator< OBJECT >::reset() {
    this->m_he = m_f->HE();
}






/// OPERATOR
template < typename OBJECT >
inline FIterator< OBJECT >& FIterator< OBJECT >::operator=( const FIterator& it ) {
    m_f = it.m_fs;
    this->m_he = it.m_he;
    return *this;
}



template < typename OBJECT >
inline FIterator< OBJECT >& FIterator< OBJECT >::operator++() {
    this->m_he = this->m_he->Next();
    return *this;
}



template < typename OBJECT >
inline FIterator< OBJECT >& FIterator< OBJECT >::operator--() {
    this->m_he = this->m_he->Prev();
    return *this;
}



template < typename OBJECT >
inline bool FIterator< OBJECT >::operator==( const FIterator& it ) const {
    return ( ( m_f == it.m_f ) && ( this->m_he == it.m_he ) );
}
*/



/// CONSTRUCTOR
template < typename TO_OBJECT >
FIterator< TO_OBJECT >::FIterator( const Face_ptr& f ) :
    DCEL_Iterator< Face_ptr, TO_OBJECT >( f ) { }






/// DESTRUCTOR
template < typename TO_OBJECT >
FIterator< TO_OBJECT >::~FIterator() { }






/// SIZE
template < typename TO_OBJECT >
inline uint FIterator< TO_OBJECT >::size() const {
    uint i = 0;
    HalfEdge_ptr it = this->m_object->HE();
    if( it != nullptr ) {
        do {
            if( it->Next() != nullptr ) {
                it = it->Next();
                ++i;
            } else {
                it = nullptr;
            }
        } while( ( it != this->m_object->HE() ) && ( it != nullptr ) );
    }
    return i;
}






/// OPERATOR
template < typename TO_OBJECT >
inline FIterator< TO_OBJECT >& FIterator< TO_OBJECT >::operator=( const FIterator& it ) {
    this->m_object = it.m_object;
    this->m_he = it.m_he;
    return *this;
}



template < typename TO_OBJECT >
inline FIterator< TO_OBJECT >& FIterator< TO_OBJECT >::operator++() {
    this->m_he = this->m_he->Next();
    return *this;
}



template < typename TO_OBJECT >
inline FIterator< TO_OBJECT >& FIterator< TO_OBJECT >::operator--() {
    this->m_he = this->m_he->Prev();
    return *this;
}



template < typename TO_OBJECT >
inline bool FIterator< TO_OBJECT >::operator==( const FIterator& it ) const {
    return ( ( this->m_object == it.m_object ) && ( this->m_he == it.m_he ) );
}



} // namespace Core
} // namespace Ra
