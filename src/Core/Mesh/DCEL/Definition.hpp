#ifndef RADIUMENGINE_DCEL_FORWARD_DEFINITION_HPP
#define RADIUMENGINE_DCEL_FORWARD_DEFINITION_HPP

#include <vector>

#include <Core/CoreMacros.hpp>

namespace Ra {
namespace Core {

#ifdef CORE_DEBUG
enum DcelComponentType {
    DCEL_UNDEFINED,
    DCEL_VERTEX,
    DCEL_HALFEDGE,
    DCEL_FULLEDGE,
    DCEL_FACE,
    DCEL_DCEL
};
#endif


/**
* Forward declaration of the DCEL classes
*/

class Vertex;
class HalfEdge;
class FullEdge;
class Face;
class Dcel;

typedef Vertex*   Vertex_ptr;
typedef HalfEdge* HalfEdge_ptr;
typedef FullEdge* FullEdge_ptr;
typedef Face*     Face_ptr;
typedef Dcel*     Dcel_ptr;

typedef std::vector< Vertex_ptr >   VertexList;
typedef std::vector< HalfEdge_ptr > HalfEdgeList;
typedef std::vector< FullEdge_ptr > FullEdgeList;
typedef std::vector< Face_ptr >     FaceList;
typedef std::vector< Dcel_ptr >     DcelList;

} // namespace Core
} // namespace Ra

#endif // RADIUMENGINE_DCEL_FORWARD_DEFINITION_HPP
