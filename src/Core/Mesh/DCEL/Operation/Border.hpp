#ifndef RADIUMENGINE_BORDER_OPERATION_DEFINITION_HPP
#define RADIUMENGINE_BORDER_OPERATION_DEFINITION_HPP

#include <Core/Mesh/DCEL/Definition.hpp>

namespace Ra {
namespace Core {

bool isBorder( const Vertex_ptr&   v  );
bool isBorder( const HalfEdge_ptr& he );
bool isBorder( const FullEdge_ptr& fe );
bool isBorder( const Face_ptr&     f  );

} // namespace Core
} // namespace Ra

#endif // RADIUMENGINE_BORDER_OPERATION_DEFINITION_HPP
