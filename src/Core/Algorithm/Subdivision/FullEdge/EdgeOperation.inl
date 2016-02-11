#include <Core/Algorithm/Subdivision/FullEdge/EdgeOperation.hpp>

#include <Core/Mesh/DCEL/Dcel.hpp>

namespace Ra {
namespace Core {

/// DCEL
inline Dcel_ptr EdgeOperation::getDCEL() const {
    return m_dcel;
}

inline void EdgeOperation::setDCEL( const Dcel_ptr& dcel ) {
    m_dcel = dcel;
}

} // namespace Core
} // namespace Ra
