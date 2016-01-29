#include <Core/Mesh/DCEL/Operation/Bind.hpp>

#include <Core/Mesh/DCEL/Vertex.hpp>
#include <Core/Mesh/DCEL/HalfEdge.hpp>
#include <Core/Mesh/DCEL/FullEdge.hpp>
#include <Core/Mesh/DCEL/Face.hpp>

#include <Core/Mesh/DCEL/Iterator/Vertex/VHEIterator.hpp>
#include <Core/Mesh/DCEL/Iterator/Face/FHEIterator.hpp>

namespace Ra {
namespace Core {

/// BIND
void bind( const Vertex_ptr& v ) {
    VHEIterator it( v );
    if( ( *it ) != nullptr ) {
        const uint size = it.size();
        for( uint i = 0; i < size; ++i ) {
            it->setV( v );
            ++it;
        }
    }
}

void bind( const HalfEdge_ptr& he ) {
    if( he->V() != nullptr ) {
        he->V()->setHE( he );
    }

    if( he->Next() != nullptr ) {
        he->Next()->setPrev( he );
    }


    if( he->Prev() != nullptr ) {
        he->Prev()->setNext( he );
    }

    if( he->Twin() != nullptr ) {
        he->Twin()->setTwin( he );
    }

    if( he->FE() != nullptr ) {
        he->FE()->setHE( he );
    }

    if( he->F() != nullptr ) {
        he->F()->setHE( he );
    }
}

void bind( const FullEdge_ptr& fe ) {
    if( fe->HE() != nullptr ) {
        fe->HE()->setFE( fe );
        if( fe->HE()->Twin() != nullptr ) {
            fe->HE()->Twin()->setFE( fe );
        }
    }
}

void bind( const Face_ptr& f ) {
    FHEIterator it( f );
    if( ( *it ) != nullptr ) {
        const uint size = it.size();
        for( uint i = 0; i < size; ++i ) {
            it->setF( f );
            ++it;
        }
    }
}



/// UNBIND
void unbind( Vertex_ptr& v ) {
    VHEIterator it( v );
    if( ( *it ) != nullptr ) {
        const uint size = it.size();
        for( uint i = 0; i < size; ++i ) {
            if( it->V() == v ) {
                it->setV( nullptr );
            }
            ++it;
        }
    }
    v->setHE( nullptr );
}

void unbind( HalfEdge_ptr& he ) {
    if( he->V() != nullptr ) {
        if( he->V()->HE() == he ) {
            if( he->Twin() != nullptr ) {
                he->V()->setHE( he->Twin()->Next() );
            } else {
                he->V()->setHE( nullptr );
            }
        }
    }

    if( he->Next() != nullptr ) {
        if( he->Next()->Prev() == he ) {
            he->Next()->setPrev( nullptr );
        }

        if( he->F() != nullptr ) {
            if( he->F()->HE() == he ) {
                if( he->Next()->F() == he->F() ) {
                    he->F()->setHE( he->Next() );
                    he->setF( nullptr );
                }
            }
        }
    }

    if( he->Prev() != nullptr ) {
        if( he->Prev()->Next() == he ) {
            he->Prev()->setNext( nullptr );
        }

        if( he->F() != nullptr ) {
            if( he->F()->HE() == he ) {
                if( he->Prev()->F() == he->F() ) {
                    he->F()->setHE( he->Prev() );
                    he->setF( nullptr );
                }
            }
        }
    }

    if( he->Twin() != nullptr ) {
        if( he->Twin()->Twin() == he ) {
            he->Twin()->setTwin( nullptr );

            if( he->FE() != nullptr ) {
                if( he->FE()->HE() == he ) {
                    he->FE()->setHE( he->Twin() );
                }
            }
        }
    } else {
        if( he->FE() != nullptr ) {
            if( he->FE()->HE() == he ) {
                he->FE()->setHE( nullptr );
            }
        }
    }


    if( he->F() != nullptr ) {
        if( he->F()->HE() == he ) {
            he->F()->setHE( nullptr );
        }
    }

    he->setV( nullptr );
    he->setNext( nullptr );
    he->setPrev( nullptr );
    he->setTwin( nullptr );
    he->setFE( nullptr );
    he->setF( nullptr );
}

void unbind( FullEdge_ptr& fe ) {
    if( fe->HE() != nullptr ) {
        if( fe->HE()->FE() == fe ) {
            fe->HE()->setFE( nullptr );
        }
        if( fe->HE()->Twin() != nullptr ) {
            if( fe->HE()->Twin()->FE() == fe ) {
                fe->HE()->Twin()->setFE( nullptr );
            }
        }
    }
    fe->setHE( nullptr );
}

void unbind( Face_ptr& f ) {
    FHEIterator it( f );
    if( ( *it ) != nullptr ) {
        const uint size = it.size();
        for( uint i = 0; i < size; ++i ) {
            if( it->F() == f ) {
                it->setF( nullptr );
            }
            ++it;
        }
    }
    f->setHE( nullptr );
}

} // namespace Core
} // namespace Ra
