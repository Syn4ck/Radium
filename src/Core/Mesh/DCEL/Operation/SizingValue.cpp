#include <Core/Mesh/DCEL/Operation/SizingValue.hpp>

//#include <cmath>

#include <Core/Geometry/Area/Area.hpp>
#include <Core/Geometry/Laplacian/Laplacian.hpp>
#include <Core/Geometry/Curvature/Curvature.hpp>

#include <Core/Mesh/DCEL/Vertex.hpp>
#include <Core/Mesh/DCEL/FullEdge.hpp>
#include <Core/Mesh/DCEL/Face.hpp>

#include <Core/Mesh/DCEL/Iterator/Vertex/VVIterator.hpp>
#include <Core/Mesh/DCEL/Iterator/Face/FVIterator.hpp>

#include <Core/Mesh/DCEL/Operation/Valid.hpp>
#include <Core/Mesh/DCEL/Operation/Convert.hpp>

namespace Ra {
namespace Core {

Scalar sizingValue( const Vertex_ptr& v, const Scalar eps ) {
    CORE_ASSERT( isValid( v ), "Vertex not valid." );

    Vector3 p;
    convert( v, p );
    CORE_ASSERT( p.allFinite(), "Position of v is not finite." );

    VVIterator it( v );
    VectorArray< Vector3 > one_ring;
    convert( it.list(), one_ring );
    CORE_ASSERT( !one_ring.empty(), "One ring is empty." );

    Scalar area = Geometry::voronoiArea( p, one_ring );
    //Scalar area = Geometry::mixedArea( p, one_ring );
    CORE_ASSERT( std::isfinite( area ), "Area is not finite." );
    CORE_ASSERT( ( area != 0.0 ), "Area is null." );
    CORE_ASSERT( ( area > 0.0 ), "Area is negative." );

    Vector3 L = Geometry::cotangentWeightLaplacian( p, one_ring );
    CORE_ASSERT( L.allFinite(), "Laplacian is not finite." );

    Vector3 Hn = Geometry::meanCurvatureNormal( L, area );
    CORE_ASSERT( Hn.allFinite(), "Mean Curvature Normal is not finite." );

    Scalar H = Geometry::meanCurvature( Hn );
    CORE_ASSERT( std::isfinite( H ), "Mean Curvature is not finite." );

    Scalar K = Geometry::gaussianCurvature( p, one_ring, area );
    CORE_ASSERT( std::isfinite( K ), "Gaussian Curvature is not finite." );

    Scalar kappa = Geometry::maxCurvature( H, K );
    CORE_ASSERT( std::isfinite( kappa ), "Max Curvature is not finite." );
    CORE_ASSERT( ( kappa > 0.0 ), "Max Curvature MUST be positive" );

    Scalar a = ( 6.0 * eps / kappa );
    CORE_ASSERT( std::isfinite( a ), "Scalar a is not finite." );

    Scalar b = ( 3.0 * eps * eps );
    CORE_ASSERT( std::isfinite( b ), "Scalar b is not finite." );

    CORE_ASSERT( ( a > b ), "a MUST be greater than b." );

    Scalar sv = std::sqrt( a - b );
    CORE_ASSERT( std::isfinite( sv ), "Sizing Value is not finite." );

    return ( sv );
}



Scalar sizingValue( const FullEdge_ptr& fe, const Scalar eps ) {
    CORE_ASSERT( isValid( fe ), "FullEdge not valid." );
    Scalar sv = std::min( sizingValue( fe->HE( 0 )->V(), eps ),
                          sizingValue( fe->HE( 1 )->V(), eps ) );
    CORE_ASSERT( std::isfinite( sv ), "Sizing Value is not finite." );
    return ( sv );
}



Scalar sizingValue( const Face_ptr& f, const Scalar eps ) {
    CORE_ASSERT( isValid( f ), "Face not valid." );
    Scalar sumL = 0.0;
    FVIterator it( f );
    const auto list = it.list();
    CORE_ASSERT( ( !list.empty() ), "Face is corrupted." );
    for( const auto& v : list ) {
        sumL += sizingValue( v, eps );
    }
    Scalar sv = sumL / Scalar( list.size() );
    CORE_ASSERT( std::isfinite( sv ), "Sizing Value is not finite." );
    return ( sv );
}

} // namespace Core
} // namespace Ra

