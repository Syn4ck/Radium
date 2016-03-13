#ifndef RADIUMENGINE_OFF_FILE_MANAGER_HPP
#define RADIUMENGINE_OFF_FILE_MANAGER_HPP

#include <FileManager.hpp>
#include <Core/Mesh/TriangleMesh.hpp>

namespace Ra {
namespace Core {

template < typename DATA >
class OFF_FileManager : public FileManager< DATA > {
protected:
    /// INTERFACE
    inline std::string fileExtension() const override {
        return "off";
    }

    inline std::string header() const {
        return "OFF";
    }
};


class OFFTriangleMesh : public OFF_FileManager<TriangleMesh> {
protected:
    bool importData( std::ifstream& file, TriangleMesh& data ) override {
        std::string h;
        file >> h;

        if( h != header() ) {
            return false;
        }

        uint v_size;
        uint f_size;
        uint e_size;

        file >> v_size >> f_size >> e_size;

        data.clear();
        data.m_vertices.resize( v_size );

        // Vertices
        for( uint i = 0; i < v_size; ++i ) {
            Vector3 v;
            file >> v[0] >> v[1] >> v[2];
            data.m_vertices[i] = v;
        }

        // Triangle
        for( uint i = 0; i < f_size; ++i ) {
            uint side;
            Triangle f;

            file >> side;
            if( side == 3 ) {
                file >> f[0] >> f[1] >> f[2];
                data.m_triangles.push_back( f );
            }
        }

        return true;
    }

    bool exportData( std::ifstream& file, const TriangleMesh& data ) override {
        const uint v_size = data.m_vertices.size();
        const uint f_size = data.m_triangles.size();
        const uint e_size = 0;

        if( v_size == 0 ) {
            return false;
        }

        file << header() << "\n";
        file << std::to_string( v_size ) << " ";
        file << std::to_string( f_size ) << " ";
        file << std::to_string( e_size ) << "\n";

        // Vertices
        for( uint i = 0; i < v_size; ++i ) {
            const Vector3 v = data.m_vertices.at( i );
            file << std::to_string( v[0] ) << " " <<
                    std::to_string( v[1] ) << " " <<
                    std::to_string( v[2] ) << "\n";
        }

        // Triangle
        for( uint i = 0; i < f_size; ++i ) {
            const Triangle f = data.m_triangles.at( i );
            file << "3 " << std::to_string( f[0] ) << " " <<
                            std::to_string( f[1] ) << " " <<
                            std::to_string( f[2] ) << "\n";
        }
        return true;
    }
};




static void ExportOFF( const TriangleMesh& mesh, const std::string& filename ) {
    std::string name = filename + ".off";
    std::ofstream myfile( name );
    CORE_ASSERT(myfile.is_open(), "FILE PROBLEM" );

    myfile << "OFF\n";
    myfile << std::to_string( mesh.m_vertices.size() ) << " ";
    myfile << std::to_string( mesh.m_triangles.size() ) << " ";
    myfile << "0\n";

    // Vertices
    for( uint i = 0; i < mesh.m_vertices.size(); ++i ) {
        const Vector3 v = mesh.m_vertices.at( i );
        myfile << std::to_string( v[0] ) << " " <<
                  std::to_string( v[1] ) << " " <<
                  std::to_string( v[2] ) << "\n";
    }

    // Triangle
    for( uint i = 0; i < mesh.m_triangles.size(); ++i ) {
        const Triangle f = mesh.m_triangles.at( i );
        myfile << "3 " << std::to_string( f[0] ) << " " <<
                          std::to_string( f[1] ) << " " <<
                          std::to_string( f[2] ) << "\n";
    }

    myfile.close();
}

} // namespace Core
} // namespace Ra

#endif // RADIUMENGINE_OFF_EXPORT_HPP
