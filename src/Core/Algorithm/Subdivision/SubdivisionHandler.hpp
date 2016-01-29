#ifndef RADIUMENGINE_SUBDIVISION_HANDLER_DEFINITION_HPP
#define RADIUMENGINE_SUBDIVISION_HANDLER_DEFINITION_HPP

#include <Core/Index/Index.hpp>
#include <Core/Mesh/DCEL/Definition.hpp>

//#define DEBUG_SPLIT
#define DEBUG_COLLAPSE
//#define DEBUG_FLIP


namespace Ra {
namespace Core {

class SubdivisionHandler {
public:
    /// CONSTRUCTOR
    SubdivisionHandler( const Dcel_ptr& dcel = nullptr );

    /// DESTRUCTOR
    ~SubdivisionHandler();

    /// DCEL
    inline Dcel_ptr getDCEL() const;
    inline void setDCEL( const Dcel_ptr& ptr );

#ifdef DEBUG_SPLIT
    /// SPLIT
    inline void splitFullEdge( const Index& index );
#endif

#ifdef DEBUG_COLLAPSE
    /// COLLAPSE
    inline void collapseFullEdge( const Index& index );
#endif

#ifdef DEBUG_FLIP
    /// FLIP
    inline void flipFullEdge( const Index& index );
#endif

protected:
#ifdef DEBUG_SPLIT
    /// SPLIT
    bool splitFullEdge( const FullEdge_ptr& ptr );
    bool splitFace( const Face_ptr& ptr );
#endif

#ifdef DEBUG_COLLAPSE
    /// COLLAPSE
    bool collapseFullEdge( const FullEdge_ptr& ptr );
    bool collapseFace( const Face_ptr& ptr );
#endif

#ifdef DEBUG_FLIP
    /// FLIP
    bool flipFullEdge( const FullEdge_ptr& ptr );
#endif

    /// VARIABLE
    Dcel_ptr m_dcel;
};

} // namespace Core
} // namespace Ra

#include <Core/Algorithm/Subdivision/SubdivisionHandler.inl>

#endif // RADIUMENGINE_SUBDIVISION_HANDLER_DEFINITION_HPP
