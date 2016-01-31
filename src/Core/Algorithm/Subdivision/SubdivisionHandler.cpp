#include <Core/Algorithm/Subdivision/SubdivisionHandler.hpp>

#include <Core/Mesh/DCEL/Vertex.hpp>
#include <Core/Mesh/DCEL/HalfEdge.hpp>
#include <Core/Mesh/DCEL/FullEdge.hpp>
#include <Core/Mesh/DCEL/Face.hpp>
#include <Core/Mesh/DCEL/Dcel.hpp>

#include <Core/Mesh/DCEL/Operation/Valid.hpp>
#include <Core/Mesh/DCEL/Operation/Check.hpp>
#include <Core/Mesh/DCEL/Operation/Bind.hpp>
#include <Core/Mesh/DCEL/Operation/Border.hpp>

namespace Ra {
namespace Core {

/// CONSTRUCTOR
SubdivisionHandler::SubdivisionHandler( const Dcel_ptr& dcel ) :
    m_dcel( dcel ) { }



/// DESTRUCTOR
SubdivisionHandler::~SubdivisionHandler() { }



#ifdef DEBUG_SPLIT
/// SPLIT
bool SubdivisionHandler::splitFullEdge( const FullEdge_ptr& ptr ) {
    // Check the fulledge data
    check( ptr );

    // In case of release, the previous one won't work
#ifndef CORE_DEBUG
    if( !isValid( ptr ) ) return false;
#endif

    // Useful names
    const uint n = 2; // new Vertex
    const uint x = 2; // new HalfEdge
    const uint y = 3; // new HalfEdge

    // Declare data
    Vertex_ptr   v[3];
    HalfEdge_ptr he[4];
    FullEdge_ptr fe[2];
    Face_ptr     f[2];

    // Initialize data
    he[0] = ptr->HE();
    he[1] = he[0]->Twin();
    he[x] = new HalfEdge();
    he[y] = new HalfEdge();
    v[0]  = he[0]->V();
    v[1]  = he[1]->V();
    v[n]  = new Vertex();
    fe[0] = ptr;
    fe[1] = new FullEdge( he[x] );
    f[0]  = he[0]->F();
    f[1]  = he[1]->F();

    // Set data
    v[n]->setP( 0.5 * ( v[0]->P() + v[1]->P() ) );
    v[n]->setN( ( v[0]->N() + v[1]->N() ).normalized() );
    v[n]->setHE( he[x] );

    he[x]->setV( v[n] );
    he[x]->setNext( he[0]->Next() );
    he[x]->setPrev( he[0] );
    he[x]->setTwin( he[y] );
    he[x]->setFE( fe[1] );
    he[x]->setF( f[0] );

    he[y]->setV( v[1] );
    he[y]->setNext( he[1] );
    he[y]->setPrev( he[1]->Prev() );
    he[y]->setTwin( he[x] );
    he[y]->setFE( fe[1] );
    he[y]->setF( f[1] );

    // Binding
    bind( he[x] );
    bind( he[y] );
    bind( v[n] );

    // Set the starting halfedge of the twin face
    if( f[1] != nullptr ) f[1]->setHE( he[1] );

    // Insertion
    m_dcel->insert( v[n] );
    m_dcel->insert( he[x] );
    m_dcel->insert( he[y] );
    m_dcel->insert( fe[1] );

    // Handle face splitting
    bool splitStatus = true;

    splitStatus &= splitFace( f[0] );
    CORE_ASSERT( splitStatus, "splitFace on f[0] has failed." );

    splitStatus &= splitFace( f[1] );
    CORE_ASSERT( splitStatus, "splitFace on f[1] has failed." );

    return splitStatus;
}



bool SubdivisionHandler::splitFace( const Face_ptr& ptr ) {
    // The face is a hole in the mesh
    if( ptr == nullptr ) return true;

    // It is a proper face MAYBE
#ifndef CORE_DEBUG
    if( !isValid( ptr ) ) return false;
#endif

    // Useful names
    const uint n = 3; // nth HalfEdge of the face
    const uint x = 4; // new HalfEdge
    const uint y = 5; // new HalfEdge

    // Declare data
    Vertex_ptr   v[4];
    HalfEdge_ptr he[6];
    FullEdge_ptr fe;
    Face_ptr     f[2];

    // Initialize data
    he[0] = ptr->HE();
    he[1] = he[0]->Next();
    he[2] = he[1]->Next();
    he[n] = he[0]->Prev();
    he[x] = new HalfEdge();
    he[y] = new HalfEdge();
    v[0]  = he[0]->V();
    v[1]  = he[1]->V();
    v[2]  = he[2]->V();
    v[3]  = he[n]->V();
    fe    = new FullEdge( he[x] );
    f[0]  = ptr;
    f[1]  = new Face( he[y] );

    // Set data
    he[x]->setV( v[2] );
    he[x]->setNext( he[0] );
    he[x]->setPrev( he[1] );
    he[x]->setTwin( he[y] );
    he[x]->setFE( fe );
    he[x]->setF( f[0] );

    he[y]->setV( v[0] );
    he[y]->setNext( he[2] );
    he[y]->setPrev( he[n] );
    he[y]->setTwin( he[x] );
    he[y]->setFE( fe );
    he[y]->setF( f[1] );

    // Binding
    bind( he[x] );
    bind( he[y] );
    bind( f[0] );
    bind( f[1] );

    // Insertion
    m_dcel->insert( he[x] );
    m_dcel->insert( he[y] );
    m_dcel->insert( fe );
    m_dcel->insert( f[1] );

    return true;
}
#endif


#ifdef DEBUG_COLLAPSE
/// COLLAPSE
bool SubdivisionHandler::collapseFullEdge( const FullEdge_ptr& ptr ) {
    // Check the fulledge data
    check( ptr );

    // In case of release, the previous one won't work
#ifndef CORE_DEBUG
    if( !isValid( ptr ) ) return false;
#endif

    // Useful names
    const uint x = 4; // HalfEdge to delete
    const uint y = 5; // HalfEdge to delete

    // Declare data
    Vertex_ptr   v[2];
    HalfEdge_ptr he[6];
    FullEdge_ptr fe;
    Face_ptr     f[2];

    // Initialize data
    fe = ptr;
    he[x] = fe->HE();
    he[y] = he[x]->Twin();
    he[0] = he[x]->Prev();
    he[1] = he[x]->Next();
    he[2] = he[y]->Prev();
    he[3] = he[y]->Next();
    v[0] = he[x]->V();
    v[1] = he[y]->V();
    f[0] = he[x]->F();
    f[1] = he[y]->F();

    // Set data
    v[0]->setP( 0.5 * ( v[0]->P() + v[1]->P() ) );
    v[0]->setN( ( v[0]->N() + v[1]->N() ).normalized() );

    // Unbinding
    unbind( fe );
    unbind( he[x] );
    unbind( he[y] );
    unbind( v[1] );

    // Set data
    he[1]->setPrev( he[0] );
    he[3]->setPrev( he[2] );

    // Binding
    bind( he[1] );
    bind( he[3] );
    bind( v[0] );

    // Removal
    m_dcel->removeVertex( v[1]->idx );
    m_dcel->removeHalfEdge( he[x]->idx );
    m_dcel->removeHalfEdge( he[y]->idx );
    m_dcel->removeFullEdge( fe->idx );

    // Deletion
    delete v[1];
    delete he[x];
    delete he[y];
    delete fe;

    // Handle face collapsing
    collapseFace( f[0] );
    collapseFace( f[1] );

    return true;
}



bool SubdivisionHandler::collapseFace( const Face_ptr& ptr ) {
    // The face is a hole in the mesh
    if( ptr == nullptr ) return true;

    // It is a proper face MAYBE
#ifndef CORE_DEBUG
    if( !isValid( ptr ) ) return false;
#endif

    // Useful names
    const uint x = 0; // HalfEdge to delete
    const uint y = 2; // HalfEdge to delete

    // Declare data
    Vertex_ptr   v[2];
    HalfEdge_ptr he[4];
    FullEdge_ptr fe[2];
    Face_ptr     f;

    // Initialize data
    he[x] = ptr->HE();
    he[1] = he[x]->Twin();
    he[y] = he[x]->Next();
    he[3] = he[y]->Twin();
    v[0]  = he[x]->V();
    v[1]  = he[y]->V();
    fe[0] = he[x]->FE();
    fe[1] = he[y]->FE();
    f     = ptr;

    // Set data
    he[1]->setTwin( he[3] );
    he[3]->setTwin( he[1] );
    he[3]->setFE( fe[0] );

    // Binding
    bind( he[1] );
    bind( he[3] );

    // Removal
    m_dcel->removeHalfEdge( he[x]->idx );
    m_dcel->removeHalfEdge( he[y]->idx );
    m_dcel->removeFullEdge( fe[1]->idx );
    m_dcel->removeFace( f->idx );

    // Deletion
    delete he[x];
    delete he[y];
    delete fe[1];
    delete f;

    return true;
}
#endif


#ifdef DEBUG_FLIP
/// FLIP
bool SubdivisionHandler::flipFullEdge( const FullEdge_ptr& fulledge ) {
    // Check the fulledge data
    check( fulledge );

    // In case of release, the previous one won't work
#ifndef CORE_DEBUG
    if( !isValid( fulledge ) ) return false;
#endif

    // In case of a border fulledge, do not flip
    if( isBorder( fulledge ) ) return true;

    // Useful names
    const uint x = 4; // HalfEdge to flip
    const uint y = 5; // HalfEdge to flip

    // Declare data
    Vertex_ptr   v[4];
    HalfEdge_ptr he[6];
    FullEdge_ptr fe;
    Face_ptr     f[2];

    // Initialize data
    fe   = fulledge;
    he[x] = fe->HE();
    he[y] = he[x]->Twin();
    he[0] = he[x]->Next();
    he[1] = he[x]->Prev();
    he[2] = he[y]->Next();
    he[3] = he[y]->Prev();
    v[0]  = he[0]->V();
    v[1]  = he[1]->V();
    v[2]  = he[2]->V();
    v[3]  = he[3]->V();
    f[0]  = he[x]->F();
    f[1]  = he[y]->F();

    // Set data
    v[0]->setHE( he[0] );
    v[1]->setHE( he[1] );
    v[2]->setHE( he[2] );
    v[3]->setHE( he[3] );
    he[0]->setNext( he[x] );
    he[0]->setPrev( he[3] );
    he[1]->setNext( he[2] );
    he[1]->setPrev( he[y] );
    he[2]->setNext( he[y] );
    he[2]->setPrev( he[1] );
    he[3]->setNext( he[0] );
    he[3]->setPrev( he[x] );
    he[x]->setV( v[1] );
    he[x]->setNext( he[3] );
    he[x]->setPrev( he[0] );
    he[y]->setV( v[3] );
    he[y]->setNext( he[1] );
    he[y]->setPrev( he[2] );

    // Binding
    bind( he[x] );
    bind( he[y] );
    bind( f[0] );
    bind( f[1] );

#ifdef CORE_DEBUG
    check( v[0], DCEL_CHECK_CONSISTENCY );
    check( v[1], DCEL_CHECK_CONSISTENCY );
    check( v[2], DCEL_CHECK_CONSISTENCY );
    check( v[3], DCEL_CHECK_CONSISTENCY );
#endif

    return true;
}
#endif



} // namespace Core
} // namespace Ra
