#include <Core/Algorithm/Subdivision/FullEdge/EdgeOperation.hpp>

#include <Core/Mesh/DCEL/FullEdge.hpp>
#include <Core/Mesh/DCEL/Dcel.hpp>

#include <Core/Mesh/DCEL/Operation/Valid.hpp>

namespace Ra {
namespace Core {


/// CONSTRUCTOR
EdgeOperation::EdgeOperation( const std::string& name,
                              const Dcel_ptr&    dcel,
                              const Index&       param,
                              const bool         verbosity ) :
    Algorithm< Index >( param, name, verbosity ),
    m_dcel( dcel ) { }

/// DESTRUCTOR
EdgeOperation::~EdgeOperation() { }

/// ALGORITHM STAGE
bool EdgeOperation::configCheck( uint& exitStatus ) {
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

bool EdgeOperation::preprocessing( uint& exitStatus ) {
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

bool EdgeOperation::processing( uint& exitStatus ) {

    if( !checkFullEdge( exitStatus ) ) {
        return false;
    }

    if( !isProcessable( exitStatus ) ) {
        return true;
    }

    if( !processFullEdge( exitStatus ) ) {
        return false;
    }

    return true;

}

bool EdgeOperation::postprocessing( uint& exitStatus ) {
    return true;
}


} // namespace Core
} // namespace Ra
