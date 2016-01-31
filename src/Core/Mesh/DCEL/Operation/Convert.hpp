#ifndef RADIUMENGINE_MESH_CONVERT_HPP
#define RADIUMENGINE_MESH_CONVERT_HPP

#include <Core/Mesh/TriangleMesh.hpp>
#include <Core/Mesh/DCEL/Definition.hpp>

namespace Ra {
namespace Core {

void convert( const TriangleMesh& mesh, Dcel& dcel );
void convert( const Dcel& dcel, TriangleMesh& mesh );


void convert( const Vertex_ptr& v, Vector3& p );
void convert( const VertexList& list, VectorArray< Vector3 >& point );

} // namespace Core
} // namespace Ra

#endif // RADIUMENGINE_MESH_CONVERT_HPP
