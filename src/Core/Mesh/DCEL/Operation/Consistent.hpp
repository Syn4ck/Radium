#ifndef RADIUMENGINE_DCEL_CONSISTENCY_OPERATION_DEFINITION_HPP
#define RADIUMENGINE_DCEL_CONSISTENCY_OPERATION_DEFINITION_HPP

#include <Core/Mesh/DCEL/Definition.hpp>

namespace Ra {
namespace Core {

bool isConsistent( const Vertex_ptr&   v    );
bool isConsistent( const HalfEdge_ptr& he,  );
bool isConsistent( const FullEdge_ptr& fe,  );
bool isConsistent( const Face_ptr&     f,   );
bool isConsistent( const Dcel_ptr&     dcel );

} // namespace Core
} // namespace Ra

#endif // RADIUMENGINE_DCEL_CONSISTENCY_OPERATION_DEFINITION_HPP
