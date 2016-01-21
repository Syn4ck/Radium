#ifndef RADIUMENGINE_DCEL_FACE_FULLEDGE_ITERATOR_HPP
#define RADIUMENGINE_DCEL_FACE_FULLEDGE_ITERATOR_HPP

#include <Core/Mesh/DCEL/Iterator/Face/FaceIterator.hpp>

namespace Ra {
namespace Core {

class FFEIterator : public FIterator< FullEdge_ptr > {
public:
    /// CONSTRUCTOR
    inline FFEIterator( const Face_ptr& f );
    inline FFEIterator( const FFEIterator& it ) = default;

    /// DESTRUCTOR
    inline ~FFEIterator();

    /// LIST
    inline FullEdgeList list() const override;

    /// OPERATOR
    inline FullEdge_ptr operator->() const override;
    inline FullEdge_ptr operator* () const override;
};

} // namespace Core
} // namespace Ra

#include <Core/Mesh/DCEL/Iterator/Face/FFEIterator.inl>

#endif // RADIUMENGINE_DCEL_FACE_ITERATOR_HPP
