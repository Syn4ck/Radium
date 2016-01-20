#include <Core/Mesh/DCEL/Vertex.hpp>

#include <Core/Mesh/DCEL/HalfEdge.hpp>

namespace Ra {
namespace Core {



/// CONSTRUCTOR
Vertex::Vertex( const Vector3&      p,
                const Vector3&      n,
                const HalfEdge_ptr& he,
                const Index&        index ) :
    IndexedObject( index ),
    m_p( p ),
    m_n( n ),
    m_he( he ) { }



/// DESTRUCTOR
Vertex::~Vertex() { }


/*
/// BINDABLE
void Vertex::bind() {
    HalfEdge_ptr it = m_he;
    if( it != nullptr ) {
        do {
            it->setV( this );
            if( it->Prev() != nullptr ) {
                if( it->Prev()->Twin() != nullptr ) {
                    it = it->Prev()->Twin();
                } else {
                    it = nullptr;
                }
            } else {
                it = nullptr;
            }
        } while( ( it != m_he ) && ( it != nullptr ) );
        if( it == nullptr ) {
            it = m_he;
            do {
                it->setV( this );
                if( it->Twin() != nullptr ) {
                    if( it->Twin()->Next() != nullptr ) {
                        it = it->Twin()->Next();
                    } else {
                        it = nullptr;
                    }
                } else {
                    it = nullptr;
                }
            } while( ( it != m_he ) && ( it != nullptr ) );
        }
    }
}

void Vertex::unbind() {
    HalfEdge_ptr it = m_he;
    if( it != nullptr ) {
        do {
            if( it->V() == this ) {
                it->setV( nullptr );
            }
            if( it->Prev() != nullptr ) {
                if( it->Prev()->Twin() != nullptr ) {
                    it = it->Prev()->Twin();
                } else {
                    it = nullptr;
                }
            } else {
                it = nullptr;
            }
        } while( ( it != m_he ) && ( it != nullptr ) );
        if( it == nullptr ) {
            it = m_he;
            do {
                if( it->V() == this ) {
                    it->setV( nullptr );
                }
                if( it->Twin() != nullptr ) {
                    if( it->Twin()->Next() != nullptr ) {
                        it = it->Twin()->Next();
                    } else {
                        it = nullptr;
                    }
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
