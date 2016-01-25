#include <Core/Mesh/DCEL/Operation/Valence.hpp>

#include <Core/Mesh/DCEL/Vertex.hpp>
#include <Core/Mesh/DCEL/Iterator/Vertex/VHEIterator.hpp>
#include <Core/Mesh/DCEL/Operation/Border.hpp>

namespace Ra {
namespace Core {

uint valence( const Vertex_ptr& v ) {
    VHEIterator it( v );
    return it.size();
}



uint optimalValence( const Vertex_ptr& v ) {
    if( isBorder( v ) ) {
        return 4;
    }
    return 6;
}



int dOptimalValence( const Vertex_ptr& v ) {
    return ( valence( v ) - optimalValence( v ) );
}



bool isExtraordinary( const Vertex_ptr& v ) {
    if( dOptimalValence( v ) == 0 ) return true;
    return false;
}


} // namespace Core
} // namespace Ra
