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
inline void SubdivisionHandler::splitFullEdge( const Index& index ) {
    if( m_dcel == nullptr ) return;
    if( index.isInvalid() ) return;
    FullEdge_ptr fe;
    if( m_dcel->m_fulledge.access( index, fe ) ) {
        bool status = splitFullEdge( fe );
        CORE_ASSERT( status, "Splitting failed." );
    }
}
#endif


#ifdef DEBUG_COLLAPSE
/// COLLAPSE
inline void SubdivisionHandler::collapseFullEdge( const Index& index ) {
    if( m_dcel == nullptr ) return;
    if( index.isInvalid() ) return;
    FullEdge_ptr fe;
    if( m_dcel->m_fulledge.access( index, fe ) ) {
        bool status = collapseFullEdge( fe );
        CORE_ASSERT( status, "Collapsing failed." );
    }
    return;
}
#endif


#ifdef DEBUG_FLIP
/// FLIP
inline void SubdivisionHandler::flipFullEdge( const Index& index ) {
    if( m_dcel == nullptr ) return;
    if( index.isInvalid() ) return;
    FullEdge_ptr fe;
    if( m_dcel->m_fulledge.access( index, fe ) ) {
        flipFullEdge( fe );
    }
    return;
}
#endif


} // namespace Core
} // namespace Ra
