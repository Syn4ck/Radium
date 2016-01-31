#ifndef RADIUMENGINE_REMESHING_HANDLER_DEFINITION_HPP
#define RADIUMENGINE_REMESHING_HANDLER_DEFINITION_HPP

#include <Core/Mesh/DCEL/Definition.hpp>
#include <Core/Algorithm/Algorithm.hpp>
#include <Core/Algorithm/Subdivision/SubdivisionHandler.hpp>

namespace Ra {
namespace Core {

struct BK2004Parameter {
    BK2004Parameter( const uint   algorithmIteration = 1,
                     const uint   smoothingIteration = 1,
                     const Scalar longScale          = ( 4.0 / 3.0 ),
                     const Scalar shortScale         = ( 4.0 / 5.0 ),
                     const Scalar lambdaFactor       = 1.0 );

    uint   m_algorithmIteration;
    uint   m_smoothingIteration;
    Scalar m_longScale;
    Scalar m_shortScale;
    Scalar m_lambda;
};



class BK2004 : public Algorithm< BK2004Parameter > {
public:
    enum ErrorType {
        NO_ERROR                = 0,
        DCEL_NULLPTR            = 1 << 0,
        DCEL_NO_FULLEDGE        = 1 << 1,
        DCEL_NOT_VALID          = 1 << 2,
        DCEL_NOT_CONSISTENT     = 1 << 3,
        TARGET_LENGTH_NOT_VALID = 1 << 4,
        FULLEDGE_NOT_SPLITTED   = 1 << 5,
        FULLEDGE_NOT_COLLAPSED  = 1 << 6,
        FULLEDGE_NOT_FLIPPED    = 1 << 7
    };

    /// CONSTRUCTOR
    BK2004( const Dcel_ptr&        dcel      = nullptr,
            const BK2004Parameter& param     = BK2004Parameter(),
            const bool             verbosity = false );

    /// DESTRUCTOR
    ~BK2004() { }

    /// DCEL
    inline Dcel_ptr getDCEL() const;

    inline void setDCEL( const Dcel_ptr& dcel );

    /// TARGET LENGTH
    inline Scalar getTargetLength() const;

protected:
    /// CONFIGURED
    inline bool isConfigured( uint& exitStatus ) override;

    /// ALGORITHM STAGE
    bool preprocessing( uint& exitStatus ) override;
    bool   processing( uint& exitStatus ) override;

    /// FUNCTION
    void extractIndexList( std::vector< Index >& list ) const;
    bool splitNcollapse( const std::vector< Index >& list, uint& exitStatus );
    uint edgeValence( const FullEdge_ptr& fe, const bool flip ) const;
    bool flip( uint& exitStatus );
    Vector3 g( const Vertex_pr& v ) const;
    Vector3 tangentialRelaxation( const Vertex_ptr& v ) const;
    void smoothing();

    /// VARIABLE
    Dcel_ptr           m_dcel;
    Scalar             m_targetLength;
    SubdivisionHandler m_subHandler;
};

} // namespace Core
} // namespace Ra

#include <Core/Algorithm/Subdivision/BK2004.inl>

#endif // RADIUMENGINE_REMESHING_HANDLER_DEFINITION_HPP
