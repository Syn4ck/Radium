#include <Core/Algorithm/Subdivision/BK2004/BK2004.hpp>

#ifndef NEW_INTERFACE


#include <set>
#include <utility>

#include <Core/Index/Index.hpp>
#include <Core/Mesh/DCEL/Vertex.hpp>
#include <Core/Mesh/DCEL/FullEdge.hpp>
#include <Core/Mesh/DCEL/Dcel.hpp>
#include <Core/Mesh/DCEL/Operation/Border.hpp>
#include <Core/Mesh/DCEL/Operation/Consistent.hpp>
#include <Core/Mesh/DCEL/Operation/Convert.hpp>
#include <Core/Mesh/DCEL/Operation/Length.hpp>
#include <Core/Mesh/DCEL/Operation/Valence.hpp>
#include <Core/Mesh/DCEL/Operation/Valid.hpp>

#include <Core/Algorithm/Subdivision/FullEdge/EdgeSplitter.hpp>
#include <Core/Algorithm/Subdivision/FullEdge/EdgeCollapser.hpp>
#include <Core/Algorithm/Subdivision/FullEdge/EdgeFlipper.hpp>

#include <Core/Geometry/Normal/Normal.hpp>
#include <Core/Algorithm/Subdivision/BK2004/BK2004Relaxation.hpp>

