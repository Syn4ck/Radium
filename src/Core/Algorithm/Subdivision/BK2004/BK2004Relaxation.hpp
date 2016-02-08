#ifndef RADIUMENGINE_BK2004_RELAXATION_DEFINITION_HPP
#define RADIUMENGINE_BK2004_RELAXATION_DEFINITION_HPP

#include <Core/Mesh/TriangleMesh.hpp>
#include <Core/Algorithm/Algorithm.hpp>

namespace Ra {
namespace Core {

struct BK2004RelaxationParameters {
    BK2004RelaxationParameters( const uint   iteration = 1,
                                const Scalar lambda    = 0.5 );
    uint   m_iteration;
    Scalar m_lambda;
};

class BK2004Relaxation : public Algorithm< BK2004RelaxationParameters > {
public:
    /// ENUM
    enum ErrorType {
        NO_ERROR = 0,
        MESH_NULLPTR,
        LAMBDA_OUT_OF_RANGE,
        INVALID_POINT,
        INVALID_NORMAL,
        INVALID_CENTROID,
        INVALID_AREA
    };

    /// CONSTRUCTOR
    BK2004Relaxation( TriangleMesh*                     mesh      = nullptr,
                      const BK2004RelaxationParameters& param     = BK2004RelaxationParameters(),
                      const bool                        verbosity = false );



    /// DESTRUCTOR
    ~BK2004Relaxation();

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

#include <Core/Algorithm/Subdivision/BK2004/BK2004Relaxation.inl>

#endif // RADIUMENGINE_BK2004_RELAXATION_DEFINITION_HPP
