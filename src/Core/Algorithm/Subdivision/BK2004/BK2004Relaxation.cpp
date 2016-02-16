#include <Core/Algorithm/Subdivision/BK2004/BK2004Relaxation.hpp>

#include <Core/Geometry/Normal/Normal.hpp>
#include <Core/Algorithm/Subdivision/BK2004/BK2004Smoothing.hpp>

#include <Core/Math/ColorPresets.hpp>
#include <Engine/SystemDisplay/SystemDisplay.hpp>

namespace Ra {
namespace Core {

/// CONSTRUCTOR
BK2004RelaxationParameters::BK2004RelaxationParameters( const uint   iteration,
                                                        const Scalar lambda ) :
    m_iteration( iteration ),
    m_lambda( lambda ) { }


/// CONSTRUCTOR
BK2004Relaxation::BK2004Relaxation( TriangleMesh*                     mesh,
                                    const BK2004RelaxationParameters& param,
                                    const bool                        verbosity ) :
    Algorithm< BK2004RelaxationParameters >( param, "BK2004 Tangential Relaxation", verbosity ),
    m_mesh( mesh ) { }



/// DESTRUCTOR
BK2004Relaxation::~BK2004Relaxation() { }



/// ALGORITHM STAGE
bool BK2004Relaxation::configCheck( uint& exitStatus ) {
    const bool   meshStatus = ( m_mesh != nullptr );
    const bool lambdaStatus = ( ( m_param.m_lambda >= 0.0 ) && ( m_param.m_lambda <= 1.0 ) );
    const bool       status = meshStatus && lambdaStatus;
    if( !status ) {
        if( !meshStatus ) {
            exitStatus = MESH_NULLPTR;
        } else {
            if( !lambdaStatus ) {
                exitStatus = LAMBDA_OUT_OF_RANGE;
            }
        }
    }
    return status;
}



bool BK2004Relaxation::preprocessing( uint& exitStatus ) {
    // Check the positions
    for( const auto& v : m_mesh->m_vertices ) {
        if( !v.allFinite() ) {
            exitStatus = INVALID_POINT;
            return false;
        }
    }
    return true;
}



bool BK2004Relaxation::processing( uint& exitStatus ) {
    const uint size = m_mesh->m_vertices.size();

    Geometry::AdjacencyMatrix Adj = Geometry::uniformAdjacency( m_mesh->m_vertices, m_mesh->m_triangles );
    GravityCentroid           G;
    VectorArray< Vector3 >    tmp( size );
    for( uint smooth_iter = 0; smooth_iter < m_param.m_iteration; ++smooth_iter ) {
        // Compute the normals
        Geometry::uniformNormal( m_mesh->m_vertices, m_mesh->m_triangles, m_mesh->m_normals );

        // Check the normals
        for( const auto& n : m_mesh->m_normals ) {
            if( !n.allFinite() ) {
                exitStatus = INVALID_NORMAL;
                return false;
            }
        }

        // Compute the one-ring area
        Geometry::AreaMatrix A = Geometry::barycentricArea( m_mesh->m_vertices, m_mesh->m_triangles );

        // Check the areas
        if( !A.diagonal().allFinite() && ( A.diagonal().minCoeff() > 0.0 ) ) {
            exitStatus = INVALID_AREA;
        }

        // Compute the gravity centroid
        g( m_mesh->m_vertices, Adj, A, G );

        // Check the centroids
        for( uint i = 0; i < size; ++i ) {
        //for( const auto& centroid : G ) {
            //if( !centroid.allFinite() ) {
            if( !G[i].allFinite() ) {
                RA_DISPLAY_POINT( m_mesh->m_vertices[i], Colors::Red(), 1.0 );
                exitStatus = INVALID_CENTROID;
                return false;
            }
        }

        // Compute the relaxation
        tangentialRelaxation( m_mesh->m_vertices, m_mesh->m_normals, G, m_param.m_lambda, tmp );

        // Check the positions
        for( const auto& v : tmp ) {
            if( !v.allFinite() ) {
                exitStatus = INVALID_POINT;
                return false;
            }
        }

        std::swap( m_mesh->m_vertices, tmp );
    }

    return true;
}



bool BK2004Relaxation::postprocessing( uint& exitStatus ) {
    // Compute the normals
    Geometry::uniformNormal( m_mesh->m_vertices, m_mesh->m_triangles, m_mesh->m_normals );

    // Check the normals
    for( const auto& n : m_mesh->m_normals ) {
        if( !n.allFinite() ) {
            exitStatus = INVALID_NORMAL;
            return false;
        }
    }

    return true;
}


} // namespace Core
} // namespace Ra

