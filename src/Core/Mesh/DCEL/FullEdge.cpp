#include <Core/Mesh/DCEL/FullEdge.hpp>

#include <Core/Mesh/DCEL/HalfEdge.hpp>

namespace Ra {
namespace Core {



/// CONSTRUCTOR
FullEdge::FullEdge( const HalfEdge_ptr& he,
                    const Index&        index ) :
    IndexedObject( index ),
    m_he( he ) { }



/// DESTRUCTOR
FullEdge::~FullEdge() { }


/*
/// BINDABLE
void FullEdge::bind() {
    if( m_he != nullptr ) {
        m_he->setFE( this );
        if( m_he->Twin() != nullptr ) {
            m_he->Twin()->setFE( this );
        }
    }
}

void FullEdge::unbind() {
    if( m_he != nullptr ) {
        if( m_he->FE() == this ) {
            m_he->setFE( nullptr );
        }
        if( m_he->Twin() != nullptr ) {
            if( m_he->Twin()->FE() == this ) {
                m_he->Twin()->setFE( nullptr );
            }
        }
    }
    m_he = nullptr;
}
*/


} // namespace Core
} // namespace Ra
