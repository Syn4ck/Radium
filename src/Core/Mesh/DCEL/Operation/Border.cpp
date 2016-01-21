#include <Core/Mesh/DCEL/Operation/Border.hpp>

#include <Core/CoreMacros.hpp>

#include <Core/Mesh/DCEL/Vertex.hpp>
#include <Core/Mesh/DCEL/HalfEdge.hpp>
#include <Core/Mesh/DCEL/FullEdge.hpp>
#include <Core/Mesh/DCEL/Face.hpp>

#include <Core/Mesh/DCEL/Iterator/Vertex/VHEIterator.hpp>
#include <Core/Mesh/DCEL/Iterator/Face/FHEIterator.hpp>

namespace Ra {
namespace Core {

bool isBorder( const Vertex_ptr& v ) {
#ifdef CORE_DEBUG
    CORE_ASSERT( ( v != nullptr ), " Nullptr in input " );
#else
    if( v == nullptr ) return true;
#endif
    VHEIterator it( v );
    if( ( *it ) != nullptr ) {
        const uint size = it.size();
        for( uint i = 0; i < size; ++i ) {
            if( isBorder( *it ) ) {
                return true;
            }
            ++it;
        }
        return false;
    }
    return true;
}



bool isBorder( const HalfEdge_ptr& he ) {
#ifdef CORE_DEBUG
    CORE_ASSERT( ( he != nullptr ), " Nullptr in input " );
#else
    if( he == nullptr ) return true;
#endif
    return ( he->F() == nullptr );
}



bool isBorder( const FullEdge_ptr& fe ) {
#ifdef CORE_DEBUG
    CORE_ASSERT( ( fe != nullptr ), " Nullptr in input " );
#else
    if( fe == nullptr ) return true;
#endif
    return ( isBorder( fe->HE( 0 ) ) || isBorder( fe->HE( 1 ) ) );
}



bool isBorder( const Face_ptr& f ) {
#ifdef CORE_DEBUG
    CORE_ASSERT( ( f != nullptr ), " Nullptr in input " );
#else
    if( f == nullptr ) return true;
#endif
    FHEIterator it( f );
    if( ( *it ) != nullptr ) {
        const uint size = it.size();
        for( uint i = 0; i < size; ++i ) {
            if( it->Twin() != nullptr ) {
                if( isBorder( it->Twin() ) ) {
                    return true;
                }
            }
            ++it;
        }
        return false;
    }
    return true;
}

} // namespace Core
} // namespace Ra
