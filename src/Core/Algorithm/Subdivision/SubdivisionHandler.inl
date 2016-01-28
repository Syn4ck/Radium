#include <Core/Algorithm/Subdivision/SubdivisionHandler.hpp>

namespace Ra {
namespace Core {

/// DCEL
inline const Dcel_ptr getDCEL() const {
    return m_dcel;
}



inline void setDCEL( const Dcel_ptr& ptr ) {
    m_dcel = ptr;
}


/// SPLIT
inline void SubdivisionHandler::splitFullEdge( const Index& index ) {
    if( m_dcel == nullptr ) return;
    FullEdge_ptr fe;
    if( m_dcel->m_fulledge.access( index, fe ) ) {
        splitFullEdge( ptr );
    }
    return;
}

/// COLLAPSE
inline void SubdivisionHandler::collapseFullEdge( const Index& index ) {
    if( m_dcel == nullptr ) return;
    FullEdge_ptr fe;
    if( m_dcel->m_fulledge.access( index, fe ) ) {
        collapseFullEdge( ptr );
    }
    return;
}

/// FLIP
inline void SubdivisionHandler::flipFullEdge( const Index& index ) {
    if( m_dcel == nullptr ) return;
    FullEdge_ptr fe;
    if( m_dcel->m_fulledge.access( index, fe ) ) {
        flipFullEdge( ptr );
    }
    return;
}


} // namespace Core
} // namespace Ra
