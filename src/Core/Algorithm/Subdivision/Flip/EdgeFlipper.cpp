#include <Core/Algorithm/Subdivision/Flip/EdgeFlipper.hpp>

#include <Core/Geometry/Triangle/TriangleOperation.hpp>

#include <Core/Mesh/DCEL/Vertex.hpp>
#include <Core/Mesh/DCEL/HalfEdge.hpp>
#include <Core/Mesh/DCEL/FullEdge.hpp>
#include <Core/Mesh/DCEL/Face.hpp>
#include <Core/Mesh/DCEL/Dcel.hpp>

#include <Core/Mesh/DCEL/Operation/Valid.hpp>
#include <Core/Mesh/DCEL/Operation/Border.hpp>
#include <Core/Mesh/DCEL/Operation/Bind.hpp>

namespace Ra {
namespace Core {


/// CONSTRUCTOR
EdgeFlipper::EdgeFlipper( const Dcel_ptr& dcel,
                              const Index&    param,
                              const bool      verbosity ) :
    Algorithm< Index >( param, "Edge Collapser", verbosity ),
    m_dcel( dcel ) { }

/// DESTRUCTOR
EdgeFlipper::~EdgeFlipper() { }

/// ALGORITHM STAGE
bool EdgeFlipper::configCheck( uint& exitStatus ) {
    bool  dcelStatus = ( m_dcel != nullptr );
    bool indexStatus = ( m_param.isValid() );
    bool      status = ( dcelStatus && indexStatus );
    if( status ) {
        exitStatus = NO_ERROR;
    } else {
        if( !dcelStatus ) {
            exitStatus = DCEL_NULLPTR;
        } else {
            exitStatus = INVALID_INDEX;
        }
    }
    return status;
}

bool EdgeFlipper::preprocessing( uint& exitStatus ) {
    if( !m_dcel->m_fulledge.access( m_param, m_fe ) ) {
        exitStatus = EDGE_NOT_PRESENT;
        return false;
    }

    if( !isValid( m_fe ) ) {
        exitStatus = INVALID_FULLEDGE;
        return false;
    }

    return true;
}

bool EdgeFlipper::processing( uint& exitStatus ) {

    if( !checkFullEdge( exitStatus ) ) {
        return false;
    }

    if( !isFlippable( exitStatus ) ) {
        return false;
    }

    if( !flipFullEdge( exitStatus ) ) {
        return false;
    }

    return true;

}

bool EdgeFlipper::postprocessing( uint& exitStatus ) {
    return true;
}

/// FUNCTION
bool EdgeFlipper::checkFullEdge( uint& exitStatus ) {
    // Useful names
    const uint x = 4; // HalfEdge to flip
    const uint y = 5; // HalfEdge to flip

    // Declare data
    Vertex_ptr   v[4];
    HalfEdge_ptr he[6];
    FullEdge_ptr fe;
    Face_ptr     f[2];

    // Initialize data
    fe    = m_fe;
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

    // Check data
    for( uint i = 0; i < 4; ++i ) {
        if( !isValid( v[i] ) ) {
            exitStatus = INVALID_VERTEX;
            return false;
        }
    }

    for( uint i = 0; i < 6; ++i ) {
        if( !isValid( he[i] ) ) {
            exitStatus = INVALID_HALFEDGE;
            return false;
        }
    }

    for( uint i = 0; i < 2; ++i ) {
        if( !isValid( f[i] ) ) {
            exitStatus = INVALID_FACE;
            return false;
        }
    }

    if( v[0] == v[1] ) {
        exitStatus = DEGENERATE_FULLEDGE;
        return false;
    }

    return true;
}



bool EdgeFlipper::isFlippable( uint& exitStatus ) {
    if( isBorder( m_fe ) ) {
        exitStatus = BORDER_FULLEDGE;
        return false;
    }

    // Useful names
    const uint x = 4; // HalfEdge to flip
    const uint y = 5; // HalfEdge to flip

    // Declare data
    Vertex_ptr   v[4];
    HalfEdge_ptr he[6];

    // Initialize data
    he[x] = m_fe->HE( 0 );
    he[y] = m_fe->HE( 1 );
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

    const Vector3 n0 = Geometry::triangleNormal( p, q, r );
    const Vector3 n1 = Geometry::triangleNormal( p, s, q );

    // DEGENERATE
    if( ( n0.isApprox( Vector3::Zero() ) ) || ( n1.isApprox( Vector3::Zero() ) ) ) {
        exitStatus = DEGENERATE_FACE;
        return false;
    }

    // Dihedral
    const Scalar angle = Vector::angle< Vector3 >( n0, n1 );
    if( angle > Math::PiDiv4 ) {
        exitStatus = DIHEDRAL_ANGLE_TOO_LARGE;
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
        exitStatus = INVERSION_OCCURS;
        return false;
    }

    return true;

}



bool EdgeFlipper::flipFullEdge( uint& exitStatus ) {
    // Useful names
    const uint x = 4; // HalfEdge to flip
    const uint y = 5; // HalfEdge to flip

    // Declare data
    Vertex_ptr   v[4];
    HalfEdge_ptr he[6];
    FullEdge_ptr fe;
    Face_ptr     f[2];

    // Initialize data
    fe    = m_fe;
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

    return true;
}

} // namespace Core
} // namespace Ra
