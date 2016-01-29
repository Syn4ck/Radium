#include <Core/Mesh/DCEL/Iterator/Vertex/VHEIterator.hpp>

#include <Core/Mesh/DCEL/Vertex.hpp>
#include <Core/Mesh/DCEL/HalfEdge.hpp>

namespace Ra {
namespace Core {

/// CONSTRUCTOR
VHEIterator::VHEIterator( const Vertex_ptr& v ) : VIterator< HalfEdge_ptr >( v ) { }



/// DESTRUCTOR
VHEIterator::~VHEIterator() { }



/// LIST
inline HalfEdgeList VHEIterator::list() const {
    HalfEdgeList L;
    const HalfEdge_ptr& start = m_object->HE();
    HalfEdge_ptr        it    = start;
    if( it != nullptr ) {
        do {
            L.push_back( it );
            if( it->Prev() == nullptr ) break;
            it = it->Prev();
            if( it->Twin() == nullptr ) break;
            it = it->Twin();
        } while( ( it != nullptr ) && ( it != start ) );
    }
    return L;
}



/// OPERATOR
inline HalfEdge_ptr VHEIterator::operator->() const {
    return m_he;
}

inline HalfEdge_ptr VHEIterator::operator* () const {
    return m_he;
}

} // namespace Core
} // namespace Ra
