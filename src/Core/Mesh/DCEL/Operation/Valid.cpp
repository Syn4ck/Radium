#include <Core/Mesh/DCEL/Operation/Valid.hpp>

#include <Core/Mesh/DCEL/Vertex.hpp>
#include <Core/Mesh/DCEL/HalfEdge.hpp>
#include <Core/Mesh/DCEL/FullEdge.hpp>
#include <Core/Mesh/DCEL/Face.hpp>
#include <Core/Mesh/DCEL/Dcel.hpp>

namespace Ra {
namespace Core {

bool isValid( const Vertex_ptr& v ) {
    return ( ( v->idx.isValid()   ) &&
             ( v->HE() != nullptr ) );
}

bool isValid( const HalfEdge_ptr& he ) {
    return ( ( he->idx.isValid()     ) &&
             ( he->V()    != nullptr ) &&
             ( he->Next() != nullptr ) &&
             ( he->Prev() != nullptr ) &&
             ( he->Twin() != nullptr ) &&
             ( he->FE()   != nullptr ) );
}

bool isValid( const FullEdge_ptr& fe ) {
    return ( ( fe->idx.isValid()      ) &&
             ( fe->HE( 0 ) != nullptr ) &&
             ( fe->HE( 1 ) != nullptr ) );
}

bool isValid( const Face_ptr& f ) {
    return ( ( f->idx.isValid()   ) &&
             ( f->HE() != nullptr ) );
}

bool isValid( const Dcel_ptr& dcel ) {
    /// VERTEX
    const uint v_size = dcel->m_vertex.size();
    for( uint i = 0; i < v_size; ++i ) {
        Vertex_ptr v = dcel->m_vertex.at( i );
        if( !isValid( v ) ) return false;
    }

    /// HALFEDGE
    const uint he_size = dcel->m_halfedge.size();
    for( uint i = 0; i < he_size; ++i ) {
        HalfEdge_ptr he = dcel->m_halfedge.at( i );
        if( !isValid( he ) ) return false;
    }

    /// FULLEDGE
    const uint fe_size = dcel->m_fulledge.size();
    for( uint i = 0; i < fe_size; ++i ) {
        FullEdge_ptr fe = dcel->m_fulledge.at( i );
        if( !isValid( fe ) ) return false;
    }

    /// FACE
    const uint f_size = dcel->m_face.size();
    for( uint i = 0; i < f_size; ++i ) {
        Face_ptr f = dcel->m_face.at( i );
        if( !isValid( f ) ) return false;
    }
    return true;
}

} // namespace Core
} // namespace Ra
