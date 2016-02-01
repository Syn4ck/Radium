#include <Core/Mesh/DCEL/Operation/Length.hpp>

#include <limits>

#include <Core/Mesh/DCEL/Vertex.hpp>
#include <Core/Mesh/DCEL/HalfEdge.hpp>
#include <Core/Mesh/DCEL/FullEdge.hpp>
#include <Core/Mesh/DCEL/Dcel.hpp>

namespace Ra {
namespace Core {

Scalar length( const FullEdge_ptr& fe ) {
    const Vector3 p0 = fe->HE( 0 )->V()->P();
    const Vector3 p1 = fe->HE( 1 )->V()->P();
    return ( p0 - p1 ).squaredNorm();
}

Scalar maxFullEdgeLength( const Dcel_ptr& dcel ) {
    const uint size = dcel->m_fulledge.size();
    Scalar max = std::numeric_limits< Scalar >::min();
    for( uint i = 0; i < size; ++i ) {
        const Scalar l = length( dcel->m_fulledge.at( i ) );
        max = ( max < l ) ? l : max;
    }
    return max;
}

Scalar minFullEdgeLength( const Dcel_ptr& dcel ) {
    const uint size = dcel->m_fulledge.size();
    Scalar min = std::numeric_limits< Scalar >::max();
    for( uint i = 0; i < size; ++i ) {
        const Scalar l = length( dcel->m_fulledge.at( i ) );
        min = ( min > l ) ? l : min;
    }
    return min;
}

Scalar meanFullEdgeLength( const Dcel_ptr& dcel ) {
    const uint size = dcel->m_fulledge.size();
    Scalar sum = 0.0;
    for( uint i = 0; i < size; ++i ) {
        sum += length( dcel->m_fulledge.at( i ) );
    }
    return ( sum / Scalar( size ) );
}

void FullEdgeLength( const Dcel_ptr& dcel, Scalar& max, Scalar& min, Scalar& mean ) {
    const uint size = dcel->m_fulledge.size();
    max  = std::numeric_limits< Scalar >::min();
    min  = std::numeric_limits< Scalar >::max();
    mean = 0.0;
    for( uint i = 0; i < size; ++i ) {
        const Scalar l = length( dcel->m_fulledge.at( i ) );
        max   = ( max < l ) ? l : max;
        min   = ( min > l ) ? l : min;
        mean += l;
    }
    mean = mean / Scalar( size );
}

void FullEdgeLength( const Dcel_ptr& dcel, std::vector< Scalar >& lengthValue ) {
    lengthValue.clear();
    const uint size = dcel->m_fulledge.size();
    lengthValue.resize( size );
#pragma omp parallel for
    for( uint i = 0; i < size; ++i ) {
        lengthValue[i] = length( dcel->m_fulledge.at( i ) );
    }
}


} // namespace Core
} // namespace Ra
