#ifndef RADIUMENGINE_BK2004_ALGORITHM_DEFINITION_HPP
#define RADIUMENGINE_BK2004_ALGORITHM_DEFINITION_HPP

#include <set>
#include <utility>

#include <Core/Index/Index.hpp>
#include <Core/Mesh/TriangleMesh.hpp>
#include <Core/Mesh/DCEL/Definition.hpp>
#include <Core/Algorithm/Algorithm.hpp>

namespace Ra {
namespace Core {

struct BK2004Parameter {
    BK2004Parameter( const uint   algorithmIteration = 5,
                     const uint   smoothingIteration = 10,
                     const Scalar longScale          = ( 4.0 / 3.0 ),
                     const Scalar shortScale         = ( 4.0 / 5.0 ),
                     const Scalar lambdaFactor       = 0.5 );

    uint   m_algorithmIteration;
    uint   m_smoothingIteration;
    Scalar m_longScale;
    Scalar m_shortScale;
    Scalar m_lambda;
};



class BK2004 : public Algorithm< BK2004Parameter > {
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
    BK2004( const Dcel_ptr&        dcel      = nullptr,
            const BK2004Parameter& param     = BK2004Parameter(),
            const bool             verbosity = false );

    /// DESTRUCTOR
    ~BK2004();

    /// DCEL
    inline Dcel_ptr getDCEL() const;
    inline void setDCEL( const Dcel_ptr& dcel );

    /// TRIANGLE MESH
    inline TriangleMesh& getLastMesh();
    inline Dcel_ptr      getLastDCEL();

    /// TARGET LENGTH
    inline Scalar getTargetLength() const;

protected:
    /// ALGORITHM STAGE
    bool    configCheck( uint& exitStatus ) override;
    bool  preprocessing( uint& exitStatus ) override;
    bool     processing( uint& exitStatus ) override;
    bool postprocessing( uint& exitStatus ) override;

    /// FUNCTION
    void extractIndexList( std::vector< Index >& list ) const;
    bool split( uint& exitStatus );
    bool collapse( uint& exitStatus );
    bool flip( uint& exitStatus );
    bool smoothing( uint& exitStatus );



    /// VARIABLE
    Dcel_ptr           m_dcel;
    TriangleMesh       m_prevMesh;
    Dcel_ptr           m_prevDCEL;
    Scalar             m_targetLength;

    std::set< std::pair< Scalar, Index > > m_splitList;
    std::set< std::pair< Scalar, Index > > m_collapseList;
};

} // namespace Core
} // namespace Ra

#include <Core/Algorithm/Subdivision/BK2004/BK2004.inl>

#endif // RADIUMENGINE_BK2004_ALGORITHM_DEFINITION_HPP
