#include <Core/Algorithm/Subdivision/Collapse/EdgeCollapser.hpp>

#include <Core/Geometry/Triangle/TriangleOperation.hpp>

#include <Core/Mesh/DCEL/Vertex.hpp>
#include <Core/Mesh/DCEL/HalfEdge.hpp>
#include <Core/Mesh/DCEL/FullEdge.hpp>
#include <Core/Mesh/DCEL/Face.hpp>
#include <Core/Mesh/DCEL/Dcel.hpp>

#include <Core/Mesh/DCEL/Iterator/Vertex/VHEIterator.hpp>

#include <Core/Mesh/DCEL/Operation/Bind.hpp>
#include <Core/Mesh/DCEL/Operation/Border.hpp>
#include <Core/Mesh/DCEL/Operation/Valid.hpp>

namespace Ra {
namespace Core {


/// CONSTRUCTOR
EdgeCollapser::EdgeCollapser( const Dcel_ptr& dcel,
                              const Index&    param,
                              const bool      verbosity ) :
    Algorithm< Index >( param, "Edge Collapser", verbosity ),
    m_dcel( dcel ) { }

/// DESTRUCTOR
EdgeCollapser::~EdgeCollapser() { }

/// ALGORITHM STAGE
bool EdgeCollapser::configCheck( uint& exitStatus ) {
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

bool EdgeCollapser::preprocessing( uint& exitStatus ) {
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

bool EdgeCollapser::processing( uint& exitStatus ) {

    if( !checkFullEdge( exitStatus ) ) {
        return false;
    }

    if( !isCollapsable( exitStatus ) ) {
        return false;
    }

    if( !collapseFullEdge( exitStatus ) ) {
        return false;
    }

    return true;

}

bool EdgeCollapser::postprocessing( uint& exitStatus ) {
    return true;
}

/// FUNCTION
bool EdgeCollapser::checkFullEdge( uint& exitStatus ) {
    // Useful names
    const uint x = 4; // HalfEdge to delete
    const uint y = 5; // HalfEdge to delete

    // Declare data
    Vertex_ptr   v[2];
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
    v[0]  = he[x]->V();
    v[1]  = he[y]->V();
    f[0]  = he[x]->F();
    f[1]  = he[y]->F();

    // Check data
    for( uint i = 0; i < 2; ++i ) {
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



bool EdgeCollapser::isCollapsable( uint& exitStatus ) {
    // Border
    if( isBorder( m_fe ) ) {
        exitStatus = BORDER_FULLEDGE;
        return false;
    }
    // One ring intersection
    for( uint i = 0; i < 2; ++i ) {
        if( !isOneRingIntersectionFine( m_fe->HE( i ) ) ) {
            exitStatus = ONE_RING_INTERSECTION_PROBLEM;
            return false;
        }
    }

    // Inversion
    Vertex_ptr v[2];
    for( uint i = 0; i < 2; ++i ) {
        v[i] = m_fe->V( i );
    }
    Vector3 c = 0.5 * ( v[0]->P() + v[1]->P() );

    for( uint i = 0; i < 2; ++i ) {
        Vector3 p[3];
        p[0] = v[i]->P();
        VHEIterator it( v[i] );
        auto list = it.list();
        for( const auto& he : list ) {
            if( ( he->F() == nullptr      ) ||
                ( he->F() == m_fe->F( 0 ) ) ||
                ( he->F() == m_fe->F( 1 ) ) ) {
                continue;
            }

            p[1] = he->Next()->V()->P();
            p[2] = he->Prev()->V()->P();

            Vector3 n0 = Geometry::triangleNormal( p[0], p[1], p[2] );
            Vector3 n1 = Geometry::triangleNormal( c,    p[1], p[2] );

            // DEGENERATE
            if( ( n0.isApprox( Vector3::Zero() ) ) || ( n1.isApprox( Vector3::Zero() ) ) ) {
                exitStatus = DEGENERATE_FACE;
                return false;
            }

            const Scalar angle = Vector::angle< Vector3 >( n0, n1 );
            if( angle > Math::PiDiv2 ) {
                exitStatus = FACE_INVERSION;
                return false;
            }
        }
    }

    return true;
}



bool EdgeCollapser::isOneRingIntersectionFine( const HalfEdge_ptr& ptr ) {
    // Declare data
    Vertex_ptr   v[3];
    HalfEdge_ptr he[3];

    // Initialize data
    he[0] = ptr;
    he[1] = he[0]->Next()->Twin()->Next();
    he[2] = he[0]->Prev()->Twin()->Prev();
    v[0] = he[0]->V();
    v[1] = he[1]->V();
    v[2] = he[2]->V();

    // Check
    if( he[1]->Twin()->V() == v[2] ) {
        return false;
    }

    return true;
}



bool EdgeCollapser::collapseFullEdge( uint& exitStatus ) {
    // Useful names
    const uint x = 4; // HalfEdge to delete
    const uint y = 5; // HalfEdge to delete

    // Declare data
    Vertex_ptr   v[2];
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
    v[0]  = he[x]->V();
    v[1]  = he[y]->V();
    f[0]  = he[x]->F();
    f[1]  = he[y]->F();

    // Remove data
    m_dcel->removeVertex(  v[1]->idx );
    if( v[1]->idx.isValid() ) {
        exitStatus = VERTEX_NOT_REMOVED;
        return false;
    }

    m_dcel->removeHalfEdge( he[x]->idx );
    if( he[x]->idx.isValid() ) {
        exitStatus = HALFEDGE_NOT_REMOVED;
        return false;
    }

    m_dcel->removeHalfEdge( he[y]->idx );
    if( he[y]->idx.isValid() ) {
        exitStatus = HALFEDGE_NOT_REMOVED;
        return false;
    }

    m_dcel->removeHalfEdge( fe->idx );
    if( fe->idx.isValid() ) {
        exitStatus = FULLEDGE_NOT_REMOVED;
        return false;
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

    // Delete data
    delete v[1];
    delete he[x];
    delete he[y];
    delete fe;

    // Handle face collapsing
    if( !collapseFace( f[0], exitStatus ) ) {
        return false;
    }

    if( !collapseFace( f[1], exitStatus ) ) {
        return false;
    }

    return true;
}



bool EdgeCollapser::collapseFace( const Face_ptr& ptr, uint& exitStatus ) {
    // The face is a hole in the mesh
    if( ptr == nullptr ) {
        return true;
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
        exitStatus = INVALID_VERTEX;
        return false;
    }

    if( v[0] == v[1] ) {
        exitStatus = DEGENERATE_FULLEDGE;
        return false;
    }

    if( !isValid( he[x] ) || !isValid( he[y] ) ) {
        exitStatus = INVALID_HALFEDGE;
        return false;
    }

    if( !isValid( fe[1] ) ) {
        exitStatus = INVALID_FULLEDGE;
        return false;
    }

    // Remove data
    m_dcel->removeHalfEdge( he[x]->idx );
    if( he[x]->idx.isValid() ) {
        exitStatus = HALFEDGE_NOT_REMOVED;
        return false;
    }

    m_dcel->removeHalfEdge( he[y]->idx );
    if( he[y]->idx.isValid() ) {
        exitStatus = HALFEDGE_NOT_REMOVED;
        return false;
    }

    m_dcel->removeHalfEdge( fe[1]->idx );
    if( fe[1]->idx.isValid() ) {
        exitStatus = FULLEDGE_NOT_REMOVED;
        return false;
    }

    m_dcel->removeFace( f->idx );
    if( f->idx.isValid() ) {
        exitStatus = FACE_NOT_REMOVED;
        return false;
    }

    // Set data
    v[0]->setHE( he[1] );
    v[1]->setHE( he[0] );

    he[0]->setTwin( he[1] );
    he[1]->setTwin( he[0] );

    fe[0]->setHE( he[0] );
    bind( fe[0] );

    // Unbind old data
    unbind( fe[1] );
    unbind( he[x] );
    unbind( he[y] );
    unbind( f );

    // Delete data
    delete he[x];
    delete he[y];
    delete fe[1];
    delete f;

    return true;
}






} // namespace Core
} // namespace Ra
