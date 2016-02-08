#include <Core/Algorithm/Subdivision/FullEdge/EdgeSplitter.hpp>

#include <Core/Mesh/DCEL/Dcel.hpp>

namespace Ra {
namespace Core {

/// DCEL
inline Dcel_ptr EdgeSplitter::getDCEL() const {
    return m_dcel;
}

inline void EdgeSplitter::setDCEL( const Dcel_ptr& dcel ) {
    m_dcel = dcel;
}

} // namespace Core
} // namespace Ra
