#ifndef RADIUMENGINE_EDGE_OPERATION_DEFINITION_HPP
#define RADIUMENGINE_EDGE_OPERATION_DEFINITION_HPP

#include <Core/Index/Index.hpp>
#include <Core/Algorithm/Algorithm.hpp>
#include <Core/Mesh/DCEL/Definition.hpp>

namespace Ra {
namespace Core {

class EdgeOperation : public Algorithm<Index> {
public:
    /// ERROR
    enum ErrorType {
        NO_ERROR                     = 0,
        DCEL_NULLPTR,
        EDGE_NOT_PRESENT,
        BORDER_FULLEDGE,
        INVALID_INDEX,
        INVALID_VERTEX,
        INVALID_HALFEDGE,
        INVALID_FULLEDGE,
        INVALID_FACE,
        VERTEX_NOT_INSERTED,
        VERTEX_NOT_REMOVED,
        HALFEDGE_NOT_INSERTED,
        HALFEDGE_NOT_REMOVED,
        FULLEDGE_NOT_INSERTED,
        FULLEDGE_NOT_REMOVED,
        FACE_NOT_INSERTED,
        FACE_NOT_REMOVED,
        DEGENERATE_FULLEDGE,
        DEGENERATE_FACE,
        FACE_INVERSION,
        ONE_RING_INTERSECTION_PROBLEM,
        DIHEDRAL_ANGLE_TOO_LARGE,
        WORSE_QUALITY,
        NOT_MANIFOLD,
    };

    /// CONSTRUCTOR
    EdgeOperation( const std::string& name,
                   const Dcel_ptr&    dcel      = nullptr,
                   const Index&       param     = Index::INVALID_IDX(),
                   const bool         verbosity = false );

    /// DESTRUCTOR
    ~EdgeOperation();

    /// DCEL
    inline Dcel_ptr getDCEL() const;
    inline void     setDCEL( const Dcel_ptr& dcel );

protected:
    /// ALGORITHM STAGE
    bool    configCheck( uint& exitStatus ) override;
    bool  preprocessing( uint& exitStatus ) override;
    bool     processing( uint& exitStatus ) override;
    virtual bool postprocessing( uint& exitStatus ) override;

    /// FUNCTION
    virtual bool checkFullEdge( uint& exitStatus ) = 0;
    virtual bool isProcessable( uint& exitStatus ) = 0;
    virtual bool processFullEdge( uint& exitStatus ) = 0;

    /// VARIABLE
    Dcel_ptr     m_dcel;
    FullEdge_ptr m_fe;
};

} // namespace Core
} // namespace Ra

#include <Core/Algorithm/Subdivision/FullEdge/EdgeOperation.inl>

#endif // RADIUMENGINE_EDGE_OPERATION_DEFINITION_HPP
