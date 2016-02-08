#include <Core/Algorithm/Subdivision/FullEdge/EdgeCollapser.hpp>

#include <Core/Mesh/DCEL/Dcel.hpp>

namespace Ra {
namespace Core {

/// DCEL
inline Dcel_ptr EdgeCollapser::getDCEL() const {
    return m_dcel;
}

inline void EdgeCollapser::setDCEL( const Dcel_ptr& dcel ) {
    m_dcel = dcel;
}

} // namespace Core
} // namespace Ra
