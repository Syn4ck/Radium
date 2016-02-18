#include <Core/Algorithm/Subdivision/BK2004/BK2004Smoothing.hpp>

#include <Core/Geometry/Triangle/TriangleOperation.hpp>

namespace Ra {
namespace Core {



void g( const VectorArray< Vector3 >&    p,
        const Geometry::AdjacencyMatrix& Adj,
        const Geometry::AreaMatrix&      A,
        GravityCentroid&                 G ) {
    const uint size = p.size();
    VectorArray< Scalar > areaSum( size, 0.0 );
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



void g( const VectorArray< Vector3 >&    p,
        const VectorArray< Triangle >&   T,
        GravityCentroid&                 G ) {
    const uint v_size = p.size();
    std::vector< Scalar > area( v_size, 0.0 );
    std::vector< Scalar > areaSum( v_size, 0.0 );
    G.clear();
    G.resize( v_size, Vector3::Zero() );

    const uint size = T.size();
#pragma omp parallel for
    for( uint n = 0; n < size; ++n ) {
        const Triangle t = T[n];
        const uint i = t[0];
        const uint j = t[1];
        const uint k = t[2];
        const Scalar t_area  = Geometry::triangleArea( p[i], p[j], p[k] ) / 3.0;
#pragma omp atomic update
        area[i] += t_area;
#pragma omp atomic update
        area[j] += t_area;
#pragma omp atomic update
        area[k] += t_area;
    }

#pragma omp parallel for
    for( uint n = 0; n < size; ++n ) {
        const Triangle t = T[n];
        const uint i = t[0];
        const uint j = t[1];
        const uint k = t[2];
#pragma omp atomic update
        areaSum[i] += area[j];
#pragma omp atomic update
        areaSum[j] += area[k];
#pragma omp atomic update
        areaSum[k] += area[i];
#pragma omp critical
        {
            G[i] += ( area[j] * p[j] );
        }
#pragma omp critical
        {
            G[j] += ( area[k] * p[k] );
        }
#pragma omp critical
        {
            G[k] += ( area[i] * p[i] );
        }
    }

#pragma omp parallel for
    for( uint i = 0; i < v_size; ++i ) {
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

