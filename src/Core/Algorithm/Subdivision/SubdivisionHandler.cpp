#include <Core/Algorithm/Subdivision/SubdivisionHandler.hpp>

#include <Core/Geometry/Triangle/TriangleOperation.hpp>

#include <Core/Mesh/DCEL/Vertex.hpp>
#include <Core/Mesh/DCEL/HalfEdge.hpp>
#include <Core/Mesh/DCEL/FullEdge.hpp>
#include <Core/Mesh/DCEL/Face.hpp>
#include <Core/Mesh/DCEL/Dcel.hpp>

#include <Core/Mesh/DCEL/Iterator/Vertex/VHEIterator.hpp>

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

    if( isBorder( ptr ) ) {
        return true;
    }

    // In case of release, the previous one won't work
    if( !isValid( ptr ) ) {
        return false;
    }

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

    // Insertion
    m_dcel->insert( v[n] );
    m_dcel->insert( he[x] );
    m_dcel->insert( he[y] );
    m_dcel->insert( fe[1] );

    //CORE_ASSERT( m_dcel->m_vertex.compatible( v[n]->idx ), "WTF?" );
    //CORE_ASSERT( m_dcel->m_halfedge.compatible( he[x]->idx ), "WTF?" );
    //CORE_ASSERT( m_dcel->m_halfedge.compatible( he[y]->idx ), "WTF?" );
    //CORE_ASSERT( m_dcel->m_fulledge.compatible( fe[1]->idx ), "WTF?" );

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


    // Handle face splitting
    bool splitStatus = true;

    splitStatus &= splitFace( f[0] );
    //CORE_ASSERT( splitStatus, "splitFace on f[0] has failed." );

    splitStatus &= splitFace( f[1] );
    //CORE_ASSERT( splitStatus, "splitFace on f[1] has failed." );

    return splitStatus;
}



bool SubdivisionHandler::splitFace( const Face_ptr& ptr ) {
    // The face is a hole in the mesh
    if( ptr == nullptr ) return true;

    // It is a proper face MAYBE
    if( !isValid( ptr ) ) {
        return false;
    }

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

    // Insertion
    m_dcel->insert( he[x] );
    m_dcel->insert( he[y] );
    m_dcel->insert( fe    );
    m_dcel->insert( f[1]  );

    //CORE_ASSERT( m_dcel->m_halfedge.compatible( he[x]->idx ), "WTF?" );
    //CORE_ASSERT( m_dcel->m_halfedge.compatible( he[y]->idx ), "WTF?" );
    //CORE_ASSERT( m_dcel->m_fulledge.compatible( fe->idx ), "WTF?" );
    //CORE_ASSERT( m_dcel->m_face.compatible( f[1]->idx ), "WTF?" );

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

    return true;
}
#endif


#ifdef DEBUG_COLLAPSE
/// COLLAPSE
bool SubdivisionHandler::collapseFullEdge( const FullEdge_ptr& ptr ) {
    // Check the fulledge data
    check( ptr );

    if( isBorder( ptr ) ) {
        return true;
    }

    // In case of release, the previous one won't work
    if( !isValid( ptr ) ) {
        return false;
    }

    handleSpecialCase( ptr->HE( 0 ) );
    handleSpecialCase( ptr->HE( 1 ) );

    // Useful names
    const uint x = 4; // HalfEdge to delete
    const uint y = 5; // HalfEdge to delete

    // Declare data
    Vertex_ptr   v[2];
    HalfEdge_ptr he[6];
    FullEdge_ptr fe;
    Face_ptr     f[2];

    // Initialize data
    fe    = ptr;
    he[x] = fe->HE();
    he[y] = he[x]->Twin();
    he[0] = he[x]->Next();
    he[1] = he[x]->Prev();
    he[2] = he[y]->Next();
    he[3] = he[y]->Prev();
    v[0]  = he[x]->V();
    v[1]  = he[y]->V();
    f[0]  = he[x]->F();
    f[1]  = he[y]->F();

    // Check data
    if( !isValid( v[0] ) || !isValid( v[1] ) ) {
        return false;
    }

    if( v[0] == v[1] ) {
        return false;
    }

    for( uint i = 0; i < 6; ++i ) {
        if( !isValid( he[i] ) ) {
            return false;
        }
        for( uint j = ( i + 1 ); j < 6; ++j ) {
            if( he[i] == he[j] ) {
                //return false;
            }
        }
    }

    // Set data

    v[0]->setP( 0.5 * ( v[0]->P() + v[1]->P() ) );
    v[0]->setN( ( v[0]->N() + v[1]->N() ).normalized() );
    v[0]->setHE( he[0] );

    unbind( v[1]  );
    bind( v[0] );

    he[0]->setPrev( he[1] );
    he[1]->setNext( he[0] );
    he[2]->setPrev( he[3] );
    he[3]->setNext( he[2] );

    if( f[0] != nullptr ) f[0]->setHE( he[0] );
    if( f[1] != nullptr ) f[1]->setHE( he[2] );

    unbind( fe    );
    unbind( he[x] );
    unbind( he[y] );

    m_dcel->removeVertex  (  v[1]->idx );
    m_dcel->removeHalfEdge( he[x]->idx );
    m_dcel->removeHalfEdge( he[y]->idx );
    m_dcel->removeFullEdge(    fe->idx );

    delete v[1];
    delete he[x];
    delete he[y];
    delete fe;

    // Handle face collapsing
    bool collapseStatus = true;

    collapseStatus &= collapseFace( f[0] );
    //CORE_ASSERT( collapseStatus, "collapseFace on f[0] has failed." );

    collapseStatus &= collapseFace( f[1] );
    //CORE_ASSERT( collapseStatus, "collapseFace on f[1] has failed." );

    return collapseStatus;
}



