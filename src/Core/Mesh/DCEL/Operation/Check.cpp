#include <Core/Mesh/DCEL/Operation/Check.hpp>

#include <Core/Mesh/DCEL/Vertex.hpp>
#include <Core/Mesh/DCEL/HalfEdge.hpp>
#include <Core/Mesh/DCEL/FullEdge.hpp>
#include <Core/Mesh/DCEL/Face.hpp>
#include <Core/Mesh/DCEL/Dcel.hpp>

#include <Core/Mesh/DCEL/Iterator/Vertex/VHEIterator.hpp>
#include <Core/Mesh/DCEL/Iterator/Face/FHEIterator.hpp>

#include <Core/Log/Log.hpp>

namespace Ra {
namespace Core {

#define CORE_DEBUG

//=====================================================================
//=====================================================================

void check( const Vertex_ptr& v, const uint flag ) {
#ifdef CORE_DEBUG
    bool isVerbose = ( flag & DCEL_CHECK_VERBOSE ) || ( flag & DCEL_CHECK_VERBOSE_REC );

    if( isVerbose ) {
        LOG( logDEBUG ) << "Checking vertex...";
    }

    /// POINTER
    CORE_ASSERT( ( v != nullptr ), "Nullptr received.");

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

    /// CONSISTENCY
    if( flag & DCEL_CHECK_CONSISTENCY ) {
        CORE_ASSERT( ( v->HE() != nullptr ), "HalfEdge is nullptr." );
        CORE_ASSERT( ( v == v->HE()->V() ), "Vertex is not consistent." );
        VHEIterator it( v );
        const uint size = it.size();
        for( uint i = 0; i < size; ++i ) {
            CORE_ASSERT( ( v == it->V() ), "Vertex is not consistent." );
            ++it;
        }
        if( isVerbose ) {
            LOG( logDEBUG ) << "\tConsistent...[Y]";
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

    /// POINTER
    CORE_ASSERT( ( he != nullptr ), "Nullptr received.");


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

    /// CONSISTENCY
    if( flag & DCEL_CHECK_CONSISTENCY ) {
        CORE_ASSERT( ( he->V() != nullptr ), "Vertex is nullptr." );

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
        CORE_ASSERT( status, "HalfEdge is not consistent." );

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
            CORE_ASSERT( status, "HalfEdge is not consistent." );
        }

        if( isVerbose ) {
            LOG( logDEBUG ) << "\tConsistent...\t\t[Y]";
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

    /// POINTER
    CORE_ASSERT( ( fe != nullptr ), "Nullptr received.");


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

    /// CONSISTENCY
    if( flag & DCEL_CHECK_CONSISTENCY ) {
        CORE_ASSERT( ( fe->HE( 0 ) != nullptr ), "HalfEdge is nullptr." );
        CORE_ASSERT( ( ( fe == fe->HE( 0 )->FE() ) && ( fe == fe->HE( 0 )->Twin()->FE() ) ), "FullEdge is not consistent." );
        if( isVerbose ) {
            LOG( logDEBUG ) << "\tConsistent...\t\t[Y]";
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

    /// POINTER
    CORE_ASSERT( ( f != nullptr ), "Nullptr received.");


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

    /// CONSISTENCY
    if( flag & DCEL_CHECK_CONSISTENCY ) {
        CORE_ASSERT( ( f->HE() != nullptr ), "HalfEdge is nullptr." );
        CORE_ASSERT( ( f == f->HE()->F() ), "Face is not consistent." );

        FHEIterator it( f );
        const uint size = it.size();
        for( uint i = 0; i < size; ++i ) {
            CORE_ASSERT( ( f == it->F() ), "Face is not consistent." );
            ++it;
        }
        it.reset();
        it += size - 1;
        CORE_ASSERT( ( *it == f->HE()->Prev() ), "Face is not consistent." );

        if( isVerbose ) {
            LOG( logDEBUG ) << "\tConsistent...\t\t[Y]";
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
    bool isVerbose   = flag & DCEL_CHECK_VERBOSE;
    uint verbosity   = ( flag & DCEL_CHECK_VERBOSE_REC ) ? DCEL_CHECK_VERBOSE : DCEL_CHECK_NOP;
    uint consistency = flag & DCEL_CHECK_CONSISTENCY;

    if( isVerbose ) {
        LOG( logDEBUG ) << "Checking DCEL...";
    }

    /// POINTER
    CORE_ASSERT( ( dcel != nullptr ), "Nullptr received.");


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
            check( dcel->m_vertex.at( i ), verbosity | consistency | DCEL_CHECK_VERTEX_DEFAULT );
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
            check( dcel->m_halfedge.at( i ), verbosity | consistency | DCEL_CHECK_HALFEDGE_DEFAULT );
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
            check( dcel->m_fulledge.at( i ), verbosity | consistency | DCEL_CHECK_FULLEDGE_DEFAULT );
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
            check( dcel->m_face.at( i ), verbosity | consistency | DCEL_CHECK_FACE_DEFAULT );
        }
        if( isVerbose ) {
            LOG( logDEBUG ) << "\tFace List...\t[Y]";
            LOG( logDEBUG ) << "Checking Face List done.";
        }
    }

    if( isVerbose ) {
        if( consistency ) {
            LOG( logDEBUG ) << "\tConsistent...\t[Y]";
        }
        LOG( logDEBUG ) << "Checking DCEL done.";
    }
#endif
}

//=====================================================================
//=====================================================================

} // namespace Core
} // namespace Ra

