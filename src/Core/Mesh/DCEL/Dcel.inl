#include <Core/Mesh/DCEL/Dcel.hpp>

namespace Ra {
namespace Core {



/// CLEAR
inline void Dcel::clear() {
    m_vertex.clear();
    m_halfedge.clear();
    m_fulledge.clear();
    m_face.clear();
}



/// QUERY
inline bool Dcel::empty() const {
    return ( m_vertex.empty()   &&
             m_halfedge.empty() &&
             m_fulledge.empty() &&
             m_face.empty() );
}



inline bool Dcel::compact() const {
    return ( m_vertex.compact()   &&
             m_halfedge.compact() &&
             m_fulledge.compact() &&
             m_face.compact() );
}


/// INSERT
inline void insert( const Vertex_ptr& v ) {
#ifdef CORE_DEBUG
    CORE_ASSERT( ( m_vertex.insert( v, v->idx ) ), "Insertion failed." );
#else
    m_vertex.insert( v, v->idx );
#endif
}



inline void insert( const HalfEdge_ptr& he ) {
#ifdef CORE_DEBUG
    CORE_ASSERT( ( m_halfedge.insert( he, he->idx ) ), "Insertion failed." );
#else
    m_halfedge.insert( he, he->idx );
#endif
}



inline void insert( const FullEdge_ptr& fe ) {
#ifdef CORE_DEBUG
    CORE_ASSERT( ( m_fulledge.insert( fe, fe->idx ) ), "Insertion failed." );
#else
    m_fulledge.insert( fe, fe->idx );
#endif
}



inline void insert( const Face_ptr& f ) {
#ifdef CORE_DEBUG
    CORE_ASSERT( ( m_face.insert( f, f->idx ) ), "Insertion failed." );
#else
    m_face.insert( f, f->idx );
#endif
}



/// REMOVE
inline void removeVertex( const Index& index ) {
#ifdef CORE_DEBUG
    CORE_ASSERT( ( m_vertex.remove( index ) ), "Removal failed." );
#else
    m_vertex.remove( index );
#endif
}



inline void removeHalfEdge( const Index& index ) {
#ifdef CORE_DEBUG
    CORE_ASSERT( ( m_halfedge.remove( index ) ), "Removal failed." );
#else
    m_halfedge.remove( index );
#endif
}



inline void removeFullEdge( const Index& index ) {
#ifdef CORE_DEBUG
    CORE_ASSERT( ( m_fulledge.remove( index ) ), "Removal failed." );
#else
    m_fulledge.remove( index );
#endif
}



inline void removeFace( const Index& index ) {
#ifdef CORE_DEBUG
    CORE_ASSERT( ( m_face.remove( index ) ), "Removal failed." );
#else
    m_face.remove( index );
#endif
}



} // namespace Core
} // namespace Ra

