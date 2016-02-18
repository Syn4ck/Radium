#include <Core/Algorithm/Subdivision/DVBB2013/DVBB2013Smoothing.hpp>

#include <Core/Geometry/Triangle/TriangleOperation.hpp>

namespace Ra {
namespace Core {



void c( const VectorArray< Vector3 >&  p,
        const VectorArray< Triangle >& T,
        const VectorArray< Scalar >&   Lx,
        Centroid&                      C ) {
    const uint size = p.size();
    C.clear();
    C.resize( size, Vector3::Zero() );
    VectorArray< Scalar > weightSum( size, 0.0 );
#pragma omp parallel for
    for( uint n = 0; n < T.size(); ++n ) {
        const Triangle& t = T[n];
        const uint i = t[0];
        const uint j = t[1];
        const uint k = t[2];
        const Scalar  area = Geometry::triangleArea( p[i], p[j], p[k] );
        const Vector3 b    = (  p[i] +  p[j] +  p[k] ) / 3.0;
        const Scalar  Lb   = ( Lx[i] + Lx[j] + Lx[k] ) / 3.0;
        const Scalar  aLb  = area * Lb;
        const Vector3 aLbb = aLb * b;

#pragma omp critical
        {
            weightSum[i] += aLb;
            weightSum[j] += aLb;
            weightSum[k] += aLb;
        }

#pragma omp critical
        {
            C[i] += aLbb;
            C[j] += aLbb;
            C[k] += aLbb;
        }
    }
#pragma omp parallel for
    for( uint i = 0; i < size; ++i ) {
        C[i] /= weightSum[i];
    }
}



void tangentialRelaxation( const VectorArray< Vector3 >& p,
                           const VectorArray< Vector3 >& n,
                           const Centroid&               c,
                           VectorArray< Vector3 >&       outPoint ) {
    const uint size = p.size();
    outPoint.resize( size );
#pragma omp parallel for
    for( uint i = 0; i < size; ++i ) {
        outPoint[i] = c[i] + (  ( n[i] * n[i].transpose() ) * ( p[i] - c[i] ) );
    }
}



} // namespace Core
} // namespace Ra

