#include <Core/Mesh/DCEL/Iterator/Vertex/VFEIterator.hpp>

#include <Core/Mesh/DCEL/Vertex.hpp>
#include <Core/Mesh/DCEL/HalfEdge.hpp>
#include <Core/Mesh/DCEL/FullEdge.hpp>

namespace Ra {
namespace Core {



/// CONSTRUCTOR
VFEIterator::VFEIterator( const Vertex_ptr& v ) : VIterator< FullEdge_ptr >( v ) { }



/// DESTRUCTOR
VFEIterator::~VFEIterator() { }



/// LIST
inline FullEdgeList VFEIterator::list() const {
    FullEdgeList L;
    const HalfEdge_ptr& start = m_object->HE();
    HalfEdge_ptr        it    = start;
    if( it != nullptr ) {
        do {
            if( it->FE() == nullptr ) break;
            L.push_back( it->FE() );
            if( it->Prev() == nullptr ) break;
            it = it->Prev();
            if( it->Twin() == nullptr ) break;
            it = it->Twin();
        } while( ( it != nullptr ) && ( it != start ) );
    }
    return L;
}



/// OPERATOR
inline FullEdge_ptr VFEIterator::operator->() const {
    if( m_he == nullptr ) return nullptr;
    return m_he->FE();
}

inline FullEdge_ptr VFEIterator::operator* () const {
    if( m_he == nullptr ) return nullptr;
    return m_he->FE();
}



} // namespace Core
} // namespace Ra
