#ifndef RADIUMENGINE_DCEL_CHECK_OPERATION_DEFINITION_HPP
#define RADIUMENGINE_DCEL_CHECK_OPERATION_DEFINITION_HPP

#include <Core/Mesh/DCEL/Definition.hpp>

namespace Ra {
namespace Core {

enum DCEL_CHECK_FLAG {
    DCEL_CHECK_NOP              = 0,
    DCEL_CHECK_VERBOSE          = 1 << 0,
    DCEL_CHECK_INDEX            = 1 << 1,
    DCEL_CHECK_VERTEX_PTR       = 1 << 2,
    DCEL_CHECK_HALFEDGE_PTR     = 1 << 3,
    DCEL_CHECK_FULLEDGE_PTR     = 1 << 4,
    DCEL_CHECK_FACE_PTR         = 1 << 5,
    DCEL_CHECK_VERTEX_POSITION  = 1 << 6,
    DCEL_CHECK_VERTEX_NORMAL    = 1 << 7,
    DCEL_CHECK_HALFEDGE_NEXT    = 1 << 8,
    DCEL_CHECK_HALFEDGE_PREV    = 1 << 9,
    DCEL_CHECK_HALFEDGE_TWIN    = 1 << 10,
    DCEL_CHECK_FULLEDGE_STATUS  = 1 << 11
};

void check( const Vertex_ptr&   v   , const uint flag = DCEL_CHECK_VERBOSE         |
                                                        DCEL_CHECK_INDEX           |
                                                        DCEL_CHECK_VERTEX_POSITION |
                                                        DCEL_CHECK_VERTEX_NORMAL   |
                                                        DCEL_CHECK_HALFEDGE_PTR );

void check( const HalfEdge_ptr& he  , const uint flag = DCEL_CHECK_VERBOSE       |
                                                        DCEL_CHECK_INDEX         |
                                                        DCEL_CHECK_VERTEX_PTR    |
                                                        DCEL_CHECK_HALFEDGE_NEXT |
                                                        DCEL_CHECK_HALFEDGE_PREV |
                                                        DCEL_CHECK_HALFEDGE_TWIN |
                                                        DCEL_CHECK_FULLEDGE_PTR  |
                                                        DCEL_CHECK_FACE_PTR );

void check( const FullEdge_ptr& fe  , const uint flag = DCEL_CHECK_VERBOSE          |
                                                        DCEL_CHECK_INDEX            |
                                                        DCEL_CHECK_HALFEDGE_PTR     |
                                                        DCEL_CHECK_FULLEDGE_STATUS );

void check( const Face_ptr&     f   , const uint flag = DCEL_CHECK_VERBOSE      |
                                                        DCEL_CHECK_INDEX        |
                                                        DCEL_CHECK_HALFEDGE_PTR );

void check( const Dcel_ptr&     dcel, const uint flag = DCEL_CHECK_VERBOSE      |
                                                        DCEL_CHECK_VERTEX_PTR   |
                                                        DCEL_CHECK_HALFEDGE_PTR |
                                                        DCEL_CHECK_FULLEDGE_PTR |
                                                        DCEL_CHECK_FACE_PTR );

} // namespace Core
} // namespace Ra

#endif // RADIUMENGINE_DCEL_CHECK_OPERATION_DEFINITION_HPP
