#ifndef RADIUMENGINE_DVBB2013_SMOOTHING_DEFINITION_HPP
#define RADIUMENGINE_DVBB2013_SMOOTHING_DEFINITION_HPP

#include <Core/Containers/VectorArray.hpp>
#include <Core/Math/LinearAlgebra.hpp>
#include <Core/Mesh/MeshTypes.hpp>

namespace Ra {
namespace Core {

typedef VectorArray< Vector3 > Centroid;

void c( const VectorArray< Vector3 >&    p,
        const VectorArray< Triangle >&   T,
        const VectorArray< Scalar >&     Lx,
        Centroid&                        C );



void tangentialRelaxation( const VectorArray< Vector3 >& p,
                           const VectorArray< Vector3 >& n,
                           const Centroid&               c,
                           VectorArray< Vector3 >&       outPoint );



} // namespace Core
} // namespace Ra

#endif // RADIUMENGINE_DVBB2013_SMOOTHING_DEFINITION_HPP
