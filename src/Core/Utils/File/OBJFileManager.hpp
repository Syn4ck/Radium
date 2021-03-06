#ifndef RADIUMENGINE_OBJ_FILE_MANAGER_HPP
#define RADIUMENGINE_OBJ_FILE_MANAGER_HPP

#include <Core/Utils/File/FileManager.hpp>
#include <Core/Mesh/TriangleMesh.hpp>

namespace Ra {
namespace Core {

class OBJFileManager : public FileManager< TriangleMesh > {
public:
    /// CONSTRUCTOR
    OBJFileManager();

    /// DESTRUCTOR
    virtual ~OBJFileManager();

protected:    
    /// INTERFACE
    virtual std::string fileExtension() const override;
    virtual bool importData( std::ifstream& file, TriangleMesh& data ) override;
    virtual bool exportData( std::ofstream& file, const TriangleMesh& data ) override;
};

} // namespace Core
} // namespace Ra

#endif // RADIUMENGINE_OBJ_FILE_MANAGER_HPP
