#ifndef RADIUMENGINE_DCEL_EXTRACTION_DEFINITION_HPP
#define RADIUMENGINE_DCEL_EXTRACTION_DEFINITION_HPP

#include <Core/Mesh/TriangleMesh.hpp>
#include <Core/Mesh/DCEL/Definition.hpp>

namespace Ra {
namespace Core {

void extract( const Vertex_ptr& v, TriangleMesh& mesh );


void extract( const FullEdge_ptr& fe, TriangleMesh& mesh );


void extract( const Face_ptr& f, TriangleMesh& mesh );

} // namespace Core
} // namespace Ra

#endif // RADIUMENGINE_DCEL_EXTRACTION_DEFINITION_HPP
