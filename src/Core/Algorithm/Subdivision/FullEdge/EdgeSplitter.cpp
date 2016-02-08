#include <Core/Algorithm/Subdivision/FullEdge/EdgeSplitter.hpp>

#include <Core/Mesh/DCEL/Vertex.hpp>
#include <Core/Mesh/DCEL/HalfEdge.hpp>
#include <Core/Mesh/DCEL/FullEdge.hpp>
#include <Core/Mesh/DCEL/Face.hpp>
#include <Core/Mesh/DCEL/Dcel.hpp>

#include <Core/Mesh/DCEL/Operation/Valid.hpp>
#include <Core/Mesh/DCEL/Operation/Bind.hpp>

namespace Ra {
namespace Core {


/// CONSTRUCTOR
EdgeSplitter::EdgeSplitter( const Dcel_ptr& dcel,
                            const Index&    param,
                            const bool      verbosity ) :
    Algorithm< Index >( param, "Edge Splitter", verbosity ),
    m_dcel( dcel ) { }

/// DESTRUCTOR
EdgeSplitter::~EdgeSplitter() { }

/// ALGORITHM STAGE
bool EdgeSplitter::configCheck( uint& exitStatus ) {
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

bool EdgeSplitter::preprocessing( uint& exitStatus ) {
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

bool EdgeSplitter::processing( uint& exitStatus ) {

    if( !checkFullEdge( exitStatus ) ) {
        return false;
    }

    if( !isSplittable( exitStatus ) ) {
        return true;
    }

    if( !splitFullEdge( exitStatus ) ) {
        return false;
    }

    return true;

}

bool EdgeSplitter::postprocessing( uint& exitStatus ) {
    return true;
}

/// FUNCTION
bool EdgeSplitter::checkFullEdge( uint& exitStatus ) {
    // Declare data
    Vertex_ptr   v[2];
    HalfEdge_ptr he[2];
    FullEdge_ptr fe;
    Face_ptr     f[2];

    // Initialize data
    fe    = m_fe;
    he[0] = fe->HE();
    he[1] = he[0]->Twin();
    v[0]  = he[0]->V();
    v[1]  = he[1]->V();
    f[0]  = he[0]->F();
    f[1]  = he[1]->F();

    // Check data
    for( uint i = 0; i < 2; ++i ) {
        if( !isValid( v[i] ) ) {
            exitStatus = INVALID_VERTEX;
            return false;
        }

        if( !isValid( he[i] ) ) {
            exitStatus = INVALID_HALFEDGE;
            return false;
        }

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



bool EdgeSplitter::isSplittable( uint& exitStatus ) {
    return true;
}



bool EdgeSplitter::splitFullEdge( uint& exitStatus ) {
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
    fe[0] = m_fe;
    he[0] = fe[0]->HE();
    he[1] = he[0]->Twin();
    he[x] = new HalfEdge();
    he[y] = new HalfEdge();
    v[0]  = he[0]->V();
    v[1]  = he[1]->V();
    v[n]  = new Vertex();
    fe[1] = new FullEdge( he[x] );
    f[0]  = he[0]->F();
    f[1]  = he[1]->F();

    // Insertion
    m_dcel->insert( v[n] );
    m_dcel->insert( he[x] );
    m_dcel->insert( he[y] );
    m_dcel->insert( fe[1] );

    bool insertStatus = true;

    if( !( insertStatus &= v[n]->idx.isValid() ) ) {
        exitStatus = VERTEX_NOT_INSERTED;
    }

    if( !( insertStatus &= he[x]->idx.isValid() ) ) {
        exitStatus = HALFEDGE_NOT_INSERTED;
    }

    if( !( insertStatus &= he[y]->idx.isValid() ) ) {
        exitStatus = HALFEDGE_NOT_INSERTED;
    }

    if( !( insertStatus &= fe[1]->idx.isValid() ) ) {
        exitStatus = FULLEDGE_NOT_INSERTED;
    }

    if( !insertStatus ) {
        delete v[n];
        delete he[x];
        delete he[y];
        delete fe[1];
        return false;
    }

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
    if( !splitFace( f[0], exitStatus ) ) {
        return false;
    }

    if( !splitFace( f[1], exitStatus ) ) {
        return false;
    }

    return true;
}

bool EdgeSplitter::splitFace( const Face_ptr& ptr, uint& exitStatus ) {
    // The face is a hole in the mesh
    if( ptr == nullptr ) return true;

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

    bool insertStatus = true;

    if( !( insertStatus &= he[x]->idx.isValid() ) ) {
        exitStatus = HALFEDGE_NOT_INSERTED;
    }

    if( !( insertStatus &= he[x]->idx.isValid() ) ) {
        exitStatus = HALFEDGE_NOT_INSERTED;
    }

    if( !( insertStatus &= fe->idx.isValid() ) ) {
        exitStatus = FULLEDGE_NOT_INSERTED;
    }

    if( !( insertStatus &= f[1]->idx.isValid() ) ) {
        exitStatus = FACE_NOT_INSERTED;
    }

    if( !insertStatus ) {
        delete he[x];
        delete he[y];
        delete fe;
        delete f[1];
        return false;
    }

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


} // namespace Core
} // namespace Ra
