#include <Core/Mesh/DCEL/HalfEdge.hpp>

#include <Core/Mesh/DCEL/Vertex.hpp>
#include <Core/Mesh/DCEL/FullEdge.hpp>
#include <Core/Mesh/DCEL/Face.hpp>

namespace Ra {
namespace Core {



/// CONSTRUCTOR
HalfEdge::HalfEdge( const Vertex_ptr&   v,
                    const HalfEdge_ptr& next,
                    const HalfEdge_ptr& prev,
                    const HalfEdge_ptr& twin,
                    const FullEdge_ptr& fe,
                    const Face_ptr&     f,
                    const Index&        index ) :
    IndexedObject( index ),
    m_v( v ),
    m_next( next ),
    m_prev( prev ),
    m_twin( twin ),
    m_fe( fe ),
    m_f( f ) { }



/// DESTRUCTOR
HalfEdge::~HalfEdge() { }


/*
/// BINDABLE
void HalfEdge::bind() {
    if( m_v != nullptr ) {
        m_v->setHE( this );
    }

    if( m_next != nullptr ) {
        m_next->setPrev( this );
    }


    if( m_prev != nullptr ) {
        m_prev->setNext( this );
    }

    if( m_twin != nullptr ) {
        m_twin->setTwin( this );
    }

    if( m_fe != nullptr ) {
        m_fe->setHE( this );
    }

    if( m_f != nullptr ) {
        m_f->setHE( this );
    }
}

void HalfEdge::unbind() {
    if( m_v != nullptr ) {
        if( m_v->HE() == this ) {
            if( m_twin != nullptr ) {
                m_v->setHE( m_twin->Next() );
            } else {
                m_v->setHE( nullptr );
            }
        }
    }

    if( m_next != nullptr ) {
        if( m_next->Prev() == this ) {
            m_next->setPrev( nullptr );
        }

        if( m_f != nullptr ) {
            if( m_f->HE() == this ) {
                if( m_next->F() == m_f ) {
                    m_f->setHE( m_next );
                    m_f = nullptr;
                }
            }
        }
    }

    if( m_prev != nullptr ) {
        if( m_prev->Next() == this ) {
            m_prev->setNext( nullptr );
        }

        if( m_f != nullptr ) {
            if( m_f->HE() == this ) {
                if( m_prev->F() == m_f ) {
                    m_f->setHE( m_prev );
                    m_f = nullptr;
                }
            }
        }
    }

    if( m_twin != nullptr ) {
        if( m_twin->Twin() == this ) {
            m_twin->setTwin( nullptr );

            if( m_fe != nullptr ) {
                if( m_fe->HE() == this ) {
                    m_fe->setHE( m_twin );
                }
            }
        }
    } else {
        if( m_fe != nullptr ) {
            if( m_fe->HE() == this ) {
                m_fe->setHE( nullptr );
            }
        }
    }


    if( m_f != nullptr ) {
        if( m_f->HE() == this ) {
            m_f->setHE( nullptr );
        }
    }

    m_v    = nullptr;
    m_next = nullptr;
    m_prev = nullptr;
    m_twin = nullptr;
    m_fe   = nullptr;
    m_f    = nullptr;
}
*/


} // namespace Core
} // namespace Ra
