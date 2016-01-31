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
                     const Scalar lambdaFactor       = 1.0 );

    uint   m_algorithmIteration;
    uint   m_smoothingIteration;
    Scalar m_lambda;
};



class BK2004 : public Algorithm< BK2004Parameter > {
public:
    enum ErrorType {
        NO_ERROR = 0,
        DCEL_NULLPTR,
        DCEL_NO_FULLEDGE,
        DCEL_NOT_VALID,
        DCEL_NOT_CONSISTENT,
        TARGET_LENGTH_NOT_VALID,
        FULLEDGE_NOT_SPLITTED,
        FULLEDGE_NOT_COLLAPSED,
        FULLEDGE_NOT_FLIPPED
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
    inline bool isConfigured( uint& exitStatus ) const override;

    /// ALGORITHM STAGE
    bool preprocessing( uint& exitStatus ) override;
    bool   processing( uint& exitStatus ) override;

    /// VARIABLE
    Dcel_ptr           m_dcel;
    Scalar             m_targetLength;
    SubdivisionHandler m_subHandler;
};

} // namespace Core
} // namespace Ra

#include <Core/Algorithm/Subdivision/BK2004.inl>

#endif // RADIUMENGINE_REMESHING_HANDLER_DEFINITION_HPP
