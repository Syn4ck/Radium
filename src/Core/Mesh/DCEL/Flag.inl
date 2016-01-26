#include <Core/Mesh/DCEL/Flag.hpp>

namespace Ra {
namespace Core {

/// QUERY
inline bool PropertyContainer::isProperty( const uint state ) const {
    return ( m_state & state );
}

inline bool PropertyContainer::isSelected() const {
    return ( m_state & SELECTED );
}

inline bool PropertyContainer::isVisible() const {
    return ( m_state & VISIBLE );
}

inline bool PropertyContainer::isProcess() const {
    return ( m_state & PROCESS );
}

inline bool PropertyContainer::isProcessed() const {
    return ( m_state & PROCESSED );
}

inline bool PropertyContainer::isNew() const {
    return ( m_state & NEW );
}

inline bool PropertyContainer::isReady() const {
    return ( m_state & READY );
}

/// PROPERTY
inline void PropertyContainer::setProperty( const uint state ) {
    m_state = state;
}

inline void PropertyContainer::setSelected( const bool state ) {
    if( state ) {
        m_state |= SELECTED;
    } else {
        m_state &= ~SELECTED;
    }
}

inline void PropertyContainer::setVisible( const bool state ) {
    if( state ) {
        m_state |= VISIBLE;
    } else {
        m_state &= ~VISIBLE;
    }
}

inline void PropertyContainer::setProcess( const bool state ) {
    if( state ) {
        m_state |= PROCESS;
    } else {
        m_state &= ~PROCESS;
    }
}

inline void PropertyContainer::setProcessed( const bool state ) {
    if( state ) {
        m_state |= PROCESSED;
    } else {
        m_state &= ~PROCESSED;
    }
}

inline void PropertyContainer::setNew( const bool state ) {
    if( state ) {
        m_state |= NEW;
    } else {
        m_state &= ~NEW;
    }
}

inline void PropertyContainer::setReady( const bool state ) {
    if( state ) {
        m_state |= READY;
    } else {
        m_state &= ~READY;
    }
}


} // namespace Core
} // namespace Ra

