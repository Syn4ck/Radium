#include <Core/Mesh/DCEL/Operation/Print.hpp>

#include <Core/Math/LinearAlgebra.hpp>

#include <Core/Mesh/DCEL/Vertex.hpp>
#include <Core/Mesh/DCEL/HalfEdge.hpp>
#include <Core/Mesh/DCEL/FullEdge.hpp>
#include <Core/Mesh/DCEL/Face.hpp>
#include <Core/Mesh/DCEL/Dcel.hpp>

#include <Core/Log/Log.hpp>


namespace Ra {
namespace Core {

void print( const Vertex_ptr& v ) {
    const Vector3 p = v->P();
    const Vector3 n = v->N();
    const bool halfedge = ( v->HE() != nullptr );

    LOG( logDEBUG ) << "====== VERTEX INFO ======";
    LOG( logDEBUG ) << " Vertex index    : " << v->idx.getValue();
    LOG( logDEBUG ) << " Vertex position : " << "( " << p[0] << ", " << p[1] << ", " << p[2] << " )";
    LOG( logDEBUG ) << " Vertex normal   : " << "( " << n[0] << ", " << n[1] << ", " << n[2] << " )";
    LOG( logDEBUG ) << " HalfEdge        : " << ( halfedge ? std::to_string( v->HE()->idx.getValue() ) : "NO" );
}

void print( const HalfEdge_ptr& he ) {
    const bool vertex   = ( he->V()    != nullptr );
    const bool next     = ( he->Next() != nullptr );
    const bool prev     = ( he->Prev() != nullptr );
    const bool twin     = ( he->Twin() != nullptr );
    const bool fulledge = ( he->FE()   != nullptr );
    const bool face     = ( he->F()    != nullptr );

    LOG( logDEBUG ) << "====== HALFEDGE INFO ======";
    LOG( logDEBUG ) << " HalfEdge index : " << he->idx.getValue();
    LOG( logDEBUG ) << " Vertex         : " << ( vertex   ? std::to_string( he->V()->idx.getValue() )    : "NO" );
    LOG( logDEBUG ) << " Next           : " << ( next     ? std::to_string( he->Next()->idx.getValue() ) : "NO" );
    LOG( logDEBUG ) << " Prev           : " << ( prev     ? std::to_string( he->Prev()->idx.getValue() ) : "NO" );
    LOG( logDEBUG ) << " Twin           : " << ( twin     ? std::to_string( he->Twin()->idx.getValue() ) : "NO" );
    LOG( logDEBUG ) << " FullEdge       : " << ( fulledge ? std::to_string( he->FE()->idx.getValue() )   : "NO" );
    LOG( logDEBUG ) << " Face           : " << ( face     ? std::to_string( he->F()->idx.getValue() )    : "NO" );
}

void print( const FullEdge_ptr& fe ) {
    const bool halfedge0 = ( fe->HE( 0 ) != nullptr );
    const bool halfedge1 = ( fe->HE( 1 ) != nullptr );
    const bool corrupted = !( halfedge0 && halfedge1 );

    LOG( logDEBUG ) << "====== FULLEDGE INFO ======";
    LOG( logDEBUG ) << " FullEdge index : " << fe->idx.getValue();
    LOG( logDEBUG ) << " HalfEdge #1    : " << ( halfedge0 ? std::to_string( fe->HE( 0 )->idx.getValue() ) : "NO" );
    LOG( logDEBUG ) << " HalfEdge #2    : " << ( halfedge1 ? std::to_string( fe->HE( 1 )->idx.getValue() ) : "NO" );
    LOG( logDEBUG ) << " Is corrupted ? : " << ( corrupted ? "YES"                       : "NO" );
}

void print( const Face_ptr& f ) {
    const bool halfedge = ( f->HE() != nullptr );

    LOG( logDEBUG ) << "====== FACE INFO ======";
    LOG( logDEBUG ) << " Face index : " << f->idx.getValue();
    LOG( logDEBUG ) << " HalfEdge   : " << ( halfedge ? std::to_string( f->HE()->idx.getValue() ) : "NO" );
}

void print( const Dcel_ptr& dcel, const bool recursive ) {
    LOG( logDEBUG ) << "====== DCEL INFO ======";
    LOG( logDEBUG ) << " DCEL index    : " << dcel->idx.getValue();
    LOG( logDEBUG ) << " # of Vertex   : " << dcel->m_vertex.size();
    LOG( logDEBUG ) << " # of HalfEdge : " << dcel->m_halfedge.size();
    LOG( logDEBUG ) << " # of FullEdge : " << dcel->m_fulledge.size();
    LOG( logDEBUG ) << " # of Face     : " << dcel->m_face.size();

    if( recursive ) {
        /// VERTEX
        const uint v_size = dcel->m_vertex.size();
        for( uint i = 0; i < v_size; ++i ) {
            print( dcel->m_vertex.at( i ) );
        }

        /// HALFEDGE
        const uint he_size = dcel->m_halfedge.size();
        for( uint i = 0; i < he_size; ++i ) {
            print( dcel->m_halfedge.at( i ) );
        }

        /// FULLEDGE
        const uint fe_size = dcel->m_fulledge.size();
        for( uint i = 0; i < fe_size; ++i ) {
            print( dcel->m_fulledge.at( i ) );
        }

        /// FACE
        const uint f_size = dcel->m_face.size();
        for( uint i = 0; i < f_size; ++i ) {
            print( dcel->m_face.at( i ) );
        }
    }
}


#if 0
std::string to_string( const Vertex_ptr&   v    ) { }
std::string to_string( const HalfEdge_ptr& he   ) { }
std::string to_string( const FullEdge_ptr& fe   ) { }
std::string to_string( const Face_ptr&     f    ) { }
std::string to_string( const Dcel_ptr&     dcel ) { }
#endif

} // namespace Core
} // namespace Ra
