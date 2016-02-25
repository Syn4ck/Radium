#include <Core/Mesh/DCEL/Operation/Angle.hpp>

#include <Core/Mesh/DCEL/Vertex.hpp>
#include <Core/Mesh/DCEL/HalfEdge.hpp>
#include <Core/Mesh/DCEL/FullEdge.hpp>
#include <Core/Mesh/DCEL/Face.hpp>

#include <Core/Mesh/DCEL/Iterator/Face/FVIterator.hpp>

namespace Ra {
namespace Core {

Scalar angle( const FullEdge_ptr& fe ) {
    return angle( fe->F( 0 ), fe->F( 1 ) );
}

Scalar angle( const Face_ptr& f0, const Face_ptr& f1 ) {
    Face_ptr f[2];
    f[0] = f0;
    f[1] = f1;

    Vector3 n[2];
    for( uint i = 0; i < 2; ++i ) {
        n[i].setZero();
        FVIterator it( f[i] );
        const uint size = it.size();
        for( uint j = 0; j < size; ++j ) {
            n[i] += it->N();
            ++it;
        }
    }

    return Vector::angle< Vector3 >( n[0], n[1] );
}

} // namespace Core
} // namespace Ra
