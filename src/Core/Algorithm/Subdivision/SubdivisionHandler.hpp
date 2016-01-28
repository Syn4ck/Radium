#ifndef RADIUMENGINE_SUBDIVISION_HANDLER_DEFINITION_HPP
#define RADIUMENGINE_SUBDIVISION_HANDLER_DEFINITION_HPP

#include <Core/Index/Index.hpp>
#include <Core/Mesh/DCEL/Definition.hpp>

namespace Ra {
namespace Core {

class SubdivisionHandler {
public:
    /// CONSTRUCTOR
    SubdivisionHandler( const Dcel_ptr& dcel = nullptr );

    /// DESTRUCTOR
    ~SubdivisionHandler();

    /// DCEL
    inline const Dcel_ptr getDCEL() const;
    inline void setDCEL( const Dcel_ptr& ptr );

    /// SPLIT
    inline void splitFullEdge( const Index& index );

    /// COLLAPSE
    inline void collapseFullEdge( const Index& index );

    /// FLIP
    inline void flipFullEdge( const Index& index );

protected:
    /// SPLIT
    bool splitFullEdge( const FullEdge_ptr& ptr );
    bool splitFace( const Face_ptr& ptr );

    /// COLLAPSE
    bool collapseFullEdge( const FullEdge_ptr& ptr );
    bool collapseFace( const Face_ptr& ptr );

    /// FLIP
    bool flipFullEdge( const FullEdge_ptr& ptr );

    /// VARIABLE
    Dcel_ptr m_dcel;
};

} // namespace Core
} // namespace Ra

#include <Core/Algorithm/Subdivision/SubdivisionHandler.inl>

#endif // RADIUMENGINE_SUBDIVISION_HANDLER_DEFINITION_HPP
