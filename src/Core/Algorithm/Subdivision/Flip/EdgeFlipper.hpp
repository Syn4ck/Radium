#ifndef RADIUMENGINE_EDGE_FLIPPER_DEFINITION_HPP
#define RADIUMENGINE_EDGE_FLIPPER_DEFINITION_HPP

#include <Core/Index/Index.hpp>
#include <Core/Algorithm/Algorithm.hpp>
#include <Core/Mesh/DCEL/Definition.hpp>

namespace Ra {
namespace Core {

class EdgeFlipper : public Algorithm<Index> {
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
        VERTEX_NOT_INSERTED,
        HALFEDGE_NOT_INSERTED,
        FULLEDGE_NOT_INSERTED,
        FACE_NOT_INSERTED,
        BORDER_FULLEDGE,
        DIHEDRAL_ANGLE_TOO_LARGE,
        INVERSION_OCCURS,
        DEGENERATE_FULLEDGE,
        DEGENERATE_FACE
    };

    /// CONSTRUCTOR
    EdgeFlipper( const Dcel_ptr& dcel      = nullptr,
                 const Index&    param     = Index::INVALID_IDX(),
                 const bool      verbosity = false );

    /// DESTRUCTOR
    ~EdgeFlipper();

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
    bool isFlippable( uint& exitStatus );
    bool flipFullEdge( uint& exitStatus );

    /// VARIABLE
    Dcel_ptr     m_dcel;
    FullEdge_ptr m_fe;
};

} // namespace Core
} // namespace Ra

#include <Core/Algorithm/Subdivision/Flip/EdgeFlipper.inl>

#endif // RADIUMENGINE_EDGE_FLIPPER_DEFINITION_HPP
