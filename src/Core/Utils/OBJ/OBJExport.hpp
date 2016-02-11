#ifndef RADIUMENGINE_USAI_SKELETON_DEFINITION_HPP
#define RADIUMENGINE_USAI_SKELETON_DEFINITION_HPP

#include <iostream>
#include <fstream>
#include <set>
#include <map>
#include <vector>
#include <string>
#include <Core/Mesh/TriangleMesh.hpp>
#include <Core/Animation/Handle/Skeleton.hpp>

namespace Ra {
namespace Core {

static void ExportOBJ( const TriangleMesh& mesh, const std::string& filename ) {
    std::string name = filename + ".obj";
    std::ofstream myfile( name );
    CORE_ASSERT(myfile.is_open(), "FILE PROBLEM" );

    // Vertices
    for( uint i = 0; i < mesh.m_vertices.size(); ++i ) {
        const Vector3 v = mesh.m_vertices.at( i );
        myfile << "v " << std::to_string( v[0] ) << " " <<
                          std::to_string( v[1] ) << " " <<
                          std::to_string( v[2] ) << "\n";
    }

    // Normals
    for( uint i = 0; i < mesh.m_normals.size(); ++i ) {
        const Vector3 n = mesh.m_normals.at( i );
        myfile << "vn " << std::to_string( n[0] ) << " " <<
                           std::to_string( n[1] ) << " " <<
                           std::to_string( n[2] ) << "\n";
    }

    // Triangle
    for( uint i = 0; i < mesh.m_triangles.size(); ++i ) {
        const Triangle f = mesh.m_triangles.at( i );
        myfile << "f " << std::to_string( f[0] + 1 ) << "//" << std::to_string( f[0] + 1 ) << " " <<
                          std::to_string( f[1] + 1 ) << "//" << std::to_string( f[1] + 1 ) << " " <<
                          std::to_string( f[2] + 1 ) << "//" << std::to_string( f[2] + 1 ) << "\n";
    }

    myfile.close();
}

} // namespace Core
} // namespace Ra

#endif // RADIUMENGINE_USAI_SKELETON_DEFINITION_HPP
