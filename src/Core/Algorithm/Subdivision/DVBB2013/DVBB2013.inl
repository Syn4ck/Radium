#include <Core/Algorithm/Subdivision/DVBB2013/DVBB2013.hpp>

#include <Core/Mesh/DCEL/Dcel.hpp>

namespace Ra {
namespace Core {



/// DCEL
inline Dcel_ptr DVBB2013::getDCEL() const {
    return m_dcel;
}

inline void DVBB2013::setDCEL( const Dcel_ptr& dcel ) {
    m_dcel = dcel;
}

/// TRIANGLE MESH
inline TriangleMesh& DVBB2013::getLastMesh() {
    return m_prevMesh;
}

inline Dcel_ptr DVBB2013::getLastDCEL() {
    return m_prevDCEL;
}



} // namespace Core
} // namespace Ra