bool SubdivisionHandler::collapseFace( const Face_ptr& ptr ) {
    // The face is a hole in the mesh
    if( ptr == nullptr ) {
        return true;
    }

    // It is a proper face MAYBE
    if( !isValid( ptr ) ) {
        return false;
    }

    // Useful names
    const uint x = 2; // HalfEdge to delete
    const uint y = 3; // HalfEdge to delete

    // Declare data
    Vertex_ptr   v[2];
    HalfEdge_ptr he[4];
    FullEdge_ptr fe[2];
    Face_ptr     f;

    // Initialize data
    he[x] = ptr->HE();
    he[y] = he[x]->Next();
    he[0] = he[x]->Twin();
    he[1] = he[y]->Twin();
    v[0]  = he[x]->V();
    v[1]  = he[y]->V();
    fe[0] = he[x]->FE();
    fe[1] = he[y]->FE();
    f     = ptr;

    // Check data
    if( !isValid( v[0] ) || !isValid( v[1] ) ) {
        return false;
    }

    if( v[0] == v[1] ) {
        return false;
    }

    if( !isValid( he[x] ) || !isValid( he[y] ) ) {
        return false;
    }

    if( !isValid( fe[1] ) ) {
        return false;
    }

    // Set data
    v[0]->setHE( he[1] );
    v[1]->setHE( he[0] );

    he[0]->setTwin( he[1] );
    he[1]->setTwin( he[0] );

    fe[0]->setHE( he[0] );
    bind( fe[0] );

    unbind( fe[1] );
    unbind( he[x] );
    unbind( he[y] );
    unbind( f );

    m_dcel->removeHalfEdge( he[x]->idx );
    m_dcel->removeHalfEdge( he[y]->idx );
    m_dcel->removeFullEdge( fe[1]->idx );
    m_dcel->removeFace( f->idx );

    delete he[x];
    delete he[y];
    delete fe[1];
    delete f;

    return true;
}



bool SubdivisionHandler::handleSpecialCase( const HalfEdge_ptr& ptr ) {
    if( ptr->F() == nullptr ) {
        return false;
    }

    // Declare data
    Vertex_ptr   v[3];
    HalfEdge_ptr he[3];

    bool status = false;
    do {
        // Initialize data
        he[0] = ptr;
        he[1] = he[0]->Next()->Twin()->Next();
        he[2] = he[0]->Prev()->Twin()->Prev();
        v[0] = he[0]->V();
        v[1] = he[1]->V();
        v[2] = he[2]->V();

        // Check
        if( ( status = ( he[1]->Twin()->V() == v[2] ) ) ) {
            // Process
            faceMerge( he[0], he[1], he[2] );
        }
    } while( status == true );

    return status;
}



