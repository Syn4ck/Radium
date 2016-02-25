#ifndef RADIUMENGINE_DCEL_ANGLE_OPERATION_DEFINITION_HPP
#define RADIUMENGINE_DCEL_ANGLE_OPERATION_DEFINITION_HPP

#include <Core/Mesh/DCEL/Definition.hpp>

namespace Ra {
namespace Core {

Scalar angle( const FullEdge_ptr& fe );
Scalar angle( const Face_ptr& f0, const Face_ptr& f1 );
Scalar minAngle( const Face_ptr& f );
Scalar maxAngle( const Face_ptr& f );

} // namespace Core
} // namespace Ra

#endif // RADIUMENGINE_DCEL_ANGLE_OPERATION_DEFINITION_HPP
