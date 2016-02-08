#include <Core/Algorithm/Subdivision/FullEdge/EdgeFlipper.hpp>

#include <Core/Mesh/DCEL/Dcel.hpp>

namespace Ra {
namespace Core {

/// DCEL
inline Dcel_ptr EdgeFlipper::getDCEL() const {
    return m_dcel;
}

inline void EdgeFlipper::setDCEL( const Dcel_ptr& dcel ) {
    m_dcel = dcel;
}

} // namespace Core
} // namespace Ra
