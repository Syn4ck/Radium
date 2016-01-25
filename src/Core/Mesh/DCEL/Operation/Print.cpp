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

    LOG( logDEBUG ) << "====== VERTEX INFO ======";
    LOG( logDEBUG ) << " Vertex index    : " << v->idx.getValue();
    LOG( logDEBUG ) << " Vertex position : " << "( " << p[0] << ", " << p[1] << ", " << p[2] << " )";
    LOG( logDEBUG ) << " Vertex normal   : " << "( " << n[0] << ", " << n[1] << ", " << n[2] << " )";
    LOG( logDEBUG ) << " Has HalfEdge ?  : " << ( ( v->HE() != nullptr ) ? "YES" : "NO" );
}

void print( const HalfEdge_ptr& he ) {
    LOG( logDEBUG ) << "====== HALFEDGE INFO ======";
    LOG( logDEBUG ) << " HalfEdge index : " << he->idx.getValue();
    LOG( logDEBUG ) << " Has Vertex ?   : " << ( ( he->V()    != nullptr ) ? "YES" : "NO" );
    LOG( logDEBUG ) << " Has Next ?     : " << ( ( he->Next() != nullptr ) ? "YES" : "NO" );
    LOG( logDEBUG ) << " Has Prev ?     : " << ( ( he->Prev() != nullptr ) ? "YES" : "NO" );
    LOG( logDEBUG ) << " Has Twin ?     : " << ( ( he->Twin() != nullptr ) ? "YES" : "NO" );
    LOG( logDEBUG ) << " Has FullEdge ? : " << ( ( he->FE()   != nullptr ) ? "YES" : "NO" );
    LOG( logDEBUG ) << " Has Face ?     : " << ( ( he->F()    != nullptr ) ? "YES" : "NO" );
}

void print( const FullEdge_ptr& fe ) {
    LOG( logDEBUG ) << "====== FULLEDGE INFO ======";
    LOG( logDEBUG ) << " FullEdge index : " << fe->idx.getValue();
    LOG( logDEBUG ) << " Has HalfEdge ? : " << ( ( fe->HE( 0 ) != nullptr ) ? "YES" : "NO" );
    LOG( logDEBUG ) << " Is corrupted ? : " << ( ( fe->HE( 1 ) == nullptr ) ? "YES" : "NO" );
}

void print( const Face_ptr& f ) {
    LOG( logDEBUG ) << "====== FACE INFO ======";
    LOG( logDEBUG ) << " Face index     : " << f->idx.getValue();
    LOG( logDEBUG ) << " Has HalfEdge ? : " << ( ( f->HE() != nullptr ) ? "YES" : "NO" );
}

void print( const Dcel_ptr& dcel ) {
    LOG( logDEBUG ) << "====== DCEL INFO ======";
    LOG( logDEBUG ) << " DCEL index    : " << dcel->idx.getValue();
    LOG( logDEBUG ) << " # of Vertex   : " << dcel->m_vertex.size();
    LOG( logDEBUG ) << " # of HalfEdge : " << dcel->m_halfedge.size();
    LOG( logDEBUG ) << " # of FullEdge : " << dcel->m_fulledge.size();
    LOG( logDEBUG ) << " # of Face     : " << dcel->m_face.size();
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
