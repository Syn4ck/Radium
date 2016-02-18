#ifndef RADIUMENGINE_DVBB2013_RELAXATION_DEFINITION_HPP
#define RADIUMENGINE_DVBB2013_RELAXATION_DEFINITION_HPP

#include <Core/Mesh/TriangleMesh.hpp>
#include <Core/Algorithm/Algorithm.hpp>

namespace Ra {
namespace Core {

struct DVBB2013RelaxationParameters {
    DVBB2013RelaxationParameters( const uint   iteration = 1,
                                  const Scalar eps       = 0.0001 );
    uint   m_iteration;
    Scalar m_eps;
};

class DVBB2013Relaxation : public Algorithm< DVBB2013RelaxationParameters > {
public:
    /// ENUM
    enum ErrorType {
        NO_ERROR = 0,
        MESH_NULLPTR,
        INVALID_POINT,
        INVALID_NORMAL,
        INVALID_CENTROID,
        INVALID_SIZING_VALUE
    };

    /// CONSTRUCTOR
    DVBB2013Relaxation( TriangleMesh*                     mesh      = nullptr,
                      const DVBB2013RelaxationParameters& param     = DVBB2013RelaxationParameters(),
                      const bool                          verbosity = false );

    /// DESTRUCTOR
    ~DVBB2013Relaxation();

    /// MESH
    inline TriangleMesh* getMesh() const;
    inline void          setMesh( TriangleMesh* mesh );

protected:
    /// ALGORITHM STAGE
    bool    configCheck( uint& exitStatus ) override;
    bool  preprocessing( uint& exitStatus ) override;
    bool     processing( uint& exitStatus ) override;
    bool postprocessing( uint& exitStatus ) override;

    /// VARIABLE
    TriangleMesh* m_mesh;
};

} // namespace Core
} // namespace Ra

#include <Core/Algorithm/Subdivision/DVBB2013/DVBB2013Relaxation.inl>

#endif // RADIUMENGINE_DVBB2013_RELAXATION_DEFINITION_HPP
