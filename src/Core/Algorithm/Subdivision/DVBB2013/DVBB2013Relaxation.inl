#include <Core/Algorithm/Subdivision/DVBB2013/DVBB2013Relaxation.hpp>

namespace Ra {
namespace Core {

/// MESH
inline TriangleMesh* DVBB2013Relaxation::getMesh() const {
    return m_mesh;
}

inline void DVBB2013Relaxation::setMesh( TriangleMesh* mesh ) {
    m_mesh = mesh;
}

} // namespace Core
} // namespace Ra
