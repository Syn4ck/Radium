#ifndef RADIUMENGINE_EDGE_SPLITTER_DEFINITION_HPP
#define RADIUMENGINE_EDGE_SPLITTER_DEFINITION_HPP

#include <Core/Algorithm/Subdivision/FullEdge/EdgeOperation.hpp>

namespace Ra {
namespace Core {

class EdgeSplitter : public EdgeOperation {
public:
    /// CONSTRUCTOR
    EdgeSplitter( const Dcel_ptr& dcel      = nullptr,
                  const Index&    param     = Index::INVALID_IDX(),
                  const bool      verbosity = false );

    /// DESTRUCTOR
    ~EdgeSplitter();

protected:
    /// FUNCTION
    bool checkFullEdge( uint& exitStatus ) override;
    bool isProcessable( uint& exitStatus ) override;
    bool processFullEdge( uint& exitStatus ) override;
    bool splitFace( const Face_ptr& ptr, uint& exitStatus );
};

} // namespace Core
} // namespace Ra

#endif // RADIUMENGINE_EDGE_SPLITTER_DEFINITION_HPP
