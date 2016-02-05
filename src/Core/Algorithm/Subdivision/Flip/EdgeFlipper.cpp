#include <Core/Algorithm/Subdivision/Flip/EdgeFlipper.hpp>

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
        if( !isValid( v[1] ) ) {
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
        if( !isValid( f[0] ) ) {
            exitStatus = INVALID_FACE;
            return false;
        }
    }

    return true;
}


bool EdgeFlipper::flipFullEdge( uint& exitStatus ) {
    // In case of a border fulledge, do not flip
    if( isBorder( m_fe ) ) {
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
