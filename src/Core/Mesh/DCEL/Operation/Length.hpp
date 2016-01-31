#ifndef RADIUMENGINE_DCEL_LENGTH_OPERATION_DEFINITION_HPP
#define RADIUMENGINE_DCEL_LENGTH_OPERATION_DEFINITION_HPP

#include <Core/CoreMacros.hpp>
#include <Core/Mesh/DCEL/Definition.hpp>

namespace Ra {
namespace Core {

Scalar length( const FullEdge_ptr& fe );
Scalar maxFullEdgeLength( const Dcel_ptr& dcel );
Scalar minFullEdgeLength( const Dcel_ptr& dcel );
Scalar meanFullEdgeLength( const Dcel_ptr& dcel );
Scalar FullEdgeLength( const Dcel_ptr& dcel, Scalar& max, Scalar& min, Scalar& mean );
Scalar FullEdgeLength( const Dcel_ptr& dcel, std::vector< Scalar >& lengthValue );

} // namespace Core
} // namespace Ra

#endif // RADIUMENGINE_DCEL_LENGTH_OPERATION_DEFINITION_HPP
