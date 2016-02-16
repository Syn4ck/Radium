#ifndef RADIUMENGINE_EDGE_COLLAPSER_DEFINITION_HPP
#define RADIUMENGINE_EDGE_COLLAPSER_DEFINITION_HPP

#include <Core/Algorithm/Subdivision/FullEdge/EdgeOperation.hpp>

namespace Ra {
namespace Core {

class EdgeCollapser : public EdgeOperation {
public:
    /// CONSTRUCTOR
    EdgeCollapser( const Dcel_ptr& dcel      = nullptr,
                   const Index&    param     = Index::INVALID_IDX(),
                   const bool      verbosity = false );

    /// DESTRUCTOR
    ~EdgeCollapser();

protected:
    /// FUNCTION
    bool checkFullEdge( uint& exitStatus ) override;
    bool isProcessable( uint& exitStatus ) override;
    bool processFullEdge( uint& exitStatus ) override;
    bool collapseFace( const Face_ptr& ptr, uint& exitStatus );

    bool oneRingIntersection( const FullEdge_ptr& ptr );
    bool faceInversion( const FullEdge_ptr& ptr );
};

} // namespace Core
} // namespace Ra

#endif // RADIUMENGINE_EDGE_COLLAPSER_DEFINITION_HPP
