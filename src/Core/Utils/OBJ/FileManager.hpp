#ifndef RADIUMENGINE_FILE_MANAGER_HPP
#define RADIUMENGINE_FILE_MANAGER_HPP

#include <string>
#include <iostream>
#include <fstream>

namespace Ra {
namespace Core {

template < typename DATA >
class FileManager {
public:
    /// CONSTRUCTOR
    FileManager();

    /// DESTRUCTOR
    ~FileManager();

    /// INTERFACE
    bool load( const std::string& filename, DATA& data ) {
        std::ifstream file = openFile_R( filename );
        if( !file.is_open() ) {
            return false;
        }
        const bool status = exportData( file, data );
        file.close();
        return status;
    }

    bool save( const std::string& filename, const DATA& data ) {
        std::ofstream file = openFile_W( filename );
        if( !file.is_open() ) {
            return false;
        }
        const bool status = exportData( file, data );
        file.close();
        return status;
    }

protected:
    /// INTERFACE
    virtual std::string fileExtension() const = 0;
    virtual bool importData( std::ifstream& file, DATA& data ) = 0;
    virtual bool exportData( std::ifstream& file, const DATA& data ) = 0;

private:
    /// FILE
    std::ifstream openFile_R( const std::string& filename ) {
        // check estension?
        std::string name = filename;
        std::ifstream file( name );
        return file;
    }

    std::ofstream openFile_W( const std::string& filename ) {
        std::string name = filename + "." + fileExtension();
        std::ofstream file( name );
        return file;
    }
};

} // namespace Core
} // namespace Ra

#endif // RADIUMENGINE_FILE_MANAGER_HPP
