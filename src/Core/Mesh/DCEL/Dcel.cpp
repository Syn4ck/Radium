#include <Core/Mesh/DCEL/Dcel.hpp>

#include <map>

#include <Core/Mesh/DCEL/Vertex.hpp>
#include <Core/Mesh/DCEL/HalfEdge.hpp>
#include <Core/Mesh/DCEL/FullEdge.hpp>
#include <Core/Mesh/DCEL/Face.hpp>

namespace Ra {
namespace Core {



/// CONSTRUCTOR
Dcel::Dcel( const Index& index ) :
    IndexedObject( index ),
    m_vertex(),
    m_halfedge(),
    m_fulledge(),
    m_face() { }



Dcel::Dcel( const Dcel& dcel ) :
    IndexedObject( dcel.idx ),
    m_vertex(),
    m_halfedge(),
    m_fulledge(),
    m_face() {

    // Mapping between indices of the two DCELs
    std::map< Index, Index > v_table;
    std::map< Index, Index > he_table;
    std::map< Index, Index > f_table;

    // Upload the vertex data, but the halfedge pointer
    for( uint i = 0; i < dcel.m_vertex.size(); ++i ) {
        Vertex_ptr dcel_v = dcel.m_vertex.at( i );
        Vertex_ptr v      = new Vertex( dcel_v->P(), dcel_v->N() );
        insert( v );
        v_table[dcel_v->idx] = v->idx;
    }

    // Upload the halfedge data, but the halfedge pointers and the face pointer
    for( uint i = 0; i < dcel.m_halfedge.size(); ++i ) {
        HalfEdge_ptr dcel_he = dcel.m_halfedge.at( i );
        HalfEdge_ptr he      = new HalfEdge( m_vertex[v_table[dcel_he->V()->idx]] );
        insert( he );
        he_table[dcel_he->idx] = he->idx;
    }

    // Upload the fulledge data
    for( uint i = 0; i < dcel.m_fulledge.size(); ++i ) {
        FullEdge_ptr dcel_fe = dcel.m_fulledge.at( i );
        FullEdge_ptr fe      = new FullEdge( m_halfedge[he_table[dcel_fe->HE( 0 )->idx]] );
        insert( fe );
        fe->HE( 0 )->Twin()->setFE( fe );
    }

    // Upload the face data
    for( uint i = 0; i < dcel.m_face.size(); ++i ) {
        Face_ptr dcel_f = dcel.m_face.at( i );
        Face_ptr f      = new Face( m_halfedge[he_table[dcel_f->HE()->idx]] );
        insert( f );
        f_table[dcel_f->idx] = f->idx;
    }

    // Upload the remaining data, belonging to halfedges and vertices
    for( uint i = 0; i < m_halfedge.size(); ++i ) {
        HalfEdge_ptr dcel_he = dcel.m_halfedge.at( i );
        HalfEdge_ptr he      = m_halfedge[i];
        he->setNext( m_halfedge[he_table[dcel_he->Next()->idx]] );
        he->setPrev( m_halfedge[he_table[dcel_he->Prev()->idx]] );
        he->setTwin( m_halfedge[he_table[dcel_he->Twin()->idx]] );
        he->setF( m_face[f_table[dcel_he->F()->idx]] );
        he->V()->setHE( m_halfedge[he_table[dcel_he->V()->HE()->idx]] );
    }
}



/// DESTRUCTOR
Dcel::~Dcel() {
    for( uint i = 0; i < m_vertex.size(); ++i ) {
        delete m_vertex[i];
    }
    for( uint i = 0; i < m_halfedge.size(); ++i ) {
        delete m_halfedge[i];
    }
    for( uint i = 0; i < m_fulledge.size(); ++i ) {
        delete m_fulledge[i];
    }
    for( uint i = 0; i < m_face.size(); ++i ) {
        delete m_face[i];
    }
}



} // namespace Core
} // namespace Ra

