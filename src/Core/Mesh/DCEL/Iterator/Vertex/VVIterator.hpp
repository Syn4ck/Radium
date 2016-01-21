#ifndef RADIUMENGINE_DCEL_VERTEX_VERTEX_ITERATOR_HPP
#define RADIUMENGINE_DCEL_VERTEX_VERTEX_ITERATOR_HPP

#include <Core/Mesh/DCEL/Iterator/Vertex/VertexIterator.hpp>

namespace Ra {
namespace Core {

class VVIterator : public VIterator< Vertex_ptr > {
public:
    /// CONSTRUCTOR
    inline VVIterator( const Vertex_ptr& v );
    inline VVIterator( const VVIterator& it ) = default;

    /// DESTRUCTOR
    inline ~VVIterator();

    /// LIST
    inline VertexList list() const override;

    /// OPERATOR
    inline Vertex_ptr operator->() const override;
    inline Vertex_ptr operator* () const override;
};

} // namespace Core
} // namespace Ra

#include <Core/Mesh/DCEL/Iterator/Vertex/VVIterator.inl>

#endif // RADIUMENGINE_DCEL_VERTEX_VERTEX_ITERATOR_HPP
