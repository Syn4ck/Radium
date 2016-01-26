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
    bool isVerbose = ( flag & DCEL_CHECK_VERBOSE ) || ( flag & DCEL_CHECK_VERBOSE_REC );


    if( isVerbose ) {
        LOG( logDEBUG ) << "Checking vertex...";
    }

    /// INDEX
    if( flag & DCEL_CHECK_INDEX ) {
        CORE_ASSERT( v->idx.isValid(), "Index is invalid." );
        if( isVerbose ) {
            LOG( logDEBUG ) << "\tIndex...\t\t[Y]";
        }
    }

    /// POSITION
    if( flag & DCEL_CHECK_VERTEX_POSITION ) {
        CORE_ASSERT( !v->P().hasNaN(), "Position contains nan." );
        CORE_ASSERT( v->P().allFinite(), "Position contains inf." );
        if( isVerbose ) {
            LOG( logDEBUG ) << "\tPosition...[Y]";
        }
    }

    /// NORMAL
    if( flag & DCEL_CHECK_VERTEX_NORMAL ) {
        CORE_ASSERT( !v->N().hasNaN(), "Position contains nan." );
        CORE_ASSERT( v->N().allFinite(), "Position contains inf." );
        if( isVerbose ) {
            LOG( logDEBUG ) << "\tNormal...[Y]";
        }
    }

    /// HALFEDGE
    if( flag & DCEL_CHECK_HALFEDGE_PTR ) {
        CORE_ASSERT( ( v->HE() != nullptr ), "HalfEdge is nullptr." );
        if( isVerbose ) {
            LOG( logDEBUG ) << "\tHalfEdge...[Y]";
        }
    }

    if( isVerbose ) {
        LOG( logDEBUG ) << "Checking vertex done.";
    }
#endif
}

//=====================================================================
//=====================================================================

void check( const HalfEdge_ptr& he, const uint flag ) {
#ifdef CORE_DEBUG
    bool isVerbose = ( flag & DCEL_CHECK_VERBOSE ) || ( flag & DCEL_CHECK_VERBOSE_REC );

    if( isVerbose ) {
        LOG( logDEBUG ) << "Checking halfedge...";
    }

    /// INDEX
    if( flag & DCEL_CHECK_INDEX ) {
        CORE_ASSERT( he->idx.isValid(), "Index is invalid." );
        if( isVerbose ) {
            LOG( logDEBUG ) << "\tIndex...\t\t[Y]";
        }
    }

    /// VERTEX
    if( flag & DCEL_CHECK_VERTEX_PTR ) {
        CORE_ASSERT( ( he->V() != nullptr ), "Vertex is nullptr." );
        if( isVerbose ) {
            LOG( logDEBUG ) << "\tVertex...\t\t[Y]";
        }
    }

    /// NEXT
    if( flag & DCEL_CHECK_HALFEDGE_NEXT ) {
        CORE_ASSERT( ( he->Next() != nullptr ), "Next is nullptr." );
        if( isVerbose ) {
            LOG( logDEBUG ) << "\tNext...\t\t[Y]";
        }
    }

    /// PREV
    if( flag & DCEL_CHECK_HALFEDGE_PREV ) {
        CORE_ASSERT( ( he->Prev() != nullptr ), "Prev is nullptr." );
        if( isVerbose ) {
            LOG( logDEBUG ) << "\tPrev...\t\t[Y]";
        }
    }

    /// TWIN
    if( flag & DCEL_CHECK_HALFEDGE_TWIN ) {
        CORE_ASSERT( ( he->Twin() != nullptr ), "Twin is nullptr." );
        if( isVerbose ) {
            LOG( logDEBUG ) << "\tTwin...\t\t[Y]";
        }
    }

    /// FULLEDGE
    if( flag & DCEL_CHECK_FULLEDGE_PTR ) {
        CORE_ASSERT( ( he->FE() != nullptr ), "FullEdge is nullptr." );
        if( isVerbose ) {
            LOG( logDEBUG ) << "\tFullEdge...\t\t[Y]";
        }
    }

    /// FACE
    if( flag & DCEL_CHECK_FACE_PTR ) {
        CORE_ASSERT( ( he->F() != nullptr ), "Face is nullptr." );
        if( isVerbose ) {
            LOG( logDEBUG ) << "\tFace...\t\t[Y]";
        }
    }

    if( isVerbose ) {
        LOG( logDEBUG ) << "Checking halfedge done.";
    }
#endif
}

//=====================================================================
//=====================================================================

void check( const FullEdge_ptr& fe, const uint flag ) {
#ifdef CORE_DEBUG
    bool isVerbose = ( flag & DCEL_CHECK_VERBOSE ) || ( flag & DCEL_CHECK_VERBOSE_REC );

    if( isVerbose ) {
        LOG( logDEBUG ) << "Checking fullfedge...";
    }

    /// INDEX
    if( flag & DCEL_CHECK_INDEX ) {
        CORE_ASSERT( fe->idx.isValid(), "Index is invalid." );
        if( isVerbose ) {
            LOG( logDEBUG ) << "\tIndex...\t\t[Y]";
        }
    }

    /// HALFEDGE
    if( flag & DCEL_CHECK_HALFEDGE_PTR ) {
        CORE_ASSERT( ( fe->HE( 0 ) != nullptr ), "HalfEdge is nullptr." );
        if( isVerbose ) {
            LOG( logDEBUG ) << "\tHalfEdge...\t\t[Y]";
        }
    }

    /// STATUS
    if( flag & DCEL_CHECK_FULLEDGE_STATUS ) {
        CORE_ASSERT( ( fe->HE( 1 ) != nullptr ), "FullEdge is corrupted." );
        if( isVerbose ) {
            LOG( logDEBUG ) << "\tStatus...\t\t[Y]";
        }
    }

    if( isVerbose ) {
        LOG( logDEBUG ) << "Checking fullfedge done.";
    }
#endif
}

