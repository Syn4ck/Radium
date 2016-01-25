#ifndef RADIUMENGINE_DCEL_VALID_DEFINITION_HPP
#define RADIUMENGINE_DCEL_VALID_DEFINITION_HPP

#include <Core/Mesh/DCEL/Definition.hpp>

namespace Ra {
namespace Core {

bool isValid( const Vertex_ptr&   v    );
bool isValid( const HalfEdge_ptr& he   );
bool isValid( const FullEdge_ptr& fe   );
bool isValid( const Face_ptr&     f    );
bool isValid( const Dcel_ptr&     dcel );

} // namespace Core
} // namespace Ra

#endif // RADIUMENGINE_DCEL_VALID_DEFINITION_HPP