void SubdivisionHandler::faceMerge( const HalfEdge_ptr& h0, const HalfEdge_ptr& h1, const HalfEdge_ptr& h2 ) {
    // Declare data
    Vertex_ptr   v[4];
    HalfEdge_ptr he[3][3];
    FullEdge_ptr fe[3];
    Face_ptr     f[3];

    // Initialize data
    he[0][0] = h0;
    he[0][1] = h0->Next();
    he[0][2] = h0->Prev();
    he[1][0] = h1;
    he[1][1] = h1->Next();
    he[1][2] = h1->Prev();
    he[2][0] = h2;
    he[2][1] = h2->Next();
    he[2][2] = h2->Prev();

    v[0] = he[0][0]->V();
    v[1] = he[1][0]->V();
    v[2] = he[2][0]->V();
    v[3] = he[0][2]->V();

    fe[0] = he[0][1]->FE();
    fe[1] = he[1][1]->FE();
    fe[2] = he[2][1]->FE();

    f[0] = he[0][0]->F();
    f[1] = he[1][0]->F();
    f[2] = he[2][0]->F();

    // Set data
    for( uint i = 0; i < 3; ++i ) {
        v[i]->setHE( he[i][0] );
        he[i][0]->setNext( he[( i + 1 ) % 3 ][0] );
        he[i][0]->setPrev( he[( i + 2 ) % 3 ][0] );
        he[i][0]->setF( f[0] );
    }
    f[0]->setHE( he[0][0] );

    // Remove and delete data
    m_dcel->removeVertex( v[3]->idx );
    delete v[3];
    for( uint i = 0; i < 3; ++i ) {
        for( uint j = 1; j < 3; ++j ) {
            m_dcel->removeHalfEdge( he[i][j]->idx );
            delete he[i][j];
        }
        m_dcel->removeFullEdge( fe[i]->idx );
        delete fe[i];
    }
    m_dcel->removeFace( f[1]->idx );
    m_dcel->removeFace( f[2]->idx );
    delete f[1];
    delete f[2];

}
#endif


#ifdef DEBUG_FLIP
/// FLIP

bool SubdivisionHandler::isFlippable( const FullEdge_ptr& fe ) {
    if( isBorder( fe ) ) {
        return false;
    }

    // Useful names
    const uint x = 4; // HalfEdge to flip
    const uint y = 5; // HalfEdge to flip

    // Declare data
    Vertex_ptr   v[4];
    HalfEdge_ptr he[6];

    // Initialize data
    he[x] = fe->HE( 0 );
    he[y] = fe->HE( 1 );
    he[0] = he[x]->Next();
    he[1] = he[x]->Prev();
    he[2] = he[y]->Next();
    he[3] = he[y]->Prev();
    v[0]  = he[0]->V();
    v[1]  = he[1]->V();
    v[2]  = he[2]->V();
    v[3]  = he[3]->V();

    const Vector3 p = v[2]->P();
    const Vector3 q = v[0]->P();
    const Vector3 r = v[1]->P();
    const Vector3 s = v[3]->P();

    const Vector3 pq = ( q - p ).normalized();
    const Vector3 pr = ( r - p ).normalized();
    const Vector3 ps = ( s - p ).normalized();
    const Vector3 qr = ( r - q ).normalized();
    const Vector3 qs = ( s - q ).normalized();
    const Vector3 rs = ( s - r ).normalized();

    /*
    const Scalar pqr = Vector::angle< Vector3 >( -pq,  qr );
    const Scalar pqs = Vector::angle< Vector3 >( -pq,  qs );
    const Scalar prq = Vector::angle< Vector3 >( -pr, -qr );
    const Scalar prs = Vector::angle< Vector3 >( -pr,  rs );
    const Scalar psq = Vector::angle< Vector3 >( -ps, -qs );
    const Scalar psr = Vector::angle< Vector3 >( -ps, -rs );
    const Scalar qpr = Vector::angle< Vector3 >(  pq,  pr );
    const Scalar qps = Vector::angle< Vector3 >(  pq,  ps );
    const Scalar qrs = Vector::angle< Vector3 >( -qr,  rs );
    const Scalar qsr = Vector::angle< Vector3 >( -qs, -rs );
    const Scalar rps = Vector::angle< Vector3 >(  pr,  ps );
    const Scalar rqs = Vector::angle< Vector3 >(  qr,  qs );

    if( !( ( prs > std::min( Math::PiDiv2, pqs ) ) &&
           ( psr > std::min( Math::PiDiv2, pqr ) ) ) ) {
        return false;
    }

    if( !( ( qrs > std::min( Math::PiDiv2, qps ) ) &&
           ( qsr > std::min( Math::PiDiv2, qpr ) ) ) ) {
        return false;
    }

    if( !( ( rps > std::max( qpr, qps ) ) &&
           ( rqs > std::max( pqr , pqs ) ) ) ) {
        return false;
    }

    if( !( ( prq > std::max( prs, qrs ) ) &&
           ( psq > std::max( psr, qsr ) ) ) ) {
        return false;
    }*/


    // Dihedral
    const Vector3 n0 = Geometry::triangleNormal( p, q, r );
    const Vector3 n1 = Geometry::triangleNormal( p, s, q );

    const Scalar angle = Vector::angle< Vector3 >( n0, n1 );
    if( angle > Math::PiDiv4 ) {
        return false;
    }

    // Convexity
    const Scalar a0 = Vector::angle< Vector3 >( pq, pr );
    const Scalar a1 = Vector::angle< Vector3 >( pq, ps );

    const Scalar b0 = Vector::angle< Vector3 >( -pq, qr );
    const Scalar b1 = Vector::angle< Vector3 >( -pq, qs );

    const Scalar alpha = a0 + a1;
    const Scalar beta  = b0 + b1;

    if( ( alpha >= Math::Pi ) || ( beta >= Math::Pi ) ) {
        return false;
    }

    return true;

}


