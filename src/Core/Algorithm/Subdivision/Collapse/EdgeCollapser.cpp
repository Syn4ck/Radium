#include <Core/Algorithm/Subdivision/Collapse/EdgeCollapser.hpp>

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

    /*if( !splitFullEdge( exitStatus ) ) {
        return false;
    }*/

    return true;

}

bool EdgeCollapser::postprocessing( uint& exitStatus ) {
    return true;
}

/// FUNCTION
bool EdgeCollapser::checkFullEdge( uint& exitStatus ) {
    return true;
}



} // namespace Core
} // namespace Ra
