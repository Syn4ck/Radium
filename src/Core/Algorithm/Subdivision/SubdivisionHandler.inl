#include <Core/Algorithm/Subdivision/SubdivisionHandler.hpp>

#include <Core/Mesh/DCEL/FullEdge.hpp>
#include <Core/Mesh/DCEL/Dcel.hpp>

namespace Ra {
namespace Core {

/// DCEL
inline Dcel_ptr SubdivisionHandler::getDCEL() const {
    return m_dcel;
}



inline void SubdivisionHandler::setDCEL( const Dcel_ptr& ptr ) {
    m_dcel = ptr;
}


#ifdef DEBUG_SPLIT
/// SPLIT
inline bool SubdivisionHandler::splitFullEdge( const Index& index ) {
    if( m_dcel == nullptr ) {
        return false;
    }
    if( index.isInvalid() ) {
        return false;
    }
    FullEdge_ptr fe;
    if( m_dcel->m_fulledge.access( index, fe ) ) {
        bool status = splitFullEdge( fe );
        CORE_ASSERT( status, "Splitting failed." );
        return status;
    }
    return false;
}



inline bool SubdivisionHandler::splitFullEdge( const uint i ) {
    if( m_dcel == nullptr ) {
        return false;
    }
    if( i < m_dcel->m_fulledge.size() ) {
        return false;
    }
    FullEdge_ptr fe;
    if( m_dcel->m_fulledge.access( i, fe ) ) {
        bool status = splitFullEdge( fe );
        CORE_ASSERT( status, "Splitting failed." );
        return status;
    }
    return false;
}
#endif


#ifdef DEBUG_COLLAPSE
/// COLLAPSE
inline bool SubdivisionHandler::collapseFullEdge( const Index& index ) {
    if( m_dcel == nullptr ) {
        return false;
    }
    if( index.isInvalid() ) {
        return false;
    }
    FullEdge_ptr fe;
    if( m_dcel->m_fulledge.access( index, fe ) ) {
        bool status = collapseFullEdge( fe );
        CORE_ASSERT( status, "Collapsing failed." );
        return status;
    }
    return false;
}



inline bool SubdivisionHandler::collapseFullEdge( const uint i ) {
    if( m_dcel == nullptr ) {
        return false;
    }
    if( i < m_dcel->m_fulledge.size() ) {
        return false;
    }
    FullEdge_ptr fe;
    if( m_dcel->m_fulledge.access( i, fe ) ) {
        bool status = collapseFullEdge( fe );
        CORE_ASSERT( status, "Collapsing failed." );
        return status;
    }
    return false;
}
#endif


#ifdef DEBUG_FLIP
/// FLIP
inline bool SubdivisionHandler::flipFullEdge( const Index& index ) {
    if( m_dcel == nullptr ) {
        return false;
    }
    if( index.isInvalid() ) {
        return false;
    }
    FullEdge_ptr fe;
    if( m_dcel->m_fulledge.access( index, fe ) ) {
        bool status = flipFullEdge( fe );
        CORE_ASSERT( status, "Flipping failed." );
        return status;
    }
    return false;
}



inline bool SubdivisionHandler::flipFullEdge( const uint i ) {
    if( m_dcel == nullptr ) {
        return false;
    }
    if( i < m_dcel->m_fulledge.size() ) {
        return false;
    }
    FullEdge_ptr fe;
    if( m_dcel->m_fulledge.access( i, fe ) ) {
        bool status = flipFullEdge( fe );
        CORE_ASSERT( status, "Flipping failed." );
        return status;
    }
    return false;
}
#endif


} // namespace Core
} // namespace Ra
