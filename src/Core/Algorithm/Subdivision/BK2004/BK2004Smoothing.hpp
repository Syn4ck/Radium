#ifndef RADIUMENGINE_BK2004_SMOOTHING_DEFINITION_HPP
#define RADIUMENGINE_BK2004_SMOOTHING_DEFINITION_HPP

#include <Core/Containers/VectorArray.hpp>
#include <Core/Geometry/Adjacency/Adjacency.hpp>
#include <Core/Geometry/Area/Area.hpp>

namespace Ra {
namespace Core {

typedef VectorArray< Vector3 > GravityCentroid;

void g( const VectorArray< Vector3 >&    p,
        const Geometry::AdjacencyMatrix& Adj,
        const Geometry::AreaMatrix&      A,
        GravityCentroid&                 G );



void tangentialRelaxation( const VectorArray< Vector3 >& p,
                           const VectorArray< Vector3 >& n,
                           const GravityCentroid&        g,
                           const Scalar                  lambda,
                           VectorArray< Vector3 >&       outPoint );



} // namespace Core
} // namespace Ra

#endif // RADIUMENGINE_BK2004_SMOOTHING_DEFINITION_HPP
