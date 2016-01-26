#ifndef RADIUMENGINE_INDEX_HPP
#define RADIUMENGINE_INDEX_HPP

#include <Core/RaCore.hpp>
#include <limits>

namespace Ra {
namespace Core {

class Index {
public:
    /// CONSTRUCTOR
    Index( const int i = __invalid() );
    Index( const Index& i ) = default;

    /// DESTRUCTOR
    ~Index() { }

    /// COPY
    inline void copy( const Index& id );

    /// VALID
    inline bool isValid() const;

    /// INVALID
    inline bool isInvalid() const;
    inline void setInvalid();

    /// STATIC
    static Index INVALID_IDX() {
        return Index( __invalid() );
    }
    static Index MAX_IDX() {
        return Index( __maxIdx() );
    }

    /// INDEX
    inline int  getValue() const;
    inline void setValue( const int i );

    /// OPERATOR
    inline Index& operator= ( const Index& id );
    inline Index& operator++();
    inline Index& operator--();
    inline        operator  int() const;
    inline Index  operator+ ( const Index& id  ) const;
    inline Index  operator+ ( const int    off ) const;
    inline Index  operator- ( const Index& id  ) const;
    inline Index  operator- ( const int    off ) const;
    inline bool   operator==( const Index& id  ) const;
    inline bool   operator!=( const Index& id  ) const;
    inline bool   operator< ( const Index& id  ) const;
    inline bool   operator<=( const Index& id  ) const;
    inline bool   operator> ( const Index& id  ) const;
    inline bool   operator>=( const Index& id  ) const;
    inline bool   operator==( const int    i   ) const;
    inline bool   operator!=( const int    i   ) const;
    inline bool   operator< ( const int    i   ) const;
    inline bool   operator<=( const int    i   ) const;
    inline bool   operator> ( const int    i   ) const;
    inline bool   operator>=( const int    i   ) const;

protected:
    /// VARIABLE
    int m_idx;

private:
    /// STATIC
    static int __invalid() { return -1; }
    static int __maxIdx()  { return std::numeric_limits<int>::max(); }
};

} // namespace Core
} // namespace Ra

#include <Core/Index/Index.inl>

#endif // RADIUMENGINE_INDEX_HPP
