#include <Core/Mesh/DCEL/Operation/Consistency.hpp>

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
    if( v->HE() == nullptr ) return false;
    if( v != v->HE()->V() ) return false;
    VHEIterator it( v );
    const uint size = it.size();
    for( uint i = 0; i < size; ++i ) {
        if( v != it->V() ) return false;
        ++it;
    }
    return true;
}

//=====================================================================
//=====================================================================

bool isConsistent( const HalfEdge_ptr& he ) {
    if( he->V() == nullptr ) return false;

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
    if( !status ) return false;

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
        if( !status ) return false;
    }

    return true;
}

//=====================================================================
//=====================================================================

bool isConsistent( const FullEdge_ptr& fe ) {
    if( fe->HE( 0 ) == nullptr ) return false;
    if( ( fe != fe->HE( 0 )->FE() ) || ( fe != fe->HE( 1 )->FE() ) ) return false;
    return true;
}

//=====================================================================
//=====================================================================

bool isConsistent( const Face_ptr& f ) {
    if( f->HE() == nullptr ) return false;
    if( f != f->HE()->F() ) return false;

    FHEIterator it( f );
    const uint size = it.size();
    for( uint i = 0; i < size; ++i ) {
        if( f != it->F() ) return false;
        ++it;
    }
    it.reset();
    it += size - 1;
    if( *it == f->HE()->Prev() ) return false;
    return true;s
}

//=====================================================================
//=====================================================================

bool isConsistent( const Dcel_ptr& dcel ) {
    /// VERTEX
    const uint v_size = dcel->m_vertex.size();
    for( uint i = 0; i < v_size; ++i ) {
        if( !isConsistent( dcel->m_vertex.at( i ) ) ) return false;
    }

    /// HALFEDGE
    const uint he_size = dcel->m_halfedge.size();
    for( uint i = 0; i < he_size; ++i ) {
        if( !isConsistent( dcel->m_halfedge.at( i ) ) ) return false;
    }

    /// FULLEDGE
    const uint fe_size = dcel->m_fulledge.size();
    for( uint i = 0; i < fe_size; ++i ) {
        if( !isConsistent( dcel->m_fulledge.at( i ) ) ) return false;
    }

    /// FACE
    const uint f_size = dcel->m_face.size();
    for( uint i = 0; i < f_size; ++i ) {
        if( !isConsistent( dcel->m_face.at( i ) ) ) return false;
    }
    return true;
}

//=====================================================================
//=====================================================================

} // namespace Core
} // namespace Ra

