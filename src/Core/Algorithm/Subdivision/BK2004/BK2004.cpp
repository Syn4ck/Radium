#include <Core/Algorithm/Subdivision/BK2004/BK2004.hpp>

#include <Core/Index/Index.hpp>
#include <Core/Mesh/DCEL/Vertex.hpp>
#include <Core/Mesh/DCEL/FullEdge.hpp>
#include <Core/Mesh/DCEL/Dcel.hpp>
#include <Core/Mesh/DCEL/Operation/Border.hpp>
#include <Core/Mesh/DCEL/Operation/Check.hpp>
#include <Core/Mesh/DCEL/Operation/Consistent.hpp>
#include <Core/Mesh/DCEL/Operation/Convert.hpp>
#include <Core/Mesh/DCEL/Operation/Extract.hpp>
#include <Core/Mesh/DCEL/Operation/Length.hpp>
#include <Core/Mesh/DCEL/Operation/Valence.hpp>
#include <Core/Mesh/DCEL/Operation/Valid.hpp>

#include <Core/Geometry/Normal/Normal.hpp>
#include <Core/Algorithm/Subdivision/BK2004/BK2004Smoothing.hpp>

namespace Ra {
namespace Core {


#define DEBUG_SMOOTH


/// CONSTRUCTOR
BK2004Parameter::BK2004Parameter( const uint   algorithmIteration,
                                  const uint   smoothingIteration,
                                  const Scalar longScale,
                                  const Scalar shortScale,
                                  const Scalar lambdaFactor ) :
    m_algorithmIteration( algorithmIteration ),
    m_smoothingIteration( smoothingIteration ),
    m_longScale( longScale ),
    m_shortScale( shortScale ),
    m_lambda( lambdaFactor ) { }



/// CONSTRUCTOR
BK2004::BK2004( const Dcel_ptr&        dcel,
                const BK2004Parameter& param,
                const bool             verbosity ) :
    Algorithm< BK2004Parameter >( param, "Botsch Kobbelt 2004", verbosity ),
    m_dcel( dcel ),
    m_targetLength( -1.0 ),
    m_subHandler( dcel ) {

    m_prevDCEL = nullptr;
}

/// DESTRUCTOR
BK2004::~BK2004() { }


/// ALGORITHM STAGE
bool BK2004::configCheck( uint& exitStatus ) {
    bool       dcelStatus = ( m_dcel != nullptr );
    bool subHandlerStatus = ( m_subHandler.getDCEL() != nullptr );
    bool         matching = ( m_dcel == m_subHandler.getDCEL() );
    bool           status = ( dcelStatus && subHandlerStatus && matching );
    if( status ) {
        exitStatus = NO_ERROR;
    } else {
        if( !dcelStatus ) {
            exitStatus = DCEL_NULLPTR;
        } else {
            if( !subHandlerStatus ) {
                exitStatus = SUBDIVIDER_NOT_INITIALIZED;
            } else {
                exitStatus = DCEL_AND_SUBDIVIDER_MISMATCH;
            }
        }
    }
    return status;
}


bool BK2004::preprocessing( uint& exitStatus ) {
    const uint size = m_dcel->m_fulledge.size();
    if( size == 0 ) {
        exitStatus = DCEL_NO_FULLEDGE;
        return false;
    }

    if( !isValid( m_dcel ) ) {
        exitStatus = DCEL_NOT_VALID;
        return false;
    }

    //check( m_dcel, DCEL_CHECK_CONSISTENCY | DCEL_CHECK_VERBOSE | DCEL_CHECK_VERBOSE_REC | DCEL_CHECK_DCEL_DEFAULT );
    if( !isConsistent( m_dcel ) ) {
        exitStatus = DCEL_NOT_CONSISTENT;
        return false;
    }

    m_targetLength = meanFullEdgeLength( m_dcel );

    if( m_targetLength <= 0.0 ) {
        exitStatus = TARGET_LENGTH_NOT_VALID;
        return false;
    }

    m_splitList.clear();
    m_collapseList.clear();

    exitStatus = NO_ERROR;
    return true;
}



bool BK2004::processing( uint& exitStatus ) {
    for( uint alg_iter = 0; alg_iter < m_param.m_algorithmIteration; ++alg_iter ) {
        const uint size = m_dcel->m_fulledge.size();
        for( uint i = 0; i < size; ++i ) {
            FullEdge_ptr fe = m_dcel->m_fulledge[i];
            Scalar l = length( fe );
            if( l > ( m_param.m_longScale * m_targetLength ) ) {
                m_splitList.insert( std::pair< Scalar, Index >( l, fe->idx ) );
            }
            if( l < ( m_param.m_shortScale * m_targetLength ) ) {
                m_collapseList.insert( std::pair< Scalar, Index >( l, fe->idx ) );
            }
        }


#ifdef DEBUG_SPLIT
        if( !split( exitStatus ) ) {
                return false;
        }

        if( !isConsistent( m_dcel ) ) {
            exitStatus = DCEL_NOT_CONSISTENT;
            return false;
        }
#endif

#ifdef DEBUG_COLLAPSE
        if( !collapse( exitStatus ) ) {
                return false;
        }

        if( !isConsistent( m_dcel ) ) {
            exitStatus = DCEL_NOT_CONSISTENT;
            return false;
        }
#endif

#ifdef DEBUG_FLIP
        if( !flip( exitStatus ) ) {
            return false;
        }

        if( !isConsistent( m_dcel ) ) {
            exitStatus = DCEL_NOT_CONSISTENT;
            return false;
        }
#endif

        /*if( !isConsistent( m_dcel ) ) {
            exitStatus = DCEL_NOT_CONSISTENT;
            return false;
        }*/

        smoothing();
    }
    exitStatus = NO_ERROR;
    return true;
}


bool BK2004::postprocessing( uint& exitStatus ) {
    return true;
}



void BK2004::extractIndexList( std::vector< Index >& list ) const {
    list.clear();
    const Scalar size = m_dcel->m_fulledge.size();
    list.resize( size );
    for( uint i = 0; i < size; ++i ) {
        list[i] = m_dcel->m_fulledge[i]->idx;
    }
}



bool BK2004::split( uint& exitStatus ) {
#ifdef DEBUG_SPLIT
    for( auto it = m_splitList.begin(); it != m_splitList.end(); ++it ) {
        m_prevMesh.clear();
        convert( *m_dcel, m_prevMesh );
        if( m_prevDCEL != nullptr ) {
            delete m_prevDCEL;
        }

        if( !m_subHandler.splitFullEdge( it->second ) ) {
            exitStatus = FULLEDGE_NOT_SPLITTED;
            return false;
        }
        m_splitList.erase( it );
    }
#endif
    exitStatus = NO_ERROR;
    return true;
}



bool BK2004::collapse( uint& exitStatus ) {
#ifdef DEBUG_COLLAPSE
    for( auto it = m_collapseList.begin(); it != m_collapseList.end(); ++it ) {
        if( m_dcel->m_fulledge.contain( it->second ) ) {
            m_prevMesh.clear();
            convert( *m_dcel, m_prevMesh );
            if( m_prevDCEL != nullptr ) {
                delete m_prevDCEL;
            }

            if( !m_subHandler.collapseFullEdge( it->second ) ) {
                exitStatus = FULLEDGE_NOT_COLLAPSED;
                return false;
            }
        }
        m_collapseList.erase( it );
    }
#endif
    exitStatus = NO_ERROR;
    return true;
}



bool BK2004::flip( uint& exitStatus ) {
#ifdef DEBUG_FLIP
    const uint size = m_dcel->m_fulledge.size();
    std::set< std::pair< int, Index > > list;
    for( uint i = 0; i < size; ++i ) {
        FullEdge_ptr fe = m_dcel->m_fulledge[i];
        if( isBorder( fe ) ) {
            continue;
        }
        int  pre_flip = int( valence( fe, false ) );
        int post_flip = int( valence( fe, true  ) );
        int value     = pre_flip - post_flip;
        if( value > 0 ) {
            list.insert( std::pair< int, Index >( value, fe->idx ) );
        }
    }

    for( auto it = list.rbegin(); it != list.rend(); ++it ) {
        if( !m_subHandler.flipFullEdge( it->second ) ) {
            exitStatus = FULLEDGE_NOT_FLIPPED;
            return false;
        }
        //list.erase( it );
    }
#endif
    exitStatus = NO_ERROR;
    return true;
}



void BK2004::smoothing() {
#ifdef DEBUG_SMOOTH
    TriangleMesh mesh;
    convert( *m_dcel, mesh );

    const uint size = mesh.m_vertices.size();

    Geometry::AdjacencyMatrix Adj = Geometry::uniformAdjacency( mesh.m_vertices, mesh.m_triangles );
    GravityCentroid           G;
    VectorArray< Vector3 > tmp( size );
    for( uint smooth_iter = 0; smooth_iter < m_param.m_smoothingIteration; ++smooth_iter ) {
        Geometry::areaWeightedNormal( mesh.m_vertices, mesh.m_triangles, mesh.m_normals );
        Geometry::AreaMatrix      A   = Geometry::barycentricArea( mesh.m_vertices, mesh.m_triangles );
        g( mesh.m_vertices, Adj, A, G );
        tangentialRelaxation( mesh.m_vertices, mesh.m_normals, G, m_param.m_lambda, tmp );
        std::swap( mesh.m_vertices, tmp );
    }
    Geometry::areaWeightedNormal( mesh.m_vertices, mesh.m_triangles, mesh.m_normals );

    for( uint i = 0; i < size; ++i ) {
        Vertex_ptr v = m_dcel->m_vertex[i];
        v->setP( mesh.m_vertices[i] );
        v->setN( mesh.m_normals[i] );
    }
#endif
}


} // namespace Core
} // namespace Ra

