#include <Core/Mesh/DCEL/Face.hpp>

#include <Core/Mesh/DCEL/HalfEdge.hpp>

namespace Ra {
namespace Core {



/// CONSTRUCTOR
Face::Face( const HalfEdge_ptr& he,
            const Index&        index ) :
    IndexedObject( index ),
    m_he( he ) { }



/// DESTRUCTOR
Face::~Face() { }


/*
/// BINDABLE
void Face::bind() {
    HalfEdge_ptr it = m_he;
    if( it != nullptr ) {
        do {
            if( it->F() == this ) {
                it->setF( this );
            }
            if( it->Next() != nullptr ) {
                it = it->Next();
            } else {
                it = nullptr;
            }
        } while( ( it != m_he ) && ( it != nullptr ) );
        if( it == nullptr ) {
            it = m_he;
            do {
                if( it->F() == this ) {
                    it->setF( this );
                }
                if( it->Prev() != nullptr ) {
                    it = it->Prev();
                } else {
                    it = nullptr;
                }
            } while( ( it != m_he ) && ( it != nullptr ) );
        }
    }
}

void Face::unbind() {
    HalfEdge_ptr it = m_he;
    if( it != nullptr ) {
        do {
            if( it->F() == this ) {
                it->setF( nullptr );
            }
            if( it->Next() != nullptr ) {
                it = it->Next();
            } else {
                it = nullptr;
            }
        } while( ( it != m_he ) && ( it != nullptr ) );
        if( it == nullptr ) {
            it = m_he;
            do {
                if( it->F() == this ) {
                    it->setF( nullptr );
                }
                if( it->Prev() != nullptr ) {
                    it = it->Prev();
                } else {
                    it = nullptr;
                }
            } while( ( it != m_he ) && ( it != nullptr ) );
        }
    }
    m_he = nullptr;
}
*/


} // namespace Core
} // namespace Ra

