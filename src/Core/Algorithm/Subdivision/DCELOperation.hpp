#ifndef RADIUMENGINE_DCEL_SUBDIVISION_OPERATION_HPP
#define RADIUMENGINE_DCEL_SUBDIVISION_OPERATION_HPP

#include <Core/Index/Index.hpp>
#include <Core/Mesh/DCEL/Definition.hpp>

#define DEBUG_SPLIT
//#define DEBUG_COLLAPSE
//#define DEBUG_FLIP

namespace Ra {
namespace Core {

#ifdef DEBUG_SPLIT
bool HESplit( Dcel& dcel, const Index& halfedge_id );
bool FESplit( Dcel& dcel, const Index& fulledge_id );
#endif

#ifdef DEBUG_COLLAPSE
bool HECollapse( Dcel& dcel, const Index& halfedge_id );
bool FECollapse( Dcel& dcel, const Index& fulledge_id );
#endif

#ifdef DEBUG_FLIP
bool FEFlip( Dcel& dcel, const Index& fulledge_id );
#endif

} // namespace Core
} // namespace Ra


#endif // RADIUMENGINE_DCEL_SUBDIVISION_OPERATION_HPP
