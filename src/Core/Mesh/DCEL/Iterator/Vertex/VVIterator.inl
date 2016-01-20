#include <Core/Mesh/DCEL/Iterator/Vertex/VVIterator.hpp>

#include <Core/Mesh/DCEL/Vertex.hpp>
#include <Core/Mesh/DCEL/HalfEdge.hpp>

namespace Ra {
namespace Core {

/// CONSTRUCTOR
VVIterator::VVIterator( Vertex_ptr& v ) : VIterator< Vertex_ptr >( v ) { }



/// DESTRUCTOR
VVIterator::~VVIterator() { }



/// LIST
inline VertexList VVIterator::list() const {
    VertexList L;
    HalfEdge_ptr it = m_object->HE();
    do {
        L.push_back( it->Next()->V() );
        it = it->Prev()->Twin();
    } while( it != m_object->HE() );
    return L;
}



/// OPERATOR
inline Vertex_ptr VVIterator::operator->() const {
    return m_he->Next()->V();
}



} // namespace Core
} // namespace Ra
