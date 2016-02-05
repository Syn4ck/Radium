#ifndef RADIUMENGINE_DCEL_VERTEX_FACE_ITERATOR_HPP
#define RADIUMENGINE_DCEL_VERTEX_FACE_ITERATOR_HPP

#include <Core/Mesh/DCEL/Iterator/Vertex/VertexIterator.hpp>

namespace Ra {
namespace Core {

class VFIterator : public VIterator< Face_ptr > {
public:
    /// CONSTRUCTOR
    inline VFIterator( const Vertex_ptr& v );
    inline VFIterator( const VFIterator& it ) = default;

    /// DESTRUCTOR
    inline ~VFIterator();

    /// LIST
    inline FaceList list() const override;

    /// SIZE
    inline uint size() const override;

    /// OPERATOR
    inline Face_ptr operator->() const override;
    inline Face_ptr operator* () const override;
    virtual VFIterator& operator++() override;
    virtual VFIterator& operator--() override;
};

} // namespace Core
} // namespace Ra

#include <Core/Mesh/DCEL/Iterator/Vertex/VFIterator.inl>

#endif // RADIUMENGINE_DCEL_VERTEX_FACE_ITERATOR_HPP
