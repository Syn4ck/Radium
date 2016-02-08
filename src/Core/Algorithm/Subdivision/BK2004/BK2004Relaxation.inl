#include <Core/Algorithm/Subdivision/BK2004/BK2004Relaxation.hpp>

namespace Ra {
namespace Core {

/// MESH
inline TriangleMesh* BK2004Relaxation::getMesh() const {
    return m_mesh;
}

inline void BK2004Relaxation::setMesh( TriangleMesh* mesh ) {
    m_mesh = mesh;
}

} // namespace Core
} // namespace Ra
