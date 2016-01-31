#include <Core/Algorithm/Subdivision/BK2004>

#include <Core/Index/Index.hpp>
#include <Core/Mesh/DCEL/FullEdge.hpp>
#include <Core/Mesh/DCEL/Dcel.hpp>
#include <Core/Mesh/DCEL/Operation/Valid.hpp>
#include <Core/Mesh/DCEL/Operation/Consistent.hpp>
#include <Core/Mesh/DCEL/Operation/Length.hpp>

namespace Ra {
namespace Core {

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
    m_targetLength( -1.0 ) { }

/// DESTRUCTOR
BK2004::~BK2004() { }


/// ALGORITHM STAGE
bool BK2004::preprocessing( uint& exitStatus ) override {
    const uint size = m_dcel.m_fulledge.size();
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

    --- Compute normals

    exitStatus = NO_ERROR;
    return true;
}



bool BK2004::processing( uint& exitStatus ) override {
    for( uint alg_iter = 0; alg_iter < m_param.m_algorithmIteration; ++alg_iter ) {
        if( !splitNcollapse( exitStatus ) ) {
                return false;
        }

        if( !flip( exitStatus ) ) {
            return false;
        }

        if( !isConsistent( m_dcel ) ) {
            exitStatus = DCEL_NOT_CONSISTENT;
            return false;
        }

        --- compute normals

        smoothing();

    }

    --- compute normals

    exitStatus = NO_ERROR;
    return true;
}



void BK2004::extractoIndexList( std::vector< Index >& list ) const {
    list.clear();
    const Scalar size = m_dcel._fulledge.size();
    list.resize( size );
    for( uint i = 0; i < size; ++i ) {
        list[i] = m_dcel.m_fulledge[i]->idx;
    }
}



bool BK2004::splitNcollapse( uint& exitStatus ) {
    const uint   size     = m_dcel.m_fulledge.size();

    std::vector< Index >  edge( size );
    extractIndexList( edge );

    std::vector< Scalar > length;
    FullEdgeLength( m_dcel, length );

    for( uint i = 0; i < size; ++i ) {
        if( !m_dcel.m_fulledge.contains( edge[i] ) ) continue;
        if( length[i] > ( m_param.m_longScale * m_targetLength ) ) {
            if( !m_subHandler.splitFullEdge( edge[i] ) ) {
                exitStatus = FULLEDGE_NOT_SPLITTED;
                return false;
            }
        }

        if( length[i] < ( m_param.m_shortScale * m_targetLength ) ) {
            if( !m_subHandler.collapseFullEdge( edge[i] ) ) {
                exitStatus = FULLEDGE_NOT_COLLAPSED;
                return false;
            }
        }
    }
    exitStatus = NO_ERROR;
    return true;
}



uint BK2004::edgeValence( const FullEdge_ptr& fe, const bool flip ) const {
    uint valence = 0.0;
    int dv[4];
    dv[0] = dOptimalValence( fe->HE( 0 )->V() );
    dv[1] = dOptimalValence( fe->HE( 1 )->V() );
    dv[2] = dOptimalValence( fe->HE( 0 )->Prev()->V() );
    dv[3] = dOptimalValence( fe->HE( 1 )->Prev()->V() );
    for( uint i = 0; i < 4; ++i ) {
        dv[i] += ( flip ) ? ( ( ( i / 2 ) * 2 ) + -1 ) : 0.0;
        valence += dv[i] * dv[i];
    }
    return valence;
}



bool BK2004::flip( uint& exitStatus ) {
    const uint size = m_dcel.m_fulledge.size();
    for( uint i = 0; i < size; ++i ) {
        FullEdge_ptr fe = m_dcel.m_fulledge[i];
        if( edgeValence( fe, false ) > edgeValence( fe, true ) ) {
            if( !m_subHandler.flipFullEdge( fe->idx ) ) {
                exitStatus = FULLEDGE_NOT_FLIPPED;
                return false;
            }
        }
    }
    exitStatus = NO_ERROR;
    return true;
}



Vector3 BK2004::g( const Vertex_pr& v ) const {
    Scalar areaSum = 0.0;
    Vector3 tmp;
    tmp.setZero();
    auto ring = Dcel::oneRing( v );
    for( auto p : ring ) {
        Scalar area = Dcel::mixedArea( *p );
        areaSum += area;
        tmp += area * p->P();
    }
    return ( tmp / areaSum );
}



Vector3 BK2004::tangentialRelaxation( const Vertex_ptr& v ) const {
    return ( v->P() + ( param.m_lambda * ( Matrix3::Identity() - ( v->N() * v->N().transpose() )  ) * ( g( v ) - v->P() ) ) );
}



void BK2004::smoothing() {
    const uint size = m_dcel.m_vertex.size();
    std::vector< Scalar > point( size );
    for( uint smooth_iter = 0; smooth_iter < m_param.m_smoothingIteration; ++smooth_iter ) {
        for( uint i = 0; i < size; ++i ) {
            point[i] = tangentialRelaxation( m_dcel.m_vertex[i] );
        }
        for( uint i = 0; i < size; ++i ) {
            m_dcel.m_vertex[i]->setP( point[i] );
        }
    }
}


} // namespace Core
} // namespace Ra


#endif // RADIUMENGINE_REMESHING_HANDLER_DEFINITION_HPP