//=====================================================================
//=====================================================================

void check( const Face_ptr& f, const uint flag ) {
#ifdef CORE_DEBUG
    bool isVerbose = ( flag & DCEL_CHECK_VERBOSE ) || ( flag & DCEL_CHECK_VERBOSE_REC );

    if( isVerbose ) {
        LOG( logDEBUG ) << "Checking face...";
    }

    /// INDEX
    if( flag & DCEL_CHECK_INDEX ) {
        CORE_ASSERT( f->idx.isValid(), "Index is invalid." );
        if( isVerbose ) {
            LOG( logDEBUG ) << "\tIndex...\t\t[Y]";
        }
    }

    /// HALFEDGE
    if( flag & DCEL_CHECK_HALFEDGE_PTR ) {
        CORE_ASSERT( ( f->HE() != nullptr ), "HalfEdge is nullptr." );
        if( isVerbose ) {
            LOG( logDEBUG ) << "\tHalfEdge...\t\t[Y]";
        }
    }

    if( isVerbose ) {
        LOG( logDEBUG ) << "Checking face done.";
    }
#endif
}

//=====================================================================
//=====================================================================

void check( const Dcel_ptr& dcel, const uint flag ) {
#ifdef CORE_DEBUG
    bool isVerbose = flag & DCEL_CHECK_VERBOSE;
    uint verbosity = ( flag & DCEL_CHECK_VERBOSE_REC ) ? DCEL_CHECK_VERBOSE : DCEL_CHECK_NOP;

    if( isVerbose ) {
        LOG( logDEBUG ) << "Checking DCEL...";
    }

    /// INDEX
    if( flag & DCEL_CHECK_INDEX ) {
        CORE_ASSERT( dcel->idx.isValid(), "Index is invalid." );
        if( isVerbose ) {
            LOG( logDEBUG ) << "\tIndex...\t\t[Y]";
        }
    }

    /// VERTEX
    const uint v_size = dcel->m_vertex.size();
    if( flag & DCEL_CHECK_VERTEX_PTR ) {
        if( isVerbose ) {
            LOG( logDEBUG ) << "Checking Vertex List...";
        }
        for( uint i = 0; i < v_size; ++i ) {
            check( dcel->m_vertex.at( i ), verbosity | DCEL_CHECK_VERTEX_DEFAULT );
        }
        if( isVerbose ) {
            LOG( logDEBUG ) << "\tVertex List...\t[Y]";
            LOG( logDEBUG ) << "Checking Vertex List done.";
        }
    }

    /// HALFEDGE
    const uint he_size = dcel->m_halfedge.size();
    if( flag & DCEL_CHECK_HALFEDGE_PTR ) {
        if( isVerbose ) {
            LOG( logDEBUG ) << "Checking HalfEdge List...";
        }
        for( uint i = 0; i < he_size; ++i ) {
            check( dcel->m_halfedge.at( i ), verbosity | DCEL_CHECK_HALFEDGE_DEFAULT );
        }
        if( isVerbose ) {
            LOG( logDEBUG ) << "\tHalfEdge List...\t[Y]";
            LOG( logDEBUG ) << "Checking HalfEdge List done.";
        }
    }

    /// FULLEDGE
    const uint fe_size = dcel->m_fulledge.size();
    if( flag & DCEL_CHECK_FULLEDGE_PTR ) {
        if( isVerbose ) {
            LOG( logDEBUG ) << "Checking FullEdge List...";
        }

        CORE_ASSERT( ( fe_size == ( he_size / 2 ) ), "Fulledges or halfedges missing." );

        for( uint i = 0; i < fe_size; ++i ) {
            check( dcel->m_fulledge.at( i ), verbosity | DCEL_CHECK_FULLEDGE_DEFAULT );
        }
        if( isVerbose ) {
            LOG( logDEBUG ) << "\tFullEdge List...\t[Y]";
            LOG( logDEBUG ) << "Checking FullEdge List done.";
        }
    }

    /// FACE
    const uint f_size = dcel->m_face.size();
    if( flag & DCEL_CHECK_FACE_PTR ) {
        if( isVerbose ) {
            LOG( logDEBUG ) << "Checking Face List...";
        }
        for( uint i = 0; i < f_size; ++i ) {
            check( dcel->m_face.at( i ), verbosity | DCEL_CHECK_FACE_DEFAULT );
        }
        if( isVerbose ) {
            LOG( logDEBUG ) << "\tFace List...\t[Y]";
            LOG( logDEBUG ) << "Checking Face List done.";
        }
    }

    if( isVerbose ) {
        LOG( logDEBUG ) << "\tDCEL...\t[Y]";
        LOG( logDEBUG ) << "Checking DCEL done.";
    }
#endif
}

//=====================================================================
//=====================================================================

} // namespace Core
} // namespace Ra

