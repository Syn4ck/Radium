#include <Core/Mesh/DCEL/Dcel.hpp>

#include <Core/Mesh/DCEL/Vertex.hpp>
#include <Core/Mesh/DCEL/HalfEdge.hpp>
#include <Core/Mesh/DCEL/FullEdge.hpp>
#include <Core/Mesh/DCEL/Face.hpp>

namespace Ra {
namespace Core {



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
inline void Dcel::insert( const Vertex_ptr& v ) {
    CORE_ASSERT( ( v != nullptr ), "Nullptr received." );
    if( !m_vertex.insert( v, v->idx ) ) {
        CORE_ASSERT( false, "Insertion failed." );
    }
}



inline void Dcel::insert( const HalfEdge_ptr& he ) {
    CORE_ASSERT( ( he != nullptr ), "Nullptr received." );
    if( !m_halfedge.insert( he, he->idx ) ) {
        CORE_ASSERT( false, "Insertion failed." );
    }
}



inline void Dcel::insert( const FullEdge_ptr& fe ) {
    CORE_ASSERT( ( fe != nullptr ), "Nullptr received." );
    if( !m_fulledge.insert( fe, fe->idx ) ) {
        CORE_ASSERT( false, "Insertion failed." );
    }
}



inline void Dcel::insert( const Face_ptr& f ) {
    CORE_ASSERT( ( f != nullptr ), "Nullptr received." );
    if( !m_face.insert( f, f->idx ) ) {
        CORE_ASSERT( false, "Insertion failed." );
    }
}



/// REMOVE
inline void Dcel::removeVertex( const Index& index ) {
    Vertex_ptr v;
    if( m_vertex.remove( index, v ) ) {
        v->idx.setInvalid();
    } else {
        CORE_ASSERT( false, "Removal failed." );
    }
}



inline void Dcel::removeHalfEdge( const Index& index ) {
    HalfEdge_ptr he;
    if( m_halfedge.remove( index, he ) ) {
        he->idx.setInvalid();
    } else {
        CORE_ASSERT( false, "Removal failed." );
    }
}



inline void Dcel::removeFullEdge( const Index& index ) {
    FullEdge_ptr fe;
    if( m_fulledge.remove( index, fe ) ) {
        fe->idx.setInvalid();
    } else {
        CORE_ASSERT( false, "Removal failed." );
    }
}



inline void Dcel::removeFace( const Index& index ) {
    Face_ptr f;
    if( m_face.remove( index, f ) ) {
        f->idx.setInvalid();
    } else {
        CORE_ASSERT( false, "Removal failed." );
    }
}



} // namespace Core
} // namespace Ra

