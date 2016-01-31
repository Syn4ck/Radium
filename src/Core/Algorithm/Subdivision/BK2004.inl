#include <Core/Algorithm/Subdivision/BK2004.hpp>

#include <Core/Mesh/DCEL/Dcel.hpp>

namespace Ra {
namespace Core {



/// DCEL
inline Dcel_ptr BK2004::getDCEL() const {
    return m_dcel;
}

inline void BK2004::setDCEL( const Dcel_ptr& dcel ) {
    m_dcel = dcel;
}

/// TARGET LENGTH
inline Scalar BK2004::getTargetLength() const {
    return m_targetLength;
}

/// CONFIGURED
inline bool BK2004::isConfigured( uint& exitStatus ) const override {
    bool status = ( m_dcel != nullptr );
    if( status ) {
        exitStatus = NO_ERROR;
    } else {
        exitStatus = DCEL_NULLPTR;
    }
    return status;
}



} // namespace Core
} // namespace Ra


#endif // RADIUMENGINE_REMESHING_HANDLER_DEFINITION_HPP
