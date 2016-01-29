#ifndef RADIUMENGINE_DCEL_ITERATOR_DEFINITION_HPP
#define RADIUMENGINE_DCEL_ITERATOR_DEFINITION_HPP

//#include <vector>
#include <Core/Mesh/DCEL/Definition.hpp>

namespace Ra {
namespace Core {

/*
template < typename OBJECT >
class Iterator {
public:
    /// CONSTRUCTOR
    Iterator();
    Iterator( const Iterator& it ) = default;

    /// DESTRUCTOR
    virtual ~Iterator();

    /// SIZE
    virtual uint size() const = 0;

    /// LIST
    virtual std::vector< OBJECT > list() const = 0;

    /// RESET
    virtual void reset() = 0;

    /// OPERATOR
    virtual OBJECT    operator->() const = 0;
    virtual Iterator& operator++() = 0;
    virtual Iterator& operator--() = 0;
    inline  Iterator& operator+=( const uint n ) const;
    inline  Iterator& operator-=( const uint n ) const;

protected:
    /// VARIABLE
    HalfEdge_ptr m_he;
};
*/


template < typename FROM_OBJECT, typename TO_OBJECT >
class DCEL_Iterator {
public:
    /// CONSTRUCTOR
    DCEL_Iterator( const FROM_OBJECT& object );
    DCEL_Iterator( const DCEL_Iterator& it ) = default;

    /// DESTRUCTOR
    virtual ~DCEL_Iterator();

    /// SIZE
    virtual uint size() const = 0;

    /// LIST
    virtual std::vector< TO_OBJECT > list() const = 0;

    /// RESET
    inline void reset();

    /// OPERATOR
    virtual TO_OBJECT      operator->() const = 0;
    virtual TO_OBJECT      operator* () const = 0;
    virtual DCEL_Iterator& operator++() = 0;
    virtual DCEL_Iterator& operator--() = 0;
    inline  DCEL_Iterator& operator+=( const uint n );
    inline  DCEL_Iterator& operator-=( const uint n );

protected:
    /// VARIABLE
    FROM_OBJECT  m_object;
    HalfEdge_ptr m_he;
};



} // namespace Core
} // namespace Ra

#include <Core/Mesh/DCEL/Iterator/Iterator.inl>

#endif // RADIUMENGINE_DCEL_ITERATOR_DEFINITION_HPP
