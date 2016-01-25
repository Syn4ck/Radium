#ifndef RADIUMENGINE_MESH_CONVERT_HPP
#define RADIUMENGINE_MESH_CONVERT_HPP

#include <Core/Mesh/TriangleMesh.hpp>
#include <Core/Mesh/DCEL/Definition.hpp>

namespace Ra {
namespace Core {

struct Twin {
    Twin();
    Twin( const uint i, const uint j );

    bool operator==( const Twin& twin ) const;
    bool operator< ( const Twin& twin ) const;

    uint m_id[2];
};

void convert( const TriangleMesh& mesh, Dcel& dcel );
void convert( const Dcel& dcel, TriangleMesh& mesh );


void convert( const Vertex_ptr& v, Vector3& p );
void convert( const VertexList& list, VectorArray< Vector3 >& point );

} // namespace Core
} // namespace Ra

#endif // RADIUMENGINE_MESH_CONVERT_HPP
