#ifndef RADIUMENGINE_DCEL_VERTEX_ITERATOR_HPP
#define RADIUMENGINE_DCEL_VERTEX_ITERATOR_HPP

#include <Core/Mesh/DCEL/Iterator/Iterator.hpp>

namespace Ra {
namespace Core {

/*
template < typename OBJECT >
class VIterator : public Iterator< OBJECT > {
public:
    /// CONSTRUCTOR
    VIterator( Vertex_ptr& v );
    VIterator( const VIterator& it ) = default;

    /// DESTRUCTOR
    ~VIterator();

    /// SIZE
    inline uint size() const override;

    /// RESET
    inline void reset() override;

    /// OPERATOR
    inline VIterator& operator= ( const VIterator& it );
    inline VIterator& operator++() override;
    inline VIterator& operator--() override;
    inline bool       operator==( const VIterator& it ) const;

protected:
    /// VARIABLE
    Vertex_ptr   m_v;
};
*/

template < typename TO_OBJECT >
class VIterator : public DCEL_Iterator< Vertex_ptr, TO_OBJECT > {
public:
    /// CONSTRUCTOR
    inline VIterator( const Vertex_ptr& v );
    inline VIterator( const VIterator& it ) = default;

    /// DESTRUCTOR
    inline ~VIterator();

    /// SIZE
    virtual uint size() const override;

    /// OPERATOR
    inline VIterator& operator= ( const VIterator& it );
    virtual VIterator& operator++() override;
    virtual VIterator& operator--() override;
    inline bool       operator==( const VIterator& it ) const;
};

} // namespace Core
} // namespace Ra

#include <Core/Mesh/DCEL/Iterator/Vertex/VertexIterator.inl>

#endif // RADIUMENGINE_DCEL_VERTEX_ITERATOR_HPP
