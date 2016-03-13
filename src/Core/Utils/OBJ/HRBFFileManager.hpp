#ifndef RADIUMENGINE_HRBF_FILE_MANAGER_HPP
#define RADIUMENGINE_HRBF_FILE_MANAGER_HPP

#include <iostream>
#include <fstream>
#include <string>
#include <Core/CoreMacros.hpp>

// Add the include for hrbfs

namespace Ibl {

template < class Hrbf >
static void ExportHRBF( const std::string& filename, const Hrbf& data ) {
    std::string name = filename + ".hrbf";
    std::ofstream myfile( name );
    CORE_ASSERT(myfile.is_open(), "FILE PROBLEM" );

    std::string scalar_type;
#ifndef CORE_USE_DOUBLE
    scalar_type = "float";
#else
    scalar_type = "double";
#endif


    if( data.m_vertices.size() > 0 ) {
        // START
        myfile << "hrbf\n";
        myfile << "format ascii 1.0\n";

        // SAMPLE HEADER
        myfile << "element sample " << std::to_string( data.m_vertices.size() ) << "\n";
        myfile << "property " << scalar_type << " x\n";
        myfile << "property " << scalar_type << " y\n";
        myfile << "property " << scalar_type << " z\n";
        myfile << "property " << scalar_type << " normal_x\n";
        myfile << "property " << scalar_type << " normal_y\n";
        myfile << "property " << scalar_type << " normal_z\n";
        myfile << "property " << scalar_type << " alpha\n";
        myfile << "property " << scalar_type << " beta\n";

        // END
        myfile << "end_header";

        // Sample
        for( uint i = 0; i < data.m_vertices.size(); ++i ) {
            const Vector3 p;// = data.m_points.at( i );    // Add a way to store the hrbf points positions
            const Vector3 n;// = data.m_normals.at( i );   // Add a way to store the hrbf points normals
            const Scalar  a;// = data.m_alphas.at( i );    // Add a way to store the hrbf points alphas
            const Scalar  b;// = data.m_betas.at( i );     // Add a way to store the hrbf points betas
            myfile << std::to_string( p[0] ) << " " <<
                      std::to_string( p[1] ) << " " <<
                      std::to_string( p[2] ) << " " <<
                      std::to_string( n[0] ) << " " <<
                      std::to_string( n[1] ) << " " <<
                      std::to_string( n[2] ) << " " <<
                      std::to_string( a    ) << " " <<
                      std::to_string( b    ) << "\n";
        }
    }

    myfile.close();
}






template < class Hrbf >
static void ImportHRBF( const std::string& filename, Hrbf& data ) {
    std::ifstream myfile( name );
    CORE_ASSERT(myfile.is_open(), "FILE PROBLEM" );

    /*
      I'm assuming the file doesn't contains blank lines or comments.
      The format should be exactly the same defined in the exporter function.
      The format I used is close to the PLY file format, that actually allows to
      add all the data we want, specifying the format and the types.
     */

    std::string header;
    std::string line;

    myfile >> header;

    // Header should be "hrbf"
    if( header != "hrbf" ) {
        myfile.close();
        return;
    }

    // Useless line, left there just to have the format closer to the PLY one
    // This line contains the format of the file we are currently reading.
    // I'm assuming is a text file.
    line = myfile.getline();

    std::string keyword;
    std::string type;
    uint        size;

    myfile >> keyword >> type >> size;

    // If there is something wrong abort.
    if( ( keyword != "element" ) || ( type != "sample" ) || ( size == 0 ) ) {
        myfile.close();
        return;
    }

    // Skipping the lines that are telling us how a single line is composed.
    // One instead should check them, item by item and create a proper parser.
    // For now I'm assuming that each hrbf has the format:
    // x y z nx ny nz alpha beta
    for( uint i = 0; i < 8; ++i ) {
        line = myfile.getline();
    }


    //Check the header closing tag
    myfile >> keyword;
    if( keyword != "end_header" ) {
        myfile.close();
        return;
    }

    //data.resize( size ); // Resize the hrbf in order to contain the file data
    for( uint i = 0; i < size; ++i ) {
        Ra::Core::Vector3 p;
        Ra::Core::Vector3 n;
        Scalar            a;
        Scalar            b;
        file >> p[0] >> p[1] >> p[2] >> n[0] >> n[1] >> n[2] >> a >> b;

        // data.m_points[i] = p; // Add a way to store the hrbf points positions
        //data.m_normals[i] = n; // Add a way to store the hrbf points normals
        // data.m_alphas[i] = a; // Add a way to store the hrbf points alphas
        //  data.m_betas[i] = b; // Add a way to store the hrbf points betas
    }

    myfile.close();
}


} // namespace Ibl

#endif // RADIUMENGINE_HRBF_FILE_MANAGER_HPP
