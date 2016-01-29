#ifndef RADIUMENGINE_DCEL_PRINT_OPERATION_DEFINITION_HPP
#define RADIUMENGINE_DCEL_PRINT_OPERATION_DEFINITION_HPP

#include <string>
#include <Core/Mesh/DCEL/Definition.hpp>

namespace Ra {
namespace Core {

void print( const Vertex_ptr&   v    );
void print( const HalfEdge_ptr& he   );
void print( const FullEdge_ptr& fe   );
void print( const Face_ptr&     f    );
void print( const Dcel_ptr&     dcel, const bool recursive = false );

#if 0
std::string to_string( const Vertex_ptr&   v    );
std::string to_string( const HalfEdge_ptr& he   );
std::string to_string( const FullEdge_ptr& fe   );
std::string to_string( const Face_ptr&     f    );
std::string to_string( const Dcel_ptr&     dcel );
#endif



} // namespace Core
} // namespace Ra

#endif // RADIUMENGINE_DCEL_CHECK_OPERATION_DEFINITION_HPP