bool SubdivisionHandler::flipFullEdge( const FullEdge_ptr& fulledge ) {
    // Check the fulledge data
    check( fulledge );

    // In case of release, the previous one won't work
    if( !isValid( fulledge ) ) {
        return false;
    }

    // In case of a border fulledge, do not flip
    if( !isFlippable( fulledge ) ) {
        return true;
    }

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
    he[x] = fe->HE( 0 );
    he[y] = fe->HE( 1 );
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

#ifdef CORE_DEBUG
    for( uint i = 0; i < 6; ++i ) {
        for( uint j = ( i + 1 ); j < 6; ++j ) {
            //CORE_ASSERT( ( he[i] != he[j] ), "Bad HalfEdge" );
        }
    }

    for( uint i = 0; i < 4; ++i ) {
        for( uint j = ( i + 1 ); j < 4; ++j ) {
            //CORE_ASSERT( ( v[i] != v[j] ), "Bad Vertex" );
        }
    }

    check( v[0], DCEL_CHECK_CONSISTENCY );
    check( v[1], DCEL_CHECK_CONSISTENCY );
    check( v[2], DCEL_CHECK_CONSISTENCY );
    check( v[3], DCEL_CHECK_CONSISTENCY );
#endif

    // Set data
    v[0]->setHE( he[0] );
    v[1]->setHE( he[1] );
    v[2]->setHE( he[2] );
    v[3]->setHE( he[3] );

    he[0]->setV( v[0] );
    he[0]->setNext( he[x] );
    he[0]->setPrev( he[3] );
    he[0]->setF( f[0] );

    he[1]->setV( v[1] );
    he[1]->setNext( he[2] );
    he[1]->setPrev( he[y] );
    he[1]->setF( f[1] );

    he[2]->setV( v[2] );
    he[2]->setNext( he[y] );
    he[2]->setPrev( he[1] );
    he[2]->setF( f[1] );

    he[3]->setV( v[3] );
    he[3]->setNext( he[0] );
    he[3]->setPrev( he[x] );
    he[3]->setF( f[0] );

    he[x]->setV( v[1] );
    he[x]->setNext( he[3] );
    he[x]->setPrev( he[0] );
    he[x]->setF( f[0] );

    he[y]->setV( v[3] );
    he[y]->setNext( he[1] );
    he[y]->setPrev( he[2] );
    he[y]->setF( f[1] );

    f[0]->setHE( he[x] );
    f[1]->setHE( he[y] );

    // Binding
    //bind( he[x] );
    //bind( he[y] );
    //bind( f[0] );
    //bind( f[1] );


#ifdef CORE_DEBUG
    for( uint i = 0; i < 6; ++i ) {
        for( uint j = ( i + 1 ); j < 6; ++j ) {
            //CORE_ASSERT( ( he[i] != he[j] ), "Bad HalfEdge" );
        }
    }

    for( uint i = 0; i < 4; ++i ) {
        for( uint j = ( i + 1 ); j < 4; ++j ) {
            //CORE_ASSERT( ( v[i] != v[j] ), "Bad Vertex" );
        }
    }

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
