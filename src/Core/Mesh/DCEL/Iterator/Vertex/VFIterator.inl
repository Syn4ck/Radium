#include <Core/Mesh/DCEL/Iterator/Vertex/VFIterator.hpp>

#include <Core/Mesh/DCEL/Vertex.hpp>
#include <Core/Mesh/DCEL/HalfEdge.hpp>
#include <Core/Mesh/DCEL/Face.hpp>

namespace Ra {
namespace Core {



/// CONSTRUCTOR
VFIterator::VFIterator( const Vertex_ptr& v ) : VIterator< Face_ptr >( v ) { }



/// DESTRUCTOR
VFIterator::~VFIterator() { }



/// LIST
inline FaceList VFIterator::list() const {
    FaceList L;
    HalfEdge_ptr it = m_object->HE();
    do {
        L.push_back( it->F() );
        it = it->Prev()->Twin();
    } while( it != m_object->HE() );
    return L;
}



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
            if( it->F() != nullptr ) {
                ++i;
            }
        } while( ( it != nullptr ) && ( it != start ) );
    }
    return i;
}



/// OPERATOR
inline Face_ptr VFIterator::operator->() const {
    return m_he->F();
}

inline Face_ptr VFIterator::operator* () const {
    return m_he->F();
}



VFIterator& VFIterator::operator++() {
    HalfEdge_ptr ptr = this->m_he;
    if( ptr == nullptr ) return *this;
    do {
        if( ptr->Prev() == nullptr ) return *this;
        ptr = ptr->Prev();
        if( ptr->Twin() == nullptr ) return *this;
        ptr = ptr->Twin();
    } while( ( ptr != this->m_he ) && ( ptr->F() == nullptr ) );
    this->m_he = ptr;
    return *this;
}



VFIterator& VFIterator::operator--() {
    HalfEdge_ptr ptr = this->m_he;
    if( ptr == nullptr ) return *this;
    do {
        if( ptr->Twin() == nullptr ) return *this;
        ptr = ptr->Twin();
        if( ptr->Next() == nullptr ) return *this;
        ptr = ptr->Next();
    } while( ( ptr != this->m_he ) && ( ptr->F() == nullptr ) );
    this->m_he = ptr;
    return *this;
}



} // namespace Core
} // namespace Ra
