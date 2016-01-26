#include <Core/Index/Index.hpp>

namespace Ra {
namespace Core {

/// CONSTRUCTOR
inline Index::Index( const int i ) {
    m_idx = ( i < 0 ) ? __invalid() : i;
}

/// COPY
inline void Index::copy( const Index& id ) {
    m_idx = id.m_idx;
}

/// VALID
inline bool Index::isValid() const {
    return ( m_idx != __invalid() );
}

/// INVALID
inline bool Index::isInvalid() const {
    return ( m_idx == __invalid() );
}

inline void Index::setInvalid() {
    m_idx = __invalid();
}

/// INDEX
inline int Index::getValue() const {
    return m_idx;
}

inline void Index::setValue( const int i ) {
    m_idx = ( i < 0 ) ? __invalid() : i;
}

/// OPERATOR
inline Index& Index::operator=( const Index& id ) {
    m_idx = id.m_idx;
    return *this;
}

inline Index& Index::operator++() {
    ++m_idx;
    return *this;
}

inline Index& Index::operator--() {
    if( m_idx != __invalid() ) {
        --m_idx;
    }
    return *this;
}

inline Index::operator int() const {
    return m_idx;
}

inline Index Index::operator+( const Index& id ) const {
    if( isInvalid() || id.isInvalid() ) {
        return INVALID_IDX();
    }
    return ( *this + id.m_idx );
}

inline Index Index::operator+( const int off ) const {
    int i;
    i = ( ( i = m_idx + off ) < 0 ) ? __invalid(): i;
    return Index( i );
}

inline Index Index::operator-( const Index& id ) const {
    if( isInvalid() || id.isInvalid() ) {
        return INVALID_IDX();
    }
    return ( *this - id.m_idx );
}

inline Index Index::operator-( const int off ) const {
    return ( *this + (-off) );
}

inline bool Index::operator==( const Index& id ) const {
    return ( m_idx == id.m_idx );
}

inline bool Index::operator!=( const Index& id ) const {
    return !( *this == id );
}

inline bool Index::operator<( const Index& id ) const {
    if( isInvalid() || id.isInvalid() ) {
        return false;
    }
    return ( m_idx < id.m_idx );
}

inline bool Index::operator<=( const Index& id ) const {
    return ( ( *this == id ) || ( *this < id ) );
}

inline bool Index::operator>( const Index& id ) const {
    return !( *this <= id );
}

inline bool Index::operator>=( const Index& id ) const {

    return !( *this < id );
}

inline bool Index::operator==( const int i ) const {
    return ( m_idx == i );
}

inline bool Index::operator!=( const int i ) const {
    return !( *this == i );
}

inline bool Index::operator<( const int i ) const {
    return ( m_idx < i );
}

inline bool Index::operator<=( const int i ) const {
    return ( ( *this == i ) || ( *this < i ) );
}

inline bool Index::operator>( const int i ) const {
    return !( *this <= i );
}

inline bool Index::operator>=( const int i ) const {
    return ( !( *this < i ) );
}



} // namespace Core
} // namespace Ra
