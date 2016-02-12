#ifndef RADIUMENGINE_EDGE_FLIPPER_DEFINITION_HPP
#define RADIUMENGINE_EDGE_FLIPPER_DEFINITION_HPP

#include <Core/Algorithm/Subdivision/FullEdge/EdgeOperation.hpp>

namespace Ra {
namespace Core {

class EdgeFlipper : public EdgeOperation {
public:
    /// CONSTRUCTOR
    EdgeFlipper( const Dcel_ptr& dcel      = nullptr,
                 const Index&    param     = Index::INVALID_IDX(),
                 const bool      verbosity = false );

    /// DESTRUCTOR
    ~EdgeFlipper();

protected:
    /// FUNCTION
    bool checkFullEdge( uint& exitStatus ) override;
    bool isProcessable( uint& exitStatus ) override;
    bool processFullEdge( uint& exitStatus ) override;

    bool isDegenerateFace() const;
    bool isDihedralAngleTooBig() const;
    bool isFaceInversion() const;
    bool isQualityDecreasing() const;
};

} // namespace Core
} // namespace Ra

#endif // RADIUMENGINE_EDGE_FLIPPER_DEFINITION_HPP
