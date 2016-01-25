#include <Core/Mesh/DCEL/Operation/Check.hpp>

#include <Core/Mesh/DCEL/Vertex.hpp>
#include <Core/Mesh/DCEL/HalfEdge.hpp>
#include <Core/Mesh/DCEL/FullEdge.hpp>
#include <Core/Mesh/DCEL/Face.hpp>
#include <Core/Mesh/DCEL/Dcel.hpp>

#include <Core/Log/Log.hpp>

namespace Ra {
namespace Core {

//=====================================================================
//=====================================================================

void check( const Vertex_ptr& v, const uint flag ) {
#ifdef CORE_DEBUG

    if( flag & DCEL_CHECK_VERBOSE ) {
        LOG( logDEBUG ) << "Checking vertex...";
    }

    /// INDEX
    if( flag & DCEL_CHECK_INDEX ) {
        CORE_ASSERT( v->idx.isValid(), "Index is invalid." );
    }

    /// POSITION
    if( flag & DCEL_CHECK_VERTEX_POSITION ) {
        CORE_ASSERT( !v->P().hasNaN(), "Position contains nan." );
        CORE_ASSERT( v->P().allFinite(), "Position contains inf." );
    }

    /// NORMAL
    if( flag & DCEL_CHECK_VERTEX_NORMAL ) {
        CORE_ASSERT( !v->N().hasNaN(), "Position contains nan." );
        CORE_ASSERT( v->N().allFinite(), "Position contains inf." );
    }

    /// HALFEDGE
    if( flag & DCEL_CHECK_HALFEDGE_PTR ) {
        CORE_ASSERT( ( v->HE() != nullptr ), "HalfEdge is nullptr." );
    }

    if( flag & DCEL_CHECK_VERBOSE ) {
        LOG( logDEBUG ) << "Checking vertex done.";
    }
#endif
}

//=====================================================================
//=====================================================================

void check( const HalfEdge_ptr& he, const uint flag ) {
#ifdef CORE_DEBUG
    if( flag & DCEL_CHECK_VERBOSE ) {
        LOG( logDEBUG ) << "Checking halfedge...";
    }

    /// INDEX
    if( flag & DCEL_CHECK_INDEX ) {
        CORE_ASSERT( he->idx.isValid(), "Index is invalid." );
    }

    /// VERTEX
    if( flag & DCEL_CHECK_VERTEX_PTR ) {
        CORE_ASSERT( ( he->V() != nullptr ), "Vertex is nullptr." );
    }

    /// NEXT
    if( flag & DCEL_CHECK_HALFEDGE_NEXT ) {
        CORE_ASSERT( ( he->Next() != nullptr ), "Next is nullptr." );
    }

    /// PREV
    if( flag & DCEL_CHECK_HALFEDGE_PREV ) {
        CORE_ASSERT( ( he->Prev() != nullptr ), "Prev is nullptr." );
    }

    /// TWIN
    if( flag & DCEL_CHECK_HALFEDGE_TWIN ) {
        CORE_ASSERT( ( he->Twin() != nullptr ), "Twin is nullptr." );
    }

    /// FULLEDGE
    if( flag & DCEL_CHECK_FULLEDGE_PTR ) {
        CORE_ASSERT( ( he->FE() != nullptr ), "FullEdge is nullptr." );
    }

    /// FACE
    if( flag & DCEL_CHECK_FACE_PTR ) {
        CORE_ASSERT( ( he->F() != nullptr ), "Face is nullptr." );
    }

    if( flag & DCEL_CHECK_VERBOSE ) {
        LOG( logDEBUG ) << "Checking halfedge done.";
    }
#endif
}

//=====================================================================
//=====================================================================

void check( const FullEdge_ptr& fe, const uint flag ) {
#ifdef CORE_DEBUG
    if( flag & DCEL_CHECK_VERBOSE ) {
        LOG( logDEBUG ) << "Checking fullfedge...";
    }

    /// INDEX
    if( flag & DCEL_CHECK_INDEX ) {
        CORE_ASSERT( fe->idx.isValid(), "Index is invalid." );
    }

    /// HALFEDGE
    if( flag & DCEL_CHECK_HALFEDGE_PTR ) {
        CORE_ASSERT( ( fe->HE( 0 ) != nullptr ), "HalfEdge is nullptr." );
    }

    /// STATUS
    if( flag & DCEL_CHECK_FULLEDGE_STATUS ) {
        CORE_ASSERT( ( fe->HE( 1 ) != nullptr ), "FullEdge is corrupted." );
    }

    if( flag & DCEL_CHECK_VERBOSE ) {
        LOG( logDEBUG ) << "Checking fullfedge done.";
    }
#endif
}

//=====================================================================
//=====================================================================

void check( const Face_ptr& f, const uint flag ) {
#ifdef CORE_DEBUG
    if( flag & DCEL_CHECK_VERBOSE ) {
        LOG( logDEBUG ) << "Checking face...";
    }

    /// INDEX
    if( flag & DCEL_CHECK_INDEX ) {
        CORE_ASSERT( f->idx.isValid(), "Index is invalid." );
    }

    /// HALFEDGE
    if( flag & DCEL_CHECK_HALFEDGE_PTR ) {
        CORE_ASSERT( ( f->HE() != nullptr ), "HalfEdge is nullptr." );
    }

    if( flag & DCEL_CHECK_VERBOSE ) {
        LOG( logDEBUG ) << "Checking face done.";
    }
#endif
}

//=====================================================================
//=====================================================================

void check( const Dcel_ptr& dcel, const uint flag ) {
#ifdef CORE_DEBUG
    if( flag & DCEL_CHECK_VERBOSE ) {
        LOG( logDEBUG ) << "Checking DCEL...";
    }

    /// INDEX
    if( flag & DCEL_CHECK_INDEX ) {
        CORE_ASSERT( dcel->idx.isValid(), "Index is invalid." );
    }

    /// VERTEX
    const uint v_size = dcel->m_vertex.size();
    if( flag & DCEL_CHECK_VERTEX_PTR ) {
        if( flag & DCEL_CHECK_VERBOSE ) {
            LOG( logDEBUG ) << "Checking vertex list...";
        }
        for( uint i = 0; i < v_size; ++i ) {
            check( dcel->m_vertex.at( i ) );
        }
        if( flag & DCEL_CHECK_VERBOSE ) {
            LOG( logDEBUG ) << "Checking vertex list done.";
        }
    }

    /// HALFEDGE
    const uint he_size = dcel->m_halfedge.size();
    if( flag & DCEL_CHECK_HALFEDGE_PTR ) {
        if( flag & DCEL_CHECK_VERBOSE ) {
            LOG( logDEBUG ) << "Checking halfedge list...";
        }
        for( uint i = 0; i < he_size; ++i ) {
            check( dcel->m_halfedge.at( i ) );
        }
        if( flag & DCEL_CHECK_VERBOSE ) {
            LOG( logDEBUG ) << "Checking halfedge list done.";
        }
    }

    /// FULLEDGE
    const uint fe_size = dcel->m_fulledge.size();
    if( flag & DCEL_CHECK_FULLEDGE_PTR ) {
        if( flag & DCEL_CHECK_VERBOSE ) {
            LOG( logDEBUG ) << "Checking fulledge list...";
        }

        CORE_ASSERT( ( fe_size == ( he_size / 2 ) ), "Fulledges or halfedges missing." );

        for( uint i = 0; i < fe_size; ++i ) {
            check( dcel->m_fulledge.at( i ) );
        }
        if( flag & DCEL_CHECK_VERBOSE ) {
            LOG( logDEBUG ) << "Checking fulledge list done.";
        }
    }

    /// FACE
    const uint f_size = dcel->m_face.size();
    if( flag & DCEL_CHECK_FACE_PTR ) {
        if( flag & DCEL_CHECK_VERBOSE ) {
            LOG( logDEBUG ) << "Checking face list...";
        }
        for( uint i = 0; i < f_size; ++i ) {
            check( dcel->m_face.at( i ) );
        }
        if( flag & DCEL_CHECK_VERBOSE ) {
            LOG( logDEBUG ) << "Checking face list done.";
        }
    }

    if( flag & DCEL_CHECK_VERBOSE ) {
        LOG( logDEBUG ) << "Checking DCEL done.";
    }
#endif
}

//=====================================================================
//=====================================================================

} // namespace Core
} // namespace Ra

