#include <Core/Algorithm/Subdivision/BK2004/BK2004.hpp>

#include <Core/Mesh/DCEL/Dcel.hpp>

namespace Ra {
namespace Core {



/// DCEL
inline Dcel_ptr BK2004::getDCEL() const {
    return m_dcel;
}

inline void BK2004::setDCEL( const Dcel_ptr& dcel ) {
    m_dcel = dcel;
    m_subHandler.setDCEL( m_dcel );
}

/// TARGET LENGTH
inline Scalar BK2004::getTargetLength() const {
    return m_targetLength;
}

/// ALGORITHM STAGE



} // namespace Core
} // namespace Ra

