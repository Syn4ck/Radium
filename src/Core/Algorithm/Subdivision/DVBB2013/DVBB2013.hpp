#ifndef RADIUMENGINE_DVBB2013_ALGORITHM_DEFINITION_HPP
#define RADIUMENGINE_DVBB2013_ALGORITHM_DEFINITION_HPP

#include <set>
#include <utility>

#include <Core/Index/Index.hpp>
#include <Core/Mesh/TriangleMesh.hpp>
#include <Core/Mesh/DCEL/Definition.hpp>
#include <Core/Algorithm/Algorithm.hpp>

namespace Ra {
namespace Core {

struct DVBB2013Parameter {
    DVBB2013Parameter( const uint   algorithmIteration = 6,
                       const uint   smoothingIteration = 12,
                       const Scalar longScale          = ( 4.0 / 3.0 ),
                       const Scalar shortScale         = ( 4.0 / 5.0 ),
                       const Scalar eps                = 0.001 );

    uint   m_algorithmIteration;
    uint   m_smoothingIteration;
    Scalar m_longScale;
    Scalar m_shortScale;
    Scalar m_eps;
};



class DVBB2013 : public Algorithm< DVBB2013Parameter > {
public:
    /// ERROR
    enum ErrorType {
        NO_ERROR                     = 0,
        DCEL_NULLPTR,
        SUBDIVIDER_NOT_INITIALIZED,
        DCEL_AND_SUBDIVIDER_MISMATCH,
        DCEL_NO_FULLEDGE,
        DCEL_NOT_VALID,
        DCEL_NOT_CONSISTENT,
        TARGET_LENGTH_NOT_VALID,
        FULLEDGE_NOT_SPLITTED,
        FULLEDGE_NOT_COLLAPSED,
        FULLEDGE_NOT_FLIPPED,
        SMOOTHING_FAILED
    };

    /// CONSTRUCTOR
    DVBB2013( const Dcel_ptr&        dcel      = nullptr,
            const DVBB2013Parameter& param     = DVBB2013Parameter(),
            const bool               verbosity = false );

    /// DESTRUCTOR
    ~DVBB2013();

    /// DCEL
    inline Dcel_ptr getDCEL() const;
    inline void setDCEL( const Dcel_ptr& dcel );

    /// TRIANGLE MESH
    inline TriangleMesh& getLastMesh();
    inline Dcel_ptr      getLastDCEL();

protected:
    /// ALGORITHM STAGE
    bool    configCheck( uint& exitStatus ) override;
    bool  preprocessing( uint& exitStatus ) override;
    bool     processing( uint& exitStatus ) override;
    bool postprocessing( uint& exitStatus ) override;

    /// FUNCTION
    bool split( uint& exitStatus );
    bool collapse( uint& exitStatus );
    bool flip( uint& exitStatus );
    bool smoothing( uint& exitStatus );

    /// VARIABLE
    Dcel_ptr           m_dcel;
    TriangleMesh       m_prevMesh;
    Dcel_ptr           m_prevDCEL;
};

} // namespace Core
} // namespace Ra

#include <Core/Algorithm/Subdivision/DVBB2013/DVBB2013.inl>

#endif // RADIUMENGINE_DVBB2013_ALGORITHM_DEFINITION_HPP
