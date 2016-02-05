#include <Core/Mesh/DCEL/Iterator/Vertex/VVIterator.hpp>

#include <Core/Mesh/DCEL/Vertex.hpp>
#include <Core/Mesh/DCEL/HalfEdge.hpp>

namespace Ra {
namespace Core {

/// CONSTRUCTOR
VVIterator::VVIterator( const Vertex_ptr& v ) : VIterator< Vertex_ptr >( v ) { }



/// DESTRUCTOR
VVIterator::~VVIterator() { }



/// LIST
inline VertexList VVIterator::list() const {
    VertexList L;
    const HalfEdge_ptr& start = m_object->HE();
    HalfEdge_ptr        it    = start;
    if( it != nullptr ) {
        do {
            if( it->Next() == nullptr ) break;
            if( it->Next()->V() == nullptr ) break;
            L.push_back( it->Next()->V() );
            if( it->Prev() == nullptr ) break;
            it = it->Prev();
            if( it->Twin() == nullptr ) break;
            it = it->Twin();
        } while( ( it != nullptr ) && ( it != start ) );
    }
    return L;
}



/// OPERATOR
inline Vertex_ptr VVIterator::operator->() const {
    if( m_he == nullptr ) return nullptr;
    if( m_he->Next() == nullptr ) return nullptr;
    return m_he->Next()->V();
}

inline Vertex_ptr VVIterator::operator* () const {
    if( m_he == nullptr ) return nullptr;
    if( m_he->Next() == nullptr ) return nullptr;
    return m_he->Next()->V();
}

} // namespace Core
} // namespace Ra
