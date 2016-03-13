#ifndef RADIUMENGINE_PLY_EXPORT_HPP
#define RADIUMENGINE_PLY_EXPORT_HPP

#include <iostream>
#include <fstream>
#include <string>
#include <Core/CoreMacros.hpp>
#include <Core/Mesh/TriangleMesh.hpp>

namespace Ra {
namespace Core {

static void ExportPLY( const TriangleMesh& mesh, const std::string& filename ) {
    std::string name = filename + ".ply";
    std::ofstream myfile( name );
    CORE_ASSERT(myfile.is_open(), "FILE PROBLEM" );

    std::string scalar_type;
#ifndef CORE_USE_DOUBLE
    scalar_type = "float";
#else
    scalar_type = "double";
#endif


    if( mesh.m_vertices.size() > 0 ) {
        // START
        myfile << "ply\n";
        myfile << "format ascii 1.0\n";

        // VERTEX HEADER
        myfile << "element vertex " << std::to_string( mesh.m_vertices.size() ) << "\n";
        myfile << "property " << scalar_type << " x\n";
        myfile << "property " << scalar_type << " y\n";
        myfile << "property " << scalar_type << " z\n";
        myfile << "property " << scalar_type << " normal_x\n";
        myfile << "property " << scalar_type << " normal_y\n";
        myfile << "property " << scalar_type << " normal_z\n";

        // FACE HEADER
        if( mesh.m_triangles.size() > 0 ) {
            myfile << "element face " << std::to_string( mesh.m_triangles.size() ) << "\n";
            myfile << "property list uchar int vertex_index\n";
        }

        // END
        myfile << "end_header";

        // Vertices
        for( uint i = 0; i < mesh.m_vertices.size(); ++i ) {
            const Vector3 v = mesh.m_vertices.at( i );
            const Vector3 n = mesh.m_normals.at( i );
            myfile << std::to_string( v[0] ) << " " <<
                      std::to_string( v[1] ) << " " <<
                      std::to_string( v[2] ) << " " <<
                      std::to_string( n[0] ) << " " <<
                      std::to_string( n[1] ) << " " <<
                      std::to_string( n[2] ) << "\n";
        }

        // Triangle
        for( uint i = 0; i < mesh.m_triangles.size(); ++i ) {
            const Triangle f = mesh.m_triangles.at( i );
            myfile << "3 " << std::to_string( f[0] ) << " " <<
                              std::to_string( f[1] ) << " " <<
                              std::to_string( f[2] ) << "\n";
        }
    }

    myfile.close();
}

} // namespace Core
} // namespace Ra

#endif // RADIUMENGINE_PLY_EXPORT_HPP
