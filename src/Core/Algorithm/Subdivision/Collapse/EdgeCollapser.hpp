#ifndef RADIUMENGINE_EDGE_COLLAPSER_DEFINITION_HPP
#define RADIUMENGINE_EDGE_COLLAPSER_DEFINITION_HPP

#include <Core/Index/Index.hpp>
#include <Core/Algorithm/Algorithm.hpp>
#include <Core/Mesh/DCEL/Definition.hpp>

namespace Ra {
namespace Core {

class EdgeCollapser : public Algorithm<Index> {
public:
    /// ERROR
    enum ErrorType {
        NO_ERROR                     = 0,
        DCEL_NULLPTR,
        INVALID_INDEX,
        EDGE_NOT_PRESENT,
        INVALID_VERTEX,
        INVALID_HALFEDGE,
        INVALID_FULLEDGE,
        INVALID_FACE,
        VERTEX_NOT_REMOVED,
        HALFEDGE_NOT_REMOVED,
        FULLEDGE_NOT_REMOVED,
        FACE_NOT_REMOVED,
        DEGENERATE_FULLEDGE,
        BORDER_FULLEDGE,
        ONE_RING_INTERSECTION_PROBLEM,
        DEGENERATE_FACE,
        FACE_INVERSION,
        FACE_NOT_COLLAPSED
    };

    /// CONSTRUCTOR
    EdgeCollapser( const Dcel_ptr& dcel      = nullptr,
                   const Index&    param     = Index::INVALID_IDX(),
                   const bool      verbosity = false );

    /// DESTRUCTOR
    ~EdgeCollapser();

    /// DCEL
    inline Dcel_ptr getDCEL() const;
    inline void     setDCEL( const Dcel_ptr& dcel );

protected:
    /// ALGORITHM STAGE
    bool    configCheck( uint& exitStatus ) override;
    bool  preprocessing( uint& exitStatus ) override;
    bool     processing( uint& exitStatus ) override;
    bool postprocessing( uint& exitStatus ) override;

    /// FUNCTION
    bool checkFullEdge( uint& exitStatus );
    bool isCollapsable( uint& exitStatus );
    bool isOneRingIntersectionFine( const HalfEdge_ptr& ptr );
    bool collapseFullEdge( uint& exitStatus );
    bool collapseFace( const Face_ptr& ptr, uint& exitStatus );


    /// VARIABLE
    Dcel_ptr     m_dcel;
    FullEdge_ptr m_fe;
};

} // namespace Core
} // namespace Ra

#include <Core/Algorithm/Subdivision/Collapse/EdgeCollapser.inl>

#endif // RADIUMENGINE_EDGE_COLLAPSER_DEFINITION_HPP
