#include <Core/Mesh/DCEL/Operation/Intersection.hpp>

#include <vector>
#include <algorithm>

#include <Core/Mesh/DCEL/Vertex.hpp>
#include <Core/Mesh/DCEL/Iterator/Vertex/VVIterator.hpp>

namespace Ra {
namespace Core {

uint oneRingIntersection( const Vertex_ptr& v0,
                          const Vertex_ptr& v1 ) {
    std::vector< Index > index;
    uint intersection = 0;
    Vertex_ptr v[2];
    v[0] = v0;
    v[1] = v1;
    for( uint i = 0; i < 2; ++i ) {
        VVIterator it( v[i] );
        auto list = it.list();
        for( const auto& item : list ) {
            index.push_back( item->idx );
        }
    }
    std::sort( index.begin(), index.end() );
    for( uint i = 0; i < index.size(); ++i ) {
        uint j;
        for( j = ( i + 1 ); j < index.size(); ++j ) {
            if( index[i] != index[j] ) {
                break;
            }
        }
        if( ( j - i ) > 1 ) {
            ++intersection;
        }
        i = j;
    }
    return intersection;
}

} // namespace Core
} // namespace Ra
