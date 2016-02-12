#include <Core/Algorithm/Subdivision/FullEdge/EdgeFlipper.hpp>

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
    EdgeOperation( "Edge Flipper", dcel, param, verbosity ) { }



/// DESTRUCTOR
EdgeFlipper::~EdgeFlipper() { }



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



bool EdgeFlipper::isProcessable( uint& exitStatus ) {
    // Border
    if( isBorder( m_fe ) ) {
        exitStatus = BORDER_FULLEDGE;
        return false;
    }

    // DEGENERATE
    if( isDegenerateFace() ) {
        exitStatus = DEGENERATE_FACE;
        return false;
    }

    // Dihedral
    if( isDihedralAngleTooBig() ) {
        exitStatus = DIHEDRAL_ANGLE_TOO_LARGE;
        return false;
    }

    // Convexity
    if( isFaceInversion() ) {
        exitStatus = FACE_INVERSION;
        return false;
    }

    // QUALITY DECREASING
    if( isQualityDecreasing() ) {
        exitStatus = WORSE_QUALITY;
        return false;
    }

    return true;
}



bool EdgeFlipper::processFullEdge( uint& exitStatus ) {
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



bool EdgeFlipper::isDegenerateFace() const {
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

    // Set points
    const Vector3 P = v[2]->P();
    const Vector3 Q = v[0]->P();
    const Vector3 R = v[1]->P();
    const Vector3 S = v[3]->P();

    const Vector3 n0 = Geometry::triangleNormal( P, Q, R );
    const Vector3 n1 = Geometry::triangleNormal( P, S, Q );

    // DEGENERATE
    if( ( n0.isApprox( Vector3::Zero() ) ) || ( n1.isApprox( Vector3::Zero() ) ) ) {
        return true;
    }

    return false;
}



bool EdgeFlipper::isDihedralAngleTooBig() const {
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

    // Set points
    const Vector3 P = v[2]->P();
    const Vector3 Q = v[0]->P();
    const Vector3 R = v[1]->P();
    const Vector3 S = v[3]->P();

    const Vector3 n0 = Geometry::triangleNormal( P, Q, R );
    const Vector3 n1 = Geometry::triangleNormal( P, S, Q );

    // Dihedral
    const Scalar angle = Vector::angle< Vector3 >( n0, n1 );
    if( angle > Math::PiDiv4 ) {
        return true;
    }

    return false;
}



bool EdgeFlipper::isFaceInversion() const {
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

    // Set points
    const Vector3 P = v[2]->P();
    const Vector3 Q = v[0]->P();
    const Vector3 R = v[1]->P();
    const Vector3 S = v[3]->P();

    // Compute edges
    const Vector3 pq = ( Q - P ).normalized();
    const Vector3 ps = ( S - P ).normalized();
    const Vector3 sq = ( Q - S ).normalized();
    const Vector3 qr = ( R - Q ).normalized();
    const Vector3 rp = ( P - R ).normalized();

    // Compute angles
    Scalar angle_p[2];
    Scalar angle_q[2];

    angle_p[0] = Vector::angle< Vector3 >( -rp,  pq );
    angle_p[1] = Vector::angle< Vector3 >(  pq,  ps );

    angle_q[0] = Vector::angle< Vector3 >( -pq,  qr );
    angle_q[1] = Vector::angle< Vector3 >( -pq, -sq );

    // Convexity
    const Scalar a0 = angle_p[0];
    const Scalar a1 = angle_p[1];

    const Scalar b0 = angle_q[0];
    const Scalar b1 = angle_q[1];

    const Scalar alpha = a0 + a1;
    const Scalar beta  = b0 + b1;

    if( ( alpha >= Math::Pi ) || ( beta >= Math::Pi ) ) {
        return true;
    }

    return false;
}



bool EdgeFlipper::isQualityDecreasing() const {
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

    const Vector3 P = v[2]->P();
    const Vector3 Q = v[0]->P();
    const Vector3 R = v[1]->P();
    const Vector3 S = v[3]->P();

    const Vector3 pq = ( Q - P ).normalized();
    const Vector3 rs = ( S - R ).normalized();
    const Vector3 ps = ( S - P ).normalized();
    const Vector3 sq = ( Q - S ).normalized();
    const Vector3 qr = ( R - Q ).normalized();
    const Vector3 rp = ( P - R ).normalized();

    Scalar angle_p[3];
    Scalar angle_q[3];
    Scalar angle_r[3];
    Scalar angle_s[3];

    // Angles : 0 small face_0, 1 small face_1, 2 big
    angle_p[0] = Vector::angle< Vector3 >( -rp,  pq );
    angle_p[1] = Vector::angle< Vector3 >(  pq,  ps );
    angle_p[2] = Vector::angle< Vector3 >( -rp,  ps );

    angle_q[0] = Vector::angle< Vector3 >( -pq,  qr );
    angle_q[1] = Vector::angle< Vector3 >( -pq, -sq );
    angle_q[2] = Vector::angle< Vector3 >(  qr, -sq );

    angle_r[0] = Vector::angle< Vector3 >(  rs,  rp );
    angle_r[1] = Vector::angle< Vector3 >(  rs, -qr );
    angle_r[2] = Vector::angle< Vector3 >(  rp, -qr );

    angle_s[0] = Vector::angle< Vector3 >( -ps, -rs );
    angle_s[1] = Vector::angle< Vector3 >(  sq, -rs );
    angle_s[2] = Vector::angle< Vector3 >( -ps,  sq );

    if( std::min( angle_p[0], std::min( angle_q[0], angle_r[2] ) ) >=
        std::min( angle_p[2], std::min( angle_r[0], angle_s[0] ) ) ) {
        return true;
    }

    if( std::min( angle_p[1], std::min( angle_q[1], angle_s[2] ) ) >=
        std::min( angle_q[2], std::min( angle_r[1], angle_s[1] ) ) ) {
        return true;
    }
    return false;
}



} // namespace Core
} // namespace Ra
