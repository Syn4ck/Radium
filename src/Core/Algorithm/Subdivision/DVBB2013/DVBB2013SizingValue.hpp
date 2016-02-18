#ifndef RADIUMENGINE_DVBB2013_SIZING_VALUE_DEFINITION_HPP
#define RADIUMENGINE_DVBB2013_SIZING_VALUE_DEFINITION_HPP

#include <Core/Geometry/Area/Area.hpp>
#include <Core/Geometry/Curvature/Curvature.hpp>
#include <Core/Geometry/Laplacian/Laplacian.hpp>

namespace Ra {
namespace Core {

typedef VectorArray< Scalar > SizingValue;

void sizingValueVertex( const VectorArray< Vector3 >& x, const VectorArray< Triangle >& T, const Scalar eps, SizingValue& Lx ) {
    const uint size = x.size();
    Lx.resize( size );

    const Geometry::AreaMatrix      A = Geometry::voronoiArea( x, T );
    const Geometry::LaplacianMatrix L = Geometry::cotangentWeightLaplacian( x, T );
    VectorArray< Vector3 >          laplacian( size );
    VectorArray< Vector3 >          Hn( size );
    VectorArray< Scalar >           H( size );
    VectorArray< Scalar >           K( size );
    VectorArray< Scalar >           k1( size );

    laplacian.getMap() = x.getMap() * L;

    Geometry::meanCurvatureNormal( laplacian, A, Hn );
    Geometry::meanCurvature( Hn, H );
    Geometry::gaussianCurvature( x, T, A, K );
    Geometry::maxCurvature( H, K, k1 );

#pragma omp parallel for
    for( uint i = 0; i < size; ++i ) {
        Lx[i] = std::sqrt( ( ( 6.0 * eps ) / k1[i] ) - ( 3.0 * eps * eps ) );
    }
}

} // namespace Core
} // namespace Ra

#endif // RADIUMENGINE_DVBB2013_SIZING_VALUE_DEFINITION_HPP
