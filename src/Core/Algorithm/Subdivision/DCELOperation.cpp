#include <Core/Algorithm/Subdivision/DCELOperation.hpp>

#include <Core/Log/Log.hpp>

#include <Core/Mesh/DCEL/Vertex.hpp>
#include <Core/Mesh/DCEL/HalfEdge.hpp>
#include <Core/Mesh/DCEL/FullEdge.hpp>
#include <Core/Mesh/DCEL/Dcel.hpp>

#include <Core/Mesh/DCEL/Operation/Bind.hpp>
#include <Core/Mesh/DCEL/Operation/Border.hpp>

namespace Ra {
namespace Core {


#ifdef DEBUG_SPLIT
bool HESplit( Dcel& dcel, const Index& halfedge_id ) {
    LOG( logDEBUG ) << "HESplit: " << halfedge_id.getValue();
    HalfEdge_ptr ptr;
    if( !dcel.m_halfedge.access( halfedge_id, ptr ) ) {
        LOG( logDEBUG ) << "HalfEdge " << halfedge_id.getValue() << " not present";
        return false;
    }

    if( ptr->F() == nullptr ) {
        LOG( logDEBUG ) << "HalfEdge " << halfedge_id.getValue() << " is a border HalfEdge";
        HalfEdge_ptr he = new HalfEdge();
        CORE_ASSERT( dcel.m_halfedge.insert( he, he->idx ), "new border HalfEdge not inserted" );
        he->setNext( ptr->Next() );
        he->setPrev( ptr );
        bind( he );
    } else {
        LOG( logDEBUG ) << "HalfEdge " << halfedge_id.getValue() << " is not a border HalfEdge";
        // Declare data
        HalfEdge_ptr he[2][3];
        FullEdge_ptr fe;
        Face_ptr     f[2];

        // Initialize data
        he[0][0] = ptr;
        he[0][1] = new HalfEdge();
        he[0][2] = ptr->Prev();
        he[1][0] = ptr->Next();
        he[1][1] = new HalfEdge();
        he[1][2] = new HalfEdge();

        fe = new FullEdge();

        f[0] = ptr->F();
        f[1] = new Face();

        // Set data
        fe->setHE( he[0][2] );
        for( uint i = 0; i < 2; ++i ) {
            f[i] = he[0][0]->F();
            for( uint j = 0; j < 3; ++j ) {
                he[i][j]->setNext( he[i][( j + 1 ) % 3] );
                he[i][j]->setPrev( he[i][( j + 2 ) % 3] );
            }
            he[i][1]->setTwin( he[( i + 1 ) % 2][1] );
        }

        // Insert data
        CORE_ASSERT( dcel.m_halfedge.insert( he[0][1], he[0][1]->idx ), "new HalfEdge[0][1] not inserted" );
        CORE_ASSERT( dcel.m_halfedge.insert( he[1][1], he[1][1]->idx ), "new HalfEdge[1][1] not inserted" );
        CORE_ASSERT( dcel.m_halfedge.insert( he[1][2], he[1][2]->idx ), "new HalfEdge[1][2] not inserted" );
        CORE_ASSERT( dcel.m_fulledge.insert( fe,       fe->idx       ), "new FullEdge not inserted" );
        CORE_ASSERT( dcel.m_face.insert( f[1],     f[1]->idx     ), "new Face[1] not inserted" );
    }

    return true;
}



bool FESplit( Dcel& dcel, const Index& fulledge_id ) {
    LOG( logDEBUG ) << "FESplit: " << fulledge_id.getValue();
    FullEdge_ptr ptr;
    if( !dcel.m_fulledge.access( fulledge_id, ptr ) ) {
        LOG( logDEBUG ) << "FullEdge " << fulledge_id.getValue() << " not present.";
        return false;
    }

    if( ( ptr->HE( 0 ) == nullptr ) || ( ptr->HE( 1 ) == nullptr ) ) {
        LOG( logDEBUG ) << "FullEdge " << fulledge_id.getValue() << " not complete.";
        return false;
    }

    if( !dcel.m_halfedge.contain( ptr->HE( 0 )->idx ) || !dcel.m_halfedge.contain( ptr->HE( 1 )->idx ) ) {
        LOG( logDEBUG ) << "FullEdge " << fulledge_id.getValue() << " is bad.";
        return false;
    }

    HalfEdge_ptr he[2];
    bool         status[2];
    for( uint i = 0; i < 2; ++i ) {
        he[i]     = ptr->HE( i );
        status[i] = HESplit( dcel, he[i]->idx );
    }

    if( status[0] && status[1] ) {
        FullEdge_ptr fe = new FullEdge( he[1] );
        CORE_ASSERT( dcel.m_fulledge.insert( fe, fe->idx ), "new FullEdge not inserted" );
            if( isBorder( he[0] ) ) {
                if( isBorder( he[1] ) ) {
                    he[0]->setTwin( he[1]->Next() );
                    he[1]->setTwin( he[0]->Next() );
                } else {
                    he[0]->setTwin( he[1]->Next()->Twin()->Next() );
                    he[1]->setTwin( he[0]->Next() );
                }
            } else {
                if( isBorder( he[1] ) ) {
                    he[0]->setTwin( he[1]->Next() );
                    he[1]->setTwin( he[0]->Next()->Twin()->Next() );
                } else {
                    he[0]->setTwin( he[1]->Next()->Twin()->Next() );
                    he[1]->setTwin( he[0]->Next()->Twin()->Next() );
                }
            }
            if( ( he[0]->V() != nullptr ) && ( he[1]->V() != nullptr ) ) {
                Vertex_ptr   v  = new Vertex();
                CORE_ASSERT( dcel.m_vertex.insert( v,  v->idx  ), "new Vertex not inserted" );
                v->setP( 0.5 * ( he[0]->V()->P() + he[1]->V()->P() ) );
                v->setN( ( he[0]->V()->N() + he[1]->V()->N() ).normalized() );
                v->setHE( he[0]->Next() );
                bind( v );
            }
        bind( ptr );
        bind( fe );
    } else {
        LOG( logDEBUG ) << "Something went bad with FullEdge " << fulledge_id.getValue();
        if( status[0] ) {
            LOG( logDEBUG ) << "HalfEdge " << he[0]->idx.getValue() << " was splitted";
        }
        if( status[1] ) {
            LOG( logDEBUG ) << "HalfEdge " << he[1]->idx.getValue() << " was splitted";
        }
        return false;
    }

    return true;
}
#endif

#ifdef DEBUG_COLLAPSE
bool HECollapse( Dcel& dcel, const Index& halfedge_id ) {
    LOG( logDEBUG ) << "HECollapse: " << halfedge_id.getValue();
    HalfEdge_ptr ptr;
    if( !dcel.m_halfedge.access( halfedge_id, ptr ) ) {
        LOG( logDEBUG ) << "HalfEdge " << halfedge_id.getValue() << " not present";
        return false;
    }

    if( ptr->isBorder() ) {
        ptr->Prev()->setNext( ptr->Next() );
        ptr->Next()->setPrev( ptr->Prev() );
        CORE_ASSERT( dcel.m_halfedge.remove( halfedge_id ), "HalfEdge not removed" );
        delete ptr;
    } else {
        // Declare the data
        HalfEdge_ptr he[3];
        FullEdge_ptr fe[2];
        Face_ptr     f;

        // Set the data
        he[0] = ptr;
        he[1] = ptr->Next();
        he[2] = ptr->Prev();
        fe[0] = he[1]->FE();
        fe[1] = he[2]->FE();
        f     = ptr->F();

        // Update the data
        he[1]->Twin()->setTwin( he[2]->Twin() );
        he[2]->Twin()->setTwin( he[1]->Twin() );
        fe[0]->setHE( he[1]->Twin() );
        fe[0]->bind();

        // Remove the data
        CORE_ASSERT( dcel.m_halfedge.remove( he[0]->idx ), "HalfEdge he[0] not removed" );
        CORE_ASSERT( dcel.m_halfedge.remove( he[1]->idx ), "HalfEdge he[1] not removed" );
        CORE_ASSERT( dcel.m_halfedge.remove( he[2]->idx ), "HalfEdge he[2] not removed" );
        CORE_ASSERT( dcel.m_fulledge.remove( fe[1]->idx ), "FullEdge fe[1] not removed" );
        CORE_ASSERT( dcel.m_face.remove( f->idx ), "Face was not removed");
        delete he[0];
        delete he[1];
        delete he[2];
        delete fe[1];
        delete f;
    }

    return true;
}



bool FECollapse( Dcel& dcel, const Index& fulledge_id ) {
    LOG( logDEBUG ) << "FECollapse: " << fulledge_id.getValue();
    FullEdge_ptr ptr;
    if( !dcel.m_fulledge.access( fulledge_id, ptr ) ) {
        LOG( logDEBUG ) << "FullEdge " << fulledge_id.getValue() << " not present.";
        return false;
    }

    if( ( ptr->HE( 0 ) == nullptr ) || ( ptr->HE( 1 ) == nullptr ) ) {
        LOG( logDEBUG ) << "FullEdge " << fulledge_id.getValue() << " not complete.";
        return false;
    }

    if( !dcel.m_halfedge.contain( ptr->HE( 0 )->idx ) || !dcel.m_halfedge.contain( ptr->HE( 1 )->idx ) ) {
        LOG( logDEBUG ) << "FullEdge " << fulledge_id.getValue() << " is bad.";
        return false;
    }

    Vertex_ptr   v[2];
    bool         status[2];
    for( uint i = 0; i < 2; ++i ) {
        v[i]      = ptr->V( i );
        status[i] = HECollapse( dcel, ptr->HE( i )->idx );
    }

    if( status[0] && status[1] ) {
        if( ( v[0] != nullptr ) && ( v[1] != nullptr ) ) {
            v[0]->setP( 0.5 * ( v[0]->P() + v[1]->P() ) );
            v[0]->setN( ( v[0]->N() + v[1]->N() ).normalized() );
            v[0]->bind();
            CORE_ASSERT( dcel.m_vertex.remove( v[1]->idx ), "Vertex v[1] not removed" );
            delete v[1];
        }
        CORE_ASSERT( dcel.m_fulledge.remove( ptr->idx ), "FullEdge not removed" );
        delete ptr;
    } else {
        LOG( logDEBUG ) << "Something went bad with FullEdge " << fulledge_id.getValue();
        if( status[0] ) {
            LOG( logDEBUG ) << "HalfEdge " << ptr->HE( 0 )->idx.getValue() << " was splitted";
        }
        if( status[1] ) {
            LOG( logDEBUG ) << "HalfEdge " << ptr->HE( 1 )->idx.getValue() << " was splitted";
        }
        return false;
    }

    return true;
}
#endif

#ifdef DEBUG_FLIP
bool FEFlip( Dcel& dcel, const Index& fulledge_id ) {
    LOG( logDEBUG ) << "FEFlip: " << fulledge_id.getValue();
    FullEdge_ptr ptr;
    if( !dcel.m_fulledge.access( fulledge_id, ptr ) ) {
        LOG( logDEBUG ) << "FullEdge " << fulledge_id.getValue() << " not present.";
        return false;
    }

    if( ( ptr->HE( 0 ) == nullptr ) || ( ptr->HE( 1 ) == nullptr ) ) {
        LOG( logDEBUG ) << "FullEdge " << fulledge_id.getValue() << " not complete.";
        return false;
    }

    if( !dcel.m_halfedge.contain( ptr->HE( 0 )->idx ) || !dcel.m_halfedge.contain( ptr->HE( 1 )->idx ) ) {
        LOG( logDEBUG ) << "FullEdge " << fulledge_id.getValue() << " is bad.";
        return false;
    }

    if( ptr->isBorder() ) {
        LOG( logDEBUG ) << "FullEdge " << fulledge_id.getValue() << " is a border FullEdge.";
        return false;
    }

    // Declare data
    HalfEdge_ptr he[2][3];
    Face_ptr     f[2];

    // Set data
    he[0][0] = ptr->HE( 0 );
    he[0][1] = ptr->HE( 1 )->Prev();
    he[0][2] = ptr->HE( 0 )->Next();
    he[1][0] = ptr->HE( 1 );
    he[1][1] = ptr->HE( 0 )->Prev();
    he[1][2] = ptr->HE( 1 )->Next();
    f[0]     = ptr->HE( 0 )->F();
    f[1]     = ptr->HE( 1 )->F();

    // Set data
    he[0][0]->setV( ptr->HE( 0 )->Prev()->V() );
    he[1][0]->setV( ptr->HE( 1 )->Prev()->V() );
    for( uint i = 0; i < 2; ++i ) {
        for( uint j = 0; j < 3; ++j ) {
            he[i][j]->setNext( he[i][( j + 1 ) % 3] );
            he[i][j]->setPrev( he[i][( j + 2 ) % 3] );
            he[i][j]->setF( f[i] );
        }
        f[i]->setHE( he[i][0] );
    }

    return true;
}
#endif


} // namespace Core
} // namespace Ra