namespace Ra {
namespace Core {


#define DEBUG_SPLIT
#define DEBUG_COLLAPSE
#define DEBUG_FLIP
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
    m_targetLength( -1.0 ) {
    m_prevDCEL = nullptr;
}

/// DESTRUCTOR
BK2004::~BK2004() { }


/// ALGORITHM STAGE
bool BK2004::configCheck( uint& exitStatus ) {
    bool status = ( m_dcel != nullptr );
    if( !status ) {
        exitStatus = DCEL_NULLPTR;
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

    if( !isConsistent( m_dcel ) ) {
        exitStatus = DCEL_NOT_CONSISTENT;
        return false;
    }

    m_targetLength = meanFullEdgeLength( m_dcel );

    if( m_targetLength <= 0.0 ) {
        exitStatus = TARGET_LENGTH_NOT_VALID;
        return false;
    }

    exitStatus = NO_ERROR;
    return true;
}



bool BK2004::processing( uint& exitStatus ) {
    for( uint alg_iter = 0; alg_iter < m_param.m_algorithmIteration; ++alg_iter ) {
        m_prevMesh.clear();
        convert( *m_dcel, m_prevMesh );

#ifdef DEBUG_SPLIT
        if( !split( exitStatus ) ) {
            m_prevMesh.clear();
            convert( *m_dcel, m_prevMesh );

            return false;
        }

#ifdef CORE_DEBUG
        if( !isConsistent( m_dcel ) ) {
            exitStatus = DCEL_NOT_CONSISTENT;
            return false;
        }
#endif

#endif

#ifdef DEBUG_COLLAPSE
        if( !collapse( exitStatus ) ) {
            m_prevMesh.clear();
            convert( *m_dcel, m_prevMesh );

            return false;
        }

#ifdef CORE_DEBUG
        if( !isConsistent( m_dcel ) ) {
            exitStatus = DCEL_NOT_CONSISTENT;
            return false;
        }
#endif

#endif

#ifdef DEBUG_FLIP
        if( !flip( exitStatus ) ) {
            m_prevMesh.clear();
            convert( *m_dcel, m_prevMesh );

            return false;
        }

#ifdef CORE_DEBUG
        if( !isConsistent( m_dcel ) ) {
            exitStatus = DCEL_NOT_CONSISTENT;
            return false;
        }
#endif

#endif

#ifdef DEBUG_SMOOTH
        if( !smoothing( exitStatus ) ) {
            m_prevMesh.clear();
            convert( *m_dcel, m_prevMesh );

            return false;
        }
#endif
    }
    return true;
}



bool BK2004::postprocessing( uint& exitStatus ) {
    return true;
}



bool BK2004::split( uint& exitStatus ) {
    EdgeSplitter splitter( m_dcel, Index::INVALID_IDX(), false/*isVerbose()*/ );

    std::set< std::pair< Scalar, Index > > splitList;

    const Scalar tooLong = std::pow( ( m_param.m_longScale * m_targetLength ), Scalar( 2.0 ) );

    const uint size = m_dcel->m_fulledge.size();
    for( uint i = 0; i < size; ++i ) {
        FullEdge_ptr fe = m_dcel->m_fulledge[i];
        Scalar l = lengthSquared( fe );
        if( l > tooLong ) {
            splitList.insert( std::pair< Scalar, Index >( l, fe->idx ) );
        }
    }

    for( auto it = splitList.rbegin(); it != splitList.rend(); ++it ) {
        /*m_prevMesh.clear();
        convert( *m_dcel, m_prevMesh );
        if( m_prevDCEL != nullptr ) {
            delete m_prevDCEL;
        }*/
        splitter.setParameters( it->second );
        splitter.run();
        if( splitter.isFailed() ) {
            exitStatus = FULLEDGE_NOT_SPLITTED;
            return false;
        }
    }
    return true;
}



bool BK2004::collapse( uint& exitStatus ) {
    EdgeCollapser collapser( m_dcel, Index::INVALID_IDX(), false/*isVerbose()*/ );

    std::set< std::pair< Scalar, Index > > collapseList;

    const Scalar tooShort = std::pow( ( m_param.m_shortScale * m_targetLength ), Scalar( 2.0 ) );

    const uint size = m_dcel->m_fulledge.size();
    for( uint i = 0; i < size; ++i ) {
        FullEdge_ptr fe = m_dcel->m_fulledge[i];
        Scalar l = lengthSquared( fe );
        if( l < tooShort ) {
            collapseList.insert( std::pair< Scalar, Index >( l, fe->idx ) );
        }
    }

    for( auto it = collapseList.begin(); it != collapseList.end(); ++it ) {
        FullEdge_ptr fe;
        if( m_dcel->m_fulledge.access( it->second, fe ) ) {
            /*m_prevMesh.clear();
            convert( *m_dcel, m_prevMesh );
            if( m_prevDCEL != nullptr ) {
                delete m_prevDCEL;
            }*/

            Scalar l = lengthSquared( fe );
            if( it->first == l ) {
                collapser.setParameters( it->second );
                collapser.run();
                if( collapser.isFailed() ) {
                    exitStatus = FULLEDGE_NOT_COLLAPSED;
                    return false;
                }
            }
        }
    }
    return true;
}



bool BK2004::flip( uint& exitStatus ) {
    EdgeFlipper flipper( m_dcel, Index::INVALID_IDX(), false/*isVerbose()*/ );
    const uint size = m_dcel->m_fulledge.size();
    std::set< std::pair< int, Index > > list;
    for( uint i = 0; i < size; ++i ) {
        FullEdge_ptr fe = m_dcel->m_fulledge[i];
        if( isBorder( fe ) ) {
            continue;
        }
        int  pre_flip = int( valence( fe, false ) );
        if( pre_flip != 0 ) {
            list.insert( std::pair< int, Index >( pre_flip, fe->idx ) );
        }
    }

    for( auto it = list.rbegin(); it != list.rend(); ++it ) {
        FullEdge_ptr fe = m_dcel->m_fulledge[it->second];
        int post_flip = int( valence( fe, true  ) );
        if( it->first > post_flip ) {
            flipper.setParameters( it->second );
            flipper.run();
            if( flipper.isFailed() ) {
                exitStatus = FULLEDGE_NOT_FLIPPED;
                return false;
            }
        }
    }
    return true;
}



bool BK2004::smoothing( uint& exitStatus ) {
    TriangleMesh mesh;
    convert( *m_dcel, mesh );

    BK2004Relaxation relaxation( &mesh, BK2004RelaxationParameters( m_param.m_smoothingIteration, m_param.m_lambda ), false/*isVerbose()*/ );
    relaxation.run();
    if( relaxation.isFailed() ) {
        exitStatus = SMOOTHING_FAILED;
        return false;
    }

    const uint size = mesh.m_vertices.size();
#pragma omp parallel for
    for( uint i = 0; i < size; ++i ) {
        Vertex_ptr v = m_dcel->m_vertex[i];
        if( !isBorder( v ) ) {
            v->setP( mesh.m_vertices[i] );
            v->setN( mesh.m_normals[i] );
        }
    }

    return true;
}



} // namespace Core
} // namespace Ra



#else


#include <set>
#include <utility>

#include <Core/Index/Index.hpp>
#include <Core/Mesh/DCEL/Vertex.hpp>
#include <Core/Mesh/DCEL/FullEdge.hpp>
#include <Core/Mesh/DCEL/Dcel.hpp>
#include <Core/Mesh/DCEL/Operation/Border.hpp>
#include <Core/Mesh/DCEL/Operation/Consistent.hpp>
#include <Core/Mesh/DCEL/Operation/Convert.hpp>
#include <Core/Mesh/DCEL/Operation/Length.hpp>
#include <Core/Mesh/DCEL/Operation/Valence.hpp>
#include <Core/Mesh/DCEL/Operation/Valid.hpp>

#include <Core/Algorithm/Subdivision/FullEdge/EdgeSplitter.hpp>
#include <Core/Algorithm/Subdivision/FullEdge/EdgeCollapser.hpp>
#include <Core/Algorithm/Subdivision/FullEdge/EdgeFlipper.hpp>

#include <Core/Geometry/Normal/Normal.hpp>
#include <Core/Algorithm/Subdivision/BK2004/BK2004Relaxation.hpp>


#ifdef NEW_CRITERIA
#include <Core/Mesh/DCEL/Iterator/Vertex/VFEIterator.hpp>
#include <Core/Mesh/DCEL/Operation/Angle.hpp>
#include <Core/Mesh/DCEL/Operation/Extract.hpp>
#include <Core/Mesh/DCEL/Operation/Intersection.hpp>
#include <Core/Geometry/Triangle/TriangleOperation.hpp>
#endif


namespace Ra {
namespace Core {


#define DEBUG_SPLIT
#define DEBUG_COLLAPSE
#define DEBUG_FLIP
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
BK2004::BK2004( const TriangleMesh* const input,
                TriangleMesh* const       output,
                const BK2004Parameter&    param ) :
    AlgorithmInterface< TriangleMesh, TriangleMesh, BK2004Parameter >( "Botsch Kobbelt 2004", input, output, param ),
    m_dcel( nullptr ),
    m_targetLength( -1.0 ) { }

/// DESTRUCTOR
BK2004::~BK2004() { }


/// ALGORITHM STAGE
bool BK2004::setup( uint& exitStatus ) {
    return true;
}

bool BK2004::configCheck( uint& exitStatus ) {
    if( m_input == nullptr ) {
        exitStatus = INPUT_NULLPTR;
        return false;
    }

    if( m_output == nullptr ) {
        exitStatus = OUTPUT_NULLPTR;
        return false;
    }

    return true;
}


bool BK2004::preprocessing( uint& exitStatus ) {
    const Scalar percStep = 1.0 / 6.0;

    if( m_dcel != nullptr ) {
        delete m_dcel;
    }
    m_dcel = new Dcel();
    convert( *m_input, *m_dcel );

    m_statistics.addPreProcessingPercentage( percStep );
    m_statistics.computeOverallPercentage();
    if( m_settings.isVerbose() ) {
        LOG( logINFO ) << "Execution..." << uint( m_statistics.getOverallPercentage() * 100.0 ) << "%";
    }

    const uint size = m_dcel->m_fulledge.size();
    if( size == 0 ) {
        exitStatus = DCEL_NO_FULLEDGE;
        return false;
    }

    m_statistics.addPreProcessingPercentage( percStep );
    m_statistics.computeOverallPercentage();
    if( m_settings.isVerbose() ) {
        LOG( logINFO ) << "Execution..." << uint( m_statistics.getOverallPercentage() * 100.0 ) << "%";
    }

    if( !isValid( m_dcel ) ) {
        exitStatus = DCEL_NOT_VALID;
        return false;
    }

    m_statistics.addPreProcessingPercentage( percStep );
    m_statistics.computeOverallPercentage();
    if( m_settings.isVerbose() ) {
        LOG( logINFO ) << "Execution..." << uint( m_statistics.getOverallPercentage() * 100.0 ) << "%";
    }

    if( !isConsistent( m_dcel ) ) {
        exitStatus = DCEL_NOT_CONSISTENT;
        return false;
    }

    m_statistics.addPreProcessingPercentage( percStep );
    m_statistics.computeOverallPercentage();
    if( m_settings.isVerbose() ) {
        LOG( logINFO ) << "Execution..." << uint( m_statistics.getOverallPercentage() * 100.0 ) << "%";
    }

    m_targetLength = meanFullEdgeLength( m_dcel );

    m_statistics.addPreProcessingPercentage( percStep );
    m_statistics.computeOverallPercentage();
    if( m_settings.isVerbose() ) {
        LOG( logINFO ) << "Execution..." << uint( m_statistics.getOverallPercentage() * 100.0 ) << "%";
    }

    if( m_targetLength <= 0.0 ) {
        exitStatus = TARGET_LENGTH_NOT_VALID;
        return false;
    }

    m_statistics.addPreProcessingPercentage( percStep );
    m_statistics.computeOverallPercentage();
    if( m_settings.isVerbose() ) {
        LOG( logINFO ) << "Execution..." << uint( m_statistics.getOverallPercentage() * 100.0 ) << "%";
    }

    return true;
}



bool BK2004::processing( uint& exitStatus ) {
#ifdef CORE_DEBUG
    const Scalar tasks    = m_param.m_algorithmIteration * 7;
#else
    const Scalar tasks    = m_param.m_algorithmIteration * 4;
#endif
    const Scalar percStep = 1.0 / tasks;

    for( uint alg_iter = 0; alg_iter < m_param.m_algorithmIteration; ++alg_iter ) {
        LOG( logINFO ) << "Vertices         : " << m_dcel->m_vertex.size();
        LOG( logINFO ) << "Edges            : " << m_dcel->m_fulledge.size();
#ifdef NEW_CRITERIA
    //decide();
#endif



#ifdef DEBUG_COLLAPSE
        if( !collapse( exitStatus ) ) {
            if( m_settings.storeMidResults() ) {
                TriangleMesh* step = addStep( "CollapseFailed" );
                convert( *m_dcel, *step );
            }

            return false;
        }

        if( m_settings.storeMidResults() ) {
            TriangleMesh* step = addStep( "AfterCollapse" );
            convert( *m_dcel, *step );
        }

        m_statistics.addProcessingPercentage( percStep );
        m_statistics.computeOverallPercentage();
        if( m_settings.isVerbose() ) {
            LOG( logINFO ) << "Execution..." << uint( m_statistics.getOverallPercentage() * 100.0 ) << "%";
        }

#ifdef CORE_DEBUG
        if( !isConsistent( m_dcel ) ) {
            exitStatus = DCEL_NOT_CONSISTENT;
            return false;
        }

        m_statistics.addProcessingPercentage( percStep );
        m_statistics.computeOverallPercentage();
        if( m_settings.isVerbose() ) {
            LOG( logINFO ) << "Execution..." << uint( m_statistics.getOverallPercentage() * 100.0 ) << "%";
        }
#endif

#endif


#ifdef DEBUG_SPLIT
        if( !split( exitStatus ) ) {
            if( m_settings.storeMidResults() ) {
                TriangleMesh* step = addStep( "SplitFailed" );
                convert( *m_dcel, *step );
            }

            return false;
        }

        if( m_settings.storeMidResults() ) {
            TriangleMesh* step = addStep( "AfterSplit" );
            convert( *m_dcel, *step );
        }
        m_statistics.addProcessingPercentage( percStep );
        m_statistics.computeOverallPercentage();
        if( m_settings.isVerbose() ) {
            LOG( logINFO ) << "Execution..." << uint( m_statistics.getOverallPercentage() * 100.0 ) << "%";
        }

#ifdef CORE_DEBUG
        if( !isConsistent( m_dcel ) ) {
            exitStatus = DCEL_NOT_CONSISTENT;
            return false;
        }

        m_statistics.addProcessingPercentage( percStep );
        m_statistics.computeOverallPercentage();
        if( m_settings.isVerbose() ) {
            LOG( logINFO ) << "Execution..." << uint( m_statistics.getOverallPercentage() * 100.0 ) << "%";
        }
#endif

#endif



#ifdef DEBUG_FLIP
        if( !flip( exitStatus ) ) {
            if( m_settings.storeMidResults() ) {
                TriangleMesh* step = addStep( "FlipFailed" );
                convert( *m_dcel, *step );
            }

            return false;
        }

        if( m_settings.storeMidResults() ) {
            TriangleMesh* step = addStep( "AfterFlip" );
            convert( *m_dcel, *step );
        }

        m_statistics.addProcessingPercentage( percStep );
        m_statistics.computeOverallPercentage();
        if( m_settings.isVerbose() ) {
            LOG( logINFO ) << "Execution..." << uint( m_statistics.getOverallPercentage() * 100.0 ) << "%";
        }

#ifdef CORE_DEBUG
        if( !isConsistent( m_dcel ) ) {
            exitStatus = DCEL_NOT_CONSISTENT;
            return false;
        }

        m_statistics.addProcessingPercentage( percStep );
        m_statistics.computeOverallPercentage();
        if( m_settings.isVerbose() ) {
            LOG( logINFO ) << "Execution..." << uint( m_statistics.getOverallPercentage() * 100.0 ) << "%";
        }
#endif

#endif

#ifdef DEBUG_SMOOTH
        if( !smoothing( exitStatus ) ) {
            if( m_settings.storeMidResults() ) {
                TriangleMesh* step = addStep( "SmoothingFailed" );
                convert( *m_dcel, *step );
            }

            return false;
        }

        if( m_settings.storeMidResults() ) {
            TriangleMesh* step = addStep( "AfterSmoothing" );
            convert( *m_dcel, *step );
        }

        m_statistics.addProcessingPercentage( percStep );
        m_statistics.computeOverallPercentage();
        if( m_settings.isVerbose() ) {
            LOG( logINFO ) << "Execution..." << uint( m_statistics.getOverallPercentage() * 100.0 ) << "%";
        }
#endif
    }
    return true;
}



bool BK2004::postprocessing( uint& exitStatus ) {
    convert( *m_dcel, *m_output );
    return true;
}



bool BK2004::split( uint& exitStatus ) {

    EdgeSplitter splitter( m_dcel, Index::INVALID_IDX(), false );
#ifndef NEW_CRITERIA
    std::set< std::pair< Scalar, Index > > splitList;

    const Scalar tooLong = std::pow( ( m_param.m_longScale * m_targetLength ), Scalar( 2.0 ) );

    const uint size = m_dcel->m_fulledge.size();
    for( uint i = 0; i < size; ++i ) {
        FullEdge_ptr fe = m_dcel->m_fulledge[i];
        Scalar l = lengthSquared( fe );
        if( l > tooLong ) {
            splitList.insert( std::pair< Scalar, Index >( l, fe->idx ) );
        }
    }

    for( auto it = splitList.rbegin(); it != splitList.rend(); ++it ) {
        splitter.setParameters( it->second );
        splitter.run();
        if( splitter.isFailed() ) {
            exitStatus = FULLEDGE_NOT_SPLITTED;
            return false;
        }
    }
#else


    std::set< std::pair< Scalar, Index > > splitList;

    const Scalar tooLong = std::pow( ( m_param.m_longScale * m_targetLength ), Scalar( 2.0 ) );

    const uint size = m_dcel->m_fulledge.size();

#pragma omp parallel for
    for( uint i = 0; i < size; ++i ) {
        const FullEdge_ptr fe = m_dcel->m_fulledge[i];
        const Scalar l = lengthSquared( fe );
        if( ( l > tooLong ) && needSplit( fe ) ) {
#pragma omp critical
            {
            splitList.insert( std::pair< Scalar, Index >( l, fe->idx ) );
            }
        }
    }

    for( auto it = splitList.rbegin(); it != splitList.rend(); ++it ) {
        splitter.setParameters( it->second );
        splitter.run();
        if( splitter.isFailed() ) {
            exitStatus = FULLEDGE_NOT_SPLITTED;
            return false;
        }
    }

#if 0
    for( const auto& entry : m_split ) {
        splitter.setParameters( entry );
        splitter.run();
        if( splitter.isFailed() ) {
            exitStatus = FULLEDGE_NOT_SPLITTED;
            return false;
        }
    }
#endif

#endif
    return true;
}



bool BK2004::collapse( uint& exitStatus ) {
    EdgeCollapser collapser( m_dcel, Index::INVALID_IDX(), false );

#ifndef NEW_CRITERIA
    std::set< std::pair< Scalar, Index > > collapseList;

    const Scalar tooShort = std::pow( ( m_param.m_shortScale * m_targetLength ), Scalar( 2.0 ) );

    const uint size = m_dcel->m_fulledge.size();
    for( uint i = 0; i < size; ++i ) {
        FullEdge_ptr fe = m_dcel->m_fulledge[i];
        Scalar l = lengthSquared( fe );
        if( l < tooShort ) {
            collapseList.insert( std::pair< Scalar, Index >( l, fe->idx ) );
        }
    }

    for( auto it = collapseList.begin(); it != collapseList.end(); ++it ) {
        FullEdge_ptr fe;
        if( m_dcel->m_fulledge.access( it->second, fe ) ) {
            Scalar l = lengthSquared( fe );
            if( it->first == l ) {
                collapser.setParameters( it->second );
                collapser.run();
                if( collapser.isFailed() ) {
                    exitStatus = FULLEDGE_NOT_COLLAPSED;
                    return false;
                }
            }
        }
    }
#else


    std::set< std::pair< Scalar, Index > > collapseList;

    const Scalar tooShort = std::pow( ( m_param.m_shortScale * m_targetLength ), Scalar( 2.0 ) );
    std::set< Index > lock;

    const uint size = m_dcel->m_fulledge.size();

#pragma omp parallel for
    for( uint i = 0; i < size; ++i ) {
        const FullEdge_ptr fe = m_dcel->m_fulledge[i];
        const Scalar l = lengthSquared( fe );
        if( ( l < tooShort ) && needCollapse( fe ) ) {
#pragma omp critical
{
            if( lock.find( fe->idx ) == lock.end() ) {
                collapseList.insert( std::pair< Scalar, Index >( l, fe->idx ) );
                setPolygonBlock( fe, lock );
            }
}
        }
    }


    for( const auto& entry : collapseList ) {
        collapser.setParameters( entry.second );
        collapser.run();
        if( collapser.isFailed() ) {
            exitStatus = FULLEDGE_NOT_COLLAPSED;
            return false;
        }
    }

#if 0
    for( const auto& entry : m_collapse ) {
        collapser.setParameters( entry );
        collapser.run();
        if( collapser.isFailed() ) {
            exitStatus = FULLEDGE_NOT_COLLAPSED;
            return false;
        }
    }
#endif


#endif

    return true;
}



bool BK2004::flip( uint& exitStatus ) {
    EdgeFlipper flipper( m_dcel, Index::INVALID_IDX(), false );
#ifndef NEW_CRITERIA

    const uint size = m_dcel->m_fulledge.size();
    std::set< std::pair< int, Index > > list;
    for( uint i = 0; i < size; ++i ) {
        FullEdge_ptr fe = m_dcel->m_fulledge[i];
        if( isBorder( fe ) ) {
            continue;
        }
        int  pre_flip = int( valence( fe, false ) );
        if( pre_flip != 0 ) {
            list.insert( std::pair< int, Index >( pre_flip, fe->idx ) );
        }
    }

    for( auto it = list.rbegin(); it != list.rend(); ++it ) {
        FullEdge_ptr fe = m_dcel->m_fulledge[it->second];
        int post_flip = int( valence( fe, true  ) );
        if( it->first > post_flip ) {
            flipper.setParameters( it->second );
            flipper.run();
            if( flipper.isFailed() ) {
                exitStatus = FULLEDGE_NOT_FLIPPED;
                return false;
            }
        }
    }
#else


    uint size = m_dcel->m_fulledge.size();
    std::set< std::pair< uint, Index > > list;
    std::vector< Index > to_process;
    std::set< Index > lock;

#pragma omp parallel for
    for( uint i = 0; i < size; ++i ) {
        const FullEdge_ptr fe = m_dcel->m_fulledge[i];
        uint  pre_flip = valence( fe, false );
        uint post_flip = valence( fe, true  );
        if( ( pre_flip > post_flip ) && needFlip( fe ) ) {
#pragma omp critical
{
            list.insert( std::pair< uint, Index >( pre_flip, fe->idx ) );
}
        }
    }


    for( const auto& entry : list ) {
        const FullEdge_ptr fe = m_dcel->m_fulledge[entry.second];
        if( lock.find( fe->idx ) == lock.end() ) {
            to_process.push_back( fe->idx );
            setDiamondBlock( fe , lock );
        }
    }

    for( const auto& entry : to_process ) {
        flipper.setParameters( entry );
        flipper.run();
        if( flipper.isFailed() ) {
            exitStatus = FULLEDGE_NOT_FLIPPED;
            return false;
        }
    }




#if 0
    for( const auto& entry : m_flip ) {
        flipper.setParameters( entry );
        flipper.run();
        if( flipper.isFailed() ) {
            exitStatus = FULLEDGE_NOT_FLIPPED;
            return false;
        }
    }
#endif

#endif
    return true;
}



bool BK2004::smoothing( uint& exitStatus ) {
    TriangleMesh mesh;
    convert( *m_dcel, mesh );

    BK2004Relaxation relaxation( &mesh, BK2004RelaxationParameters( m_param.m_smoothingIteration, m_param.m_lambda ), false );
    relaxation.run();
    if( relaxation.isFailed() ) {
        exitStatus = SMOOTHING_FAILED;
        return false;
    }

    const uint size = mesh.m_vertices.size();
#pragma omp parallel for
    for( uint i = 0; i < size; ++i ) {
        Vertex_ptr v = m_dcel->m_vertex[i];
        if( true/*!isBorder( v )*/ ) {
            v->setP( mesh.m_vertices[i] );
            v->setN( mesh.m_normals[i] );
        }
    }

    return true;
}







#ifdef NEW_CRITERIA

void BK2004::decide() {
    m_split.clear();
    m_collapse.clear();
    m_flip.clear();

    const uint size = m_dcel->m_fulledge.size();
    const Scalar tooShort = std::pow( ( m_param.m_shortScale * m_targetLength ), Scalar( 2.0 ) );
    const Scalar tooLong  = std::pow( (  m_param.m_longScale * m_targetLength ), Scalar( 2.0 ) );

    std::set< std::pair< Scalar, FullEdge_ptr > > splitList;
    std::set< std::pair< Scalar, FullEdge_ptr > > collapseList;
    std::set< std::pair<   uint, FullEdge_ptr > > flipList;
    std::set< Index > lock;

    LOG( logINFO ) << "Edges            : " << size;


#pragma omp parallel for
    for( uint i = 0; i < size; ++i ) {
        const FullEdge_ptr fe = m_dcel->m_fulledge[i];

        const Scalar length = lengthSquared( fe );

#ifdef DEBUG_SPLIT
        if( ( length > tooLong ) && needSplit( fe ) ) {
#pragma omp critical
            {
                splitList.insert( std::pair< Scalar, FullEdge_ptr >( length, fe ) );
            }
#else
        if( false ) {
#endif
        } else {
#ifdef DEBUG_COLLAPSE
            if( ( length < tooShort ) && needCollapse( fe ) ) {
#pragma omp critical
                {
                    collapseList.insert( std::pair< Scalar, FullEdge_ptr >( length, fe ) );
                }
#else
            if( false ) {
#endif
            } else {
#ifdef DEBUG_FLIP
                const uint  pre_flip = valence( fe, false );
                const uint post_flip = valence( fe, true  );
                if( ( pre_flip > post_flip ) && needFlip( fe ) ) {
        #pragma omp critical
                    {
                        flipList.insert( std::pair< uint, FullEdge_ptr >( pre_flip, fe ) );
                    }

                }
#endif
            }
        }

    }

    //=================

    for( const auto& entry : collapseList ) {
        const FullEdge_ptr fe = entry.second;
        const Index&       id = fe->idx;

        auto it = lock.find( id );
        const bool notLocked = ( it == lock.end() );
        if( notLocked ) {
            if( neighborsCheckCollapse( fe ) ) {
                m_collapse.insert( id );
                setPolygonBlock( fe, lock );
            } else {
#ifdef DEBUG_FLIP
                const uint  pre_flip = valence( fe, false );
                const uint post_flip = valence( fe, true  );
                if( ( pre_flip > post_flip ) && needFlip( fe ) ) {
                    flipList.insert( std::pair< uint, FullEdge_ptr >( pre_flip, fe ) );
                }
#endif
            }
        }
    }

    for( auto eit = splitList.rbegin(); eit != splitList.rend(); ++eit ) {
        const FullEdge_ptr fe = eit->second;
        const Index&       id = fe->idx;

        auto it = lock.find( id );
        const bool notLocked = ( it == lock.end() );
        if( notLocked ) {
            if( neighborsCheckSplit( fe ) ) {
                m_split.insert( id );
                //setDiamondBlock( fe, lock );
            } else {
#ifdef DEBUG_FLIP
                const uint  pre_flip = valence( fe, false );
                const uint post_flip = valence( fe, true  );
                if( ( pre_flip > post_flip ) && needFlip( fe ) ) {
                    flipList.insert( std::pair< uint, FullEdge_ptr >( pre_flip, fe ) );
                }
#endif
            }
        }
    }

    for( const auto& entry : flipList ) {
        const FullEdge_ptr fe = entry.second;
        const Index&       id = fe->idx;

        auto it = lock.find( id );
        const bool notLocked = ( it == lock.end() );
        if( notLocked ) {
            if( neighborsCheckFlip( fe ) ) {
                m_flip.insert( id );
                setDiamondBlock( fe, lock );
            }
        }
    }


    LOG( logINFO ) << "To split         : " << m_split.size();
    LOG( logINFO ) << "To collapse      : " << m_collapse.size();
    LOG( logINFO ) << "To flip          : " << m_flip.size();
    LOG( logINFO ) << "Total to process : " << std::to_string( m_split.size() + m_collapse.size() + m_flip.size() );
}





bool BK2004::neighborsCheckSplit( const FullEdge_ptr fe ) const {
    std::set< Index > diamond;

    diamond.insert( fe->Head( 0 )->idx );
    diamond.insert( fe->Head( 1 )->idx );
    diamond.insert( fe->Tail( 0 )->idx );
    diamond.insert( fe->Tail( 1 )->idx );

    for( const auto& index : diamond ) {
//        // Split
        auto it = m_split.find( index );
//        if( it != m_split.end() ) {
//            return false;
//        }

        // Collapse
        it = m_collapse.find( index );
        if( it != m_collapse.end() ) {
            return false;
        }

//        // Flip
//        it = m_flip.find( index );
//        if( it != m_flip.end() ) {
//            return false;
//        }
    }
    return true;
}



bool BK2004::neighborsCheckCollapse( const FullEdge_ptr fe ) const {
    std::set< Index > poly;

    for( uint i = 0; i < 2; ++i ) {
        VFEIterator it( fe->V( i ) );
        auto list = it.list();
        for( const auto& entry : list ) {
            poly.insert( entry->Head( 0 )->idx );
            poly.insert( entry->Head( 1 )->idx );
            poly.insert( entry->Tail( 0 )->idx );
            poly.insert( entry->Tail( 1 )->idx );
        }
    }
    poly.erase( fe->idx );


    for( const auto& index : poly ) {
        // Split
        auto it = m_split.find( index );
        if( it != m_split.end() ) {
            return false;
        }

        // Collapse
        it = m_collapse.find( index );
        if( it != m_collapse.end() ) {
            return false;
        }

        // Flip
        it = m_flip.find( index );
        if( it != m_flip.end() ) {
            return false;
        }
    }
    return true;
}



bool BK2004::neighborsCheckFlip( const FullEdge_ptr fe ) const {
    std::set< Index > diamond;

    diamond.insert( fe->Head( 0 )->idx );
    diamond.insert( fe->Head( 1 )->idx );
    diamond.insert( fe->Tail( 0 )->idx );
    diamond.insert( fe->Tail( 1 )->idx );

    for( const auto& index : diamond ) {
        // Split
        auto it = m_split.find( index );
        if( it != m_split.end() ) {
            return false;
        }

        // Flip
        it = m_flip.find( index );
        if( it != m_flip.end() ) {
            return false;
        }
    }

    std::set< Index > poly;
    for( uint i = 0; i < 2; ++i ) {
        VFEIterator it( fe->V( i ) );
        auto list = it.list();
        for( const auto& entry : list ) {
            poly.insert( entry->idx );
        }
    }
    poly.erase( fe->idx );


    for( const auto& index : poly ) {
        // Collapse
        auto it = m_collapse.find( index );
        if( it != m_collapse.end() ) {
            return false;
        }
    }
    return true;
}


void BK2004::setDiamondBlock( const FullEdge_ptr fe, std::set< Index >& lock ) const {
    lock.insert( fe->idx );
    lock.insert( fe->Head( 0 )->idx );
    lock.insert( fe->Head( 1 )->idx );
    lock.insert( fe->Tail( 0 )->idx );
    lock.insert( fe->Tail( 1 )->idx );
}

void BK2004::setPolygonBlock( const FullEdge_ptr fe, std::set< Index >& lock ) const {
    lock.insert( fe->idx );
    for( uint i = 0; i < 2; ++i ) {
        VFEIterator it( fe->V( i ) );
        auto list = it.list();
        for( const auto& entry : list ) {
            lock.insert( entry->idx );
        }
    }

#if 0
    lock.insert( fe->idx );
    for( uint i = 0; i < 2; ++i ) {
        VFEIterator it( fe->V( i ) );
        auto list = it.list();
        for( const auto& entry : list ) {
            lock.insert( entry->Head( 0 )->idx );
            lock.insert( entry->Head( 1 )->idx );
            lock.insert( entry->Tail( 0 )->idx );
            lock.insert( entry->Tail( 1 )->idx );
        }
    }
#endif

}



bool BK2004::needSplit( const FullEdge_ptr fe ) const {
    return true;
}

bool BK2004::needCollapse( const FullEdge_ptr fe ) const {
    if( oneRingIntersection( fe->V( 0 ), fe->V( 1 ) ) > 2 ) {
        return false;
    }

    if( faceInversionCollapse( fe ) ) {
        return false;
    }

    return true;
}

bool BK2004::needFlip( const FullEdge_ptr fe ) const {
    if( isBorder( fe ) ) {
        return false;
    }

    if( angle( fe ) > Math::PiDiv2 ) {
        return false;
    }

    if( faceInversionFlip( fe ) ) {
        return false;
    }

    if( triangleQualityDecreasing( fe ) ) {
        return false;
    }

    return true;
}


bool BK2004::faceInversionCollapse( const FullEdge_ptr fe ) const {
    TriangleMesh mesh;
    extract( fe, mesh );

    const Vector3 c = 0.5 * ( mesh.m_vertices[0] + mesh.m_vertices[1] );
    Vector3       n = Vector3::Zero();

    std::vector< Edge > edge;

    for( const auto& t : mesh.m_triangles ) {
        const uint i = t[0];
        const uint j = t[1];
        const uint k = t[2];

        if( ( ( i == 0 ) && ( j == 1 ) ) ||
            ( ( j == 0 ) && ( k == 1 ) ) ||
            ( ( k == 0 ) && ( i == 1 ) ) ) {
            const Vector3& p = mesh.m_vertices[i];
            const Vector3& q = mesh.m_vertices[j];
            const Vector3& r = mesh.m_vertices[k];
            n += Geometry::triangleNormal( p, q, r );
        } else {
            if( ( i == 0 ) || ( i == 1 ) ) {
                edge.push_back( Edge( j, k ) );
                continue;
            }

            if( ( j == 0 ) || ( j == 1 ) ) {
                edge.push_back( Edge( k, i ) );
                continue;
            }

            if( ( k == 0 ) || ( k == 1 ) ) {
                edge.push_back( Edge( i, j ) );
                continue;
            }
        }
    }
    n.normalize();

    const Plane3 plane( n, c );

    const Scalar  angle = Vector::angle< Vector3 >( Vector3( 0.0, 0.0, 1.0 ), n );
    const Vector3 axis  = ( Vector3( 0.0, 0.0, 1.0 ).cross( n ) ).normalized();

    const AngleAxis   R( angle, axis );
    const Translation T( -c );

    for( uint i = 0; i < 2; ++i ) {
        mesh.m_vertices[i] = R * T * mesh.m_vertices[i];
    }
    for( uint i = 2; i < mesh.m_vertices.size(); ++i ) {
        mesh.m_vertices[i] = R * T * plane.projection( mesh.m_vertices[i] );
    }

    for( const auto& e : edge ) {
        const uint i = e[0];
        const uint j = e[1];
        const Vector3 p0 = mesh.m_vertices[i];
        const Vector3 p1 = mesh.m_vertices[j];
        const Scalar x1 = p0[0];
        const Scalar x2 = p1[0];
        const Scalar y1 = p0[1];
        const Scalar y2 = p1[1];

        if( ( ( x1 * y2 ) - ( x2 * y1 ) ) < 0.0 ) {
            return true;
        }
    }

    return false;
}

bool BK2004::faceInversionFlip( const FullEdge_ptr fe ) const {
    Vector3 p = fe->V( 0 )->P();
    Vector3 q = fe->V( 1 )->P();
    Vector3 r = fe->HE( 0 )->Prev()->V()->P();
    Vector3 s = fe->HE( 1 )->Prev()->V()->P();

    const Vector3 c = 0.5 * ( r + s );
    const Vector3 n = ( fe->HE( 0 )->Prev()->V()->N() + fe->HE( 1 )->Prev()->V()->N() ).normalized();

    const Plane3 plane( n, c );

    const Scalar  angle = Vector::angle< Vector3 >( Vector3( 0.0, 0.0, 1.0 ), n );
    const Vector3 axis  = ( Vector3( 0.0, 0.0, 1.0 ).cross( n ) ).normalized();

    const AngleAxis   R( angle, axis );
    const Translation T( -c );

    r = R * T * r;
    s = R * T * s;
    p = R * T * plane.projection( p );
    q = R * T * plane.projection( q );

    Scalar f[2];
    f[0] = ( ( s[0] - r[0] ) * ( p[1] - r[1] ) ) - ( ( s[1] - r[1] ) * ( p[0] - r[0] ) );
    f[1] = ( ( s[0] - r[0] ) * ( q[1] - r[1] ) ) - ( ( s[1] - r[1] ) * ( q[0] - r[0] ) );

    return ( ( f[0] * f[1] ) >= 0 );
}


bool BK2004::triangleQualityDecreasing( const FullEdge_ptr& fe ) const {
    const Vector3 p = fe->V( 0 )->P();
    const Vector3 q = fe->V( 1 )->P();
    const Vector3 r = fe->HE( 0 )->Prev()->V()->P();
    const Vector3 s = fe->HE( 1 )->Prev()->V()->P();
    const Scalar pqr = Geometry::triangleAngleRatio( p, q, r );
    const Scalar psr = Geometry::triangleAngleRatio( p, s, r );
    const Scalar qps = Geometry::triangleAngleRatio( q, p, s );
    const Scalar qrs = Geometry::triangleAngleRatio( q, r, s );
    if( std::min( pqr, qps ) > std::min( psr, qrs ) ) {
        return true;
    }
    return false;
}

#endif

} // namespace Core
} // namespace Ra




#endif
