#include <Core/Mesh/DCEL/Operation/Valence.hpp>

#include <Core/Mesh/DCEL/Vertex.hpp>
#include <Core/Mesh/DCEL/FullEdge.hpp>
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


uint valence( const FullEdge_ptr& fe, const bool flip ) {
    uint val = 0.0;
    int dv[4];
    dv[0] = dOptimalValence( fe->HE( 0 )->V() );
    dv[1] = dOptimalValence( fe->HE( 1 )->V() );
    dv[2] = dOptimalValence( fe->HE( 0 )->Prev()->V() );
    dv[3] = dOptimalValence( fe->HE( 1 )->Prev()->V() );
    for( uint i = 0; i < 4; ++i ) {
        dv[i] += ( flip ) ? ( ( ( i / 2 ) * 2 ) + -1 ) : 0.0;
        val += dv[i] * dv[i];
    }
    return val;
}


} // namespace Core
} // namespace Ra
