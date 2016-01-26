#ifndef RADIUMENGINE_DCEL_FLAG_DEFINITION_HPP
#define RADIUMENGINE_DCEL_FLAG_DEFINITION_HPP

#include <Core/CoreMacros.hpp>

namespace Ra {
namespace Core {

class PropertyContainer {
public:
    /// ENUM
    enum PropertyType {
        UNSET     = 0,
        SELECTED  = 1 << 0,
        VISIBLE   = 1 << 1,
        PROCESS   = 1 << 2,
        PROCESSED = 1 << 3,
        NEW       = 1 << 4,
        READY     = 1 << 5

    };

    /// CONSTRUCTOR
    PropertyContainer() : m_state( UNSET ) { }

    /// DESTRUCTOR
    ~PropertyContainer() { }

    /// QUERY
    inline bool isProperty( const uint state ) const;
    inline bool isSelected()  const;
    inline bool isVisible()   const;
    inline bool isProcess()   const;
    inline bool isProcessed() const;
    inline bool isNew()       const;
    inline bool isReady()     const;

    /// PROPERTY
    inline void setProperty ( const uint state );
    inline void setSelected ( const bool state );
    inline void setVisible  ( const bool state );
    inline void setProcess  ( const bool state );
    inline void setProcessed( const bool state );
    inline void setNew      ( const bool state );
    inline void setReady    ( const bool state );

protected:
    /// VARIABLE
    uint m_state;
};

} // namespace Core
} // namespace Ra

#include <Core/Mesh/DCEL/Flag.inl>

#endif // RADIUMENGINE_DCEL_FLAG_DEFINITION_HPP
