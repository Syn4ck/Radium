#include <Core/Algorithm/Subdivision/BK2004/BK2004Smoothing.hpp>

namespace Ra {
namespace Core {



void g( const VectorArray< Vector3 >&    p,
        const Geometry::AdjacencyMatrix& Adj,
        const Geometry::AreaMatrix&      A,
        GravityCentroid&                 G ) {
    const uint size = p.size();
    std::vector< Scalar > areaSum( size, 0.0 );
    G.resize( size );
#pragma omp parallel for
    for( uint i = 0; i < size; ++i ) {
        G[i].setZero();
        for( Geometry::AdjacencyMatrix::InnerIterator it( Adj, i ); it; ++it ) {
              const uint   j    = it.row();
              const Scalar area = A.coeff( j, j );
              G[i] += area * p[j];
              areaSum[i] += area;
        }
        G[i] /= areaSum[i];
    }
}



void tangentialRelaxation( const VectorArray< Vector3 >& p,
                           const VectorArray< Vector3 >& n,
                           const GravityCentroid&        g,
                           const Scalar                  lambda,
                           VectorArray< Vector3 >&       outPoint ) {
    const uint size = p.size();
    outPoint.resize( size );
#pragma omp parallel for
    for( uint i = 0; i < size; ++i ) {
        outPoint[i] = p[i] + ( lambda * ( Matrix3::Identity() - ( n[i] * n[i].transpose() ) ) * ( g[i] - p[i] ) );
    }
}



} // namespace Core
} // namespace Ra

