#include <Core/Mesh/DCEL/Operation/Consistent.hpp>

#include <Core/Mesh/DCEL/Vertex.hpp>
#include <Core/Mesh/DCEL/HalfEdge.hpp>
#include <Core/Mesh/DCEL/FullEdge.hpp>
#include <Core/Mesh/DCEL/Face.hpp>
#include <Core/Mesh/DCEL/Dcel.hpp>

#include <Core/Mesh/DCEL/Iterator/Vertex/VHEIterator.hpp>
#include <Core/Mesh/DCEL/Iterator/Face/FHEIterator.hpp>

namespace Ra {
namespace Core {

//=====================================================================
//=====================================================================

bool isConsistent( const Vertex_ptr& v ) {
    if( v->HE() == nullptr ) {
        return false;
    }
    if( v != v->HE()->V() ) {
        return false;
    }
    VHEIterator it( v );
    const uint size = it.size();
    for( uint i = 0; i < size; ++i ) {
        if( v != it->V() ) {
            return false;
        }
        ++it;
    }
    return true;
}

//=====================================================================
//=====================================================================

bool isConsistent( const HalfEdge_ptr& he ) {
    if( he->V() == nullptr ) {
        return false;
    }

    if( he->Next() == he ) {
        return false;
    }

    if( he->Prev() == he ) {
        return false;
    }

    if( he->Twin() == he ) {
        return false;
    }

    VHEIterator vit( he->V() );
    const uint vsize = vit.size();
    bool status = false;
    for( uint i = 0; i < vsize; ++i ) {
        if( *vit == he ) {
            status = true;
            break;
        }
        ++vit;
    }
    if( !status ) {
        return false;
    }

    if( he->F() != nullptr ) {
        FHEIterator fit( he->F() );
        const uint fsize = fit.size();
        status = false;
        for( uint i = 0; i < fsize; ++i ) {
            if( *fit == he ) {
                status = true;
                break;
            }
            ++fit;
        }
        if( !status ) {
            return false;
        }
    }

    return true;
}

//=====================================================================
//=====================================================================

bool isConsistent( const FullEdge_ptr& fe ) {
    if( fe->HE( 0 ) == nullptr ) {
        return false;
    }
    if( ( fe != fe->HE( 0 )->FE() ) || ( fe != fe->HE( 1 )->FE() ) ) {
        return false;
    }
    if( fe->HE( 0 ) == fe->HE( 1 ) ) {
        return false;
    }
    return true;
}

//=====================================================================
//=====================================================================

bool isConsistent( const Face_ptr& f ) {
    if( f->HE() == nullptr ) {
        return false;
    }
    if( f != f->HE()->F() ) {
        return false;
    }

    FHEIterator it( f );
    const uint size = it.size();
    for( uint i = 0; i < size; ++i ) {
        if( f != it->F() ) {
            return false;
        }
        ++it;
    }
    it.reset();
    it += size - 1;
    if( *it != f->HE()->Prev() ) {
        return false;
    }
    return true;
}

//=====================================================================
//=====================================================================

bool isConsistent( const Dcel_ptr& dcel ) {
    /// VERTEX
    const uint v_size = dcel->m_vertex.size();
    for( uint i = 0; i < v_size; ++i ) {
        if( !isConsistent( dcel->m_vertex.at( i ) ) ) {
            return false;
        }
    }
#ifdef PTR_MATCH_CHECK
    for( uint i = 0; i < v_size; ++i ) {
        Vertex_ptr v0 = dcel->m_vertex.at( i );
        for( uint j = ( i + 1 ); j < v_size; ++j ) {
            Vertex_ptr v1 = dcel->m_vertex.at( j );
            if( v0 == v1 ) {
                return false;
            }
        }
    }
#endif

    /// HALFEDGE
    const uint he_size = dcel->m_halfedge.size();
    for( uint i = 0; i < he_size; ++i ) {
        if( !isConsistent( dcel->m_halfedge.at( i ) ) ) {
            return false;
        }
    }
#ifdef PTR_MATCH_CHECK
    for( uint i = 0; i < he_size; ++i ) {
        HalfEdge_ptr he0 = dcel->m_halfedge.at( i );
        for( uint j = ( i + 1 ); j < he_size; ++j ) {
            HalfEdge_ptr he1 = dcel->m_halfedge.at( j );
            if( he0 == he1 ) {
                return false;
            }
        }
    }
#endif

    /// FULLEDGE
    const uint fe_size = dcel->m_fulledge.size();
    for( uint i = 0; i < fe_size; ++i ) {
        if( !isConsistent( dcel->m_fulledge.at( i ) ) ) {
            return false;
        }
    }
#ifdef PTR_MATCH_CHECK
    for( uint i = 0; i < fe_size; ++i ) {
        FullEdge_ptr fe0 = dcel->m_fulledge.at( i );
        for( uint j = ( i + 1 ); j < fe_size; ++j ) {
            FullEdge_ptr fe1 = dcel->m_fulledge.at( j );
            if( fe0 == fe1 ) {
                return false;
            }
        }
    }
#endif

    /// FACE
    const uint f_size = dcel->m_face.size();
    for( uint i = 0; i < f_size; ++i ) {
        if( !isConsistent( dcel->m_face.at( i ) ) ) {
            return false;
        }
    }
#ifdef PTR_MATCH_CHECK
    for( uint i = 0; i < f_size; ++i ) {
        Face_ptr f0 = dcel->m_face.at( i );
        for( uint j = ( i + 1 ); j < f_size; ++j ) {
            Face_ptr f1 = dcel->m_face.at( j );
            if( f0 == f1 ) {
                return false;
            }
        }
    }
#endif
    return true;
}

//=====================================================================
//=====================================================================

} // namespace Core
} // namespace Ra

