#include <Core/Mesh/DCEL/Operation/Bind.hpp>

#include <Core/Mesh/DCEL/Vertex.hpp>
#include <Core/Mesh/DCEL/HalfEdge.hpp>
#include <Core/Mesh/DCEL/FullEdge.hpp>
#include <Core/Mesh/DCEL/Face.hpp>

#include <Core/Mesh/DCEL/Iterator/Vertex/VHEIterator.hpp>
#include <Core/Mesh/DCEL/Iterator/Face/FHEIterator.hpp>

namespace Ra {
namespace Core {

void bind( const Vertex_ptr& v ) {
    /*
    HalfEdge_ptr it = v.HE();
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
    }
    */

    VHEIterator it( v );
    if( ( *it ) != nullptr ) {
        const uint size = it.size();
        for( uint i = 0; i < size; ++i ) {
            it->setV( v );
            ++it;
        }
    }
}

void unbind( Vertex_ptr& v ) {
    /*
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
    */

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

void bind( const HalfEdge_ptr& he ) {
    /*
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
    */

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

void unbind( HalfEdge_ptr& he ) {
    /*
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
    */

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

void bind( const FullEdge_ptr& fe ) {
    /*
    if( m_he != nullptr ) {
        m_he->setFE( this );
        if( m_he->Twin() != nullptr ) {
            m_he->Twin()->setFE( this );
        }
    }
    */

    if( fe->HE() != nullptr ) {
        fe->HE()->setFE( fe );
        if( fe->HE()->Twin() != nullptr ) {
            fe->HE()->Twin()->setFE( fe );
        }
    }
}

void unbind(  FullEdge_ptr& fe ) {
    /*
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
    */

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

void bind( const Face_ptr& f ) {
    /*
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
    */

    FHEIterator it( f );
    if( ( *it ) != nullptr ) {
        const uint size = it.size();
        for( uint i = 0; i < size; ++i ) {
            it->setF( f );
            ++it;
        }
    }
}

void unbind(  Face_ptr& f ) {
    /*
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
    */

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
