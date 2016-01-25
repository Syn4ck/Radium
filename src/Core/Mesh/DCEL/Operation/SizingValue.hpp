#ifndef RADIUMENGINE_DCEL_SIZING_VALUE_DEFINITION_HPP
#define RADIUMENGINE_DCEL_SIZING_VALUE_DEFINITION_HPP

#include <Core/Mesh/DCEL/Definition.hpp>

namespace Ra {
namespace Core {

Scalar sizingValue( const Vertex_ptr&   v,  const Scalar eps );
Scalar sizingValue( const FullEdge_ptr& fe, const Scalar eps );
Scalar sizingValue( const Face_ptr&     f,  const Scalar eps );

} // namespace Core
} // namespace Ra

#endif // RADIUMENGINE_DCEL_SIZING_VALUE_DEFINITION_HPP
