#include <Core/Algorithm/Subdivision/BK2004>

#include <Core/Index/Index.hpp>
#include <Core/Mesh/DCEL/FullEdge.hpp>
#include <Core/Mesh/DCEL/Dcel.hpp>
#include <Core/Mesh/DCEL/Operation/Valid.hpp>
#include <Core/Mesh/DCEL/Operation/Consistent.hpp>
#include <Core/Mesh/DCEL/Operation/Length.hpp>

namespace Ra {
namespace Core {

class uint;
class Scalar;
class Algorithm;


/// CONSTRUCTOR
BK2004Parameter::BK2004Parameter( const uint   algorithmIteration = 1,
                                  const uint   smoothingIteration = 1,
                                  const Scalar lambdaFactor       = 1.0 ) :
    m_algorithmIteration( algorithmIteration ),
    m_smoothingIteration( smoothingIteration ),
    m_lambda( lambdaFactor ) { }



/// CONSTRUCTOR
BK2004::BK2004( const Dcel_ptr&        dcel      = nullptr,
                const BK2004Parameter& param     = BK2004Parameter(),
                const bool             verbosity = false ) :
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

    // Compute normals

    exitStatus = NO_ERROR;
    return true;
}



bool BK2004::processing( uint& exitStatus ) override {
    const Scalar tooLong  = ( 4.0 / 3.0 ) * m_targetLength;
    const Scalar tooShort = ( 4.0 / 5.0 ) * m_targetLength;

    for( uint alg_iter = 0; alg_iter < m_param.m_algorithmIteration; ++alg_iter ) {
        Scalar size = m_dcel._fulledge.size();
        std::vector< Index >  edge( size );
        std::vector< Scalar > length;
        for( uint i = 0; i < size; ++i ) {
            edge[i] = m_dcel.m_fulledge[i]->idx;
        }
        FullEdgeLength( m_dcel, length );

        for( uint i = 0; i < size; ++i ) {
            if( length[i] > tooLong ) {
                if( !m_subHandler.splitFullEdge( edge[i] ) ) {
                    exitStatus = FULLEDGE_NOT_SPLITTED;
                    return false;
                }
            }

            if( length[i] < tooShort ) {
                if( !m_subHandler.collapseFullEdge( edge[i] ) ) {
                    exitStatus = FULLEDGE_NOT_COLLAPSED;
                    return false;
                }
            }
        }

        size = m_dcel.m_fulledge.size();
        for( uint i = 0; i < size; ++i ) {
            --- Check flip valence
            if( !m_subHandler.flipFullEdge( m_dcel.m_fulledge[i]->idx ) ) {
                exitStatus = FULLEDGE_NOT_FLIPPED;
                return false;
            }
        }

        if( !isConsistent( m_dcel ) ) {
            exitStatus = DCEL_NOT_CONSISTENT;
            return false;
        }

        --- compute normals

        for( uint smooth_iter = 0; smooth_iter < m_param.m_smoothingIteration; ++smooth_iter ) {
            // TO DO
        }

    }

    exitStatus = NO_ERROR;
    return true;
}



} // namespace Core
} // namespace Ra


#endif // RADIUMENGINE_REMESHING_HANDLER_DEFINITION_HPP
