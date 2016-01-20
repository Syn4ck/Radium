#include <Core/Mesh/DCEL/Operation/Bind.hpp>

namespace Ra {
namespace Core {

void   bind( const Vertex_ptr&   v  ) {
    /*HalfEdge_ptr it = v.HE();
    if( it != nullptr ) {
        do {
            it->setV( v );
            if( it->Prev() != nullptr ) {
                if( it->Prev()->Twin() != nullptr ) {
                    it = it->Prev()->Twin();
                } else {
                    it = nullptr;
                }
            } else {
                it = nullptr;
            }
        } while( ( it != v.HE() ) && ( it != nullptr ) );
        if( it == nullptr ) {
            it = v.HE();
            do {
                it->setV( v );
                if( it->Twin() != nullptr ) {
                    if( it->Twin()->Next() != nullptr ) {
                        it = it->Twin()->Next();
                    } else {
                        it = nullptr;
                    }
                } else {
                    it = nullptr;
                }
            } while( ( it != v.HE() ) && ( it != nullptr ) );
        }
    }*/
}

void unbind( const Vertex_ptr&   v  ) {

}

void   bind( const HalfEdge_ptr& he ) {

}

void unbind( const HalfEdge_ptr& he ) {

}

void   bind( const FullEdge_ptr& fe ) {

}

void unbind( const FullEdge_ptr& fe ) {

}

void   bind( const Face_ptr&     f  ) {

}

void unbind( const Face_ptr&     f  ) {

}

} // namespace Core
} // namespace Ra
