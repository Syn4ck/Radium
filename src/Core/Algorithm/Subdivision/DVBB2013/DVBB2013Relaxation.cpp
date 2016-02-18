#include <Core/Algorithm/Subdivision/DVBB2013/DVBB2013Relaxation.hpp>

#include <Core/Geometry/Normal/Normal.hpp>
#include <Core/Algorithm/Subdivision/DVBB2013/DVBB2013Smoothing.hpp>
#include <Core/Algorithm/Subdivision/DVBB2013/DVBB2013SizingValue.hpp>

namespace Ra {
namespace Core {

/// CONSTRUCTOR
DVBB2013RelaxationParameters::DVBB2013RelaxationParameters( const uint   iteration,
                                                            const Scalar eps ) :
    m_iteration( iteration ),
    m_eps( eps ) { }


/// CONSTRUCTOR
DVBB2013Relaxation::DVBB2013Relaxation( TriangleMesh*                       mesh,
                                        const DVBB2013RelaxationParameters& param,
                                        const bool                          verbosity ) :
    Algorithm< DVBB2013RelaxationParameters >( param, "DVBB2013 Tangential Relaxation", verbosity ),
    m_mesh( mesh ) { }



/// DESTRUCTOR
DVBB2013Relaxation::~DVBB2013Relaxation() { }



/// ALGORITHM STAGE
bool DVBB2013Relaxation::configCheck( uint& exitStatus ) {
    const bool status = ( m_mesh != nullptr );
    if( !status ) {
        exitStatus = MESH_NULLPTR;
    }
    return status;
}



bool DVBB2013Relaxation::preprocessing( uint& exitStatus ) {
    // Check the positions
    for( const auto& v : m_mesh->m_vertices ) {
        if( !v.allFinite() ) {
            exitStatus = INVALID_POINT;
            return false;
        }
    }
    return true;
}



bool DVBB2013Relaxation::processing( uint& exitStatus ) {
    const uint size = m_mesh->m_vertices.size();

    //Geometry::AdjacencyMatrix Adj = Geometry::uniformAdjacency( m_mesh->m_vertices, m_mesh->m_triangles );
    Centroid                  C;
    VectorArray< Vector3 >    tmp( size );
        // Compute the normals
        Geometry::uniformNormal( m_mesh->m_vertices, m_mesh->m_triangles, m_mesh->m_normals );

        // Check the normals
        for( const auto& n : m_mesh->m_normals ) {
            if( !n.allFinite() ) {
                exitStatus = INVALID_NORMAL;
                return false;
            }
        }

        VectorArray< Scalar > Lx;
        sizingValueVertex( m_mesh->m_vertices, m_mesh->m_triangles, m_param.m_eps, Lx );

        // Check the sizing values
        for( const auto& value : Lx ) {
            if( !std::isfinite( value ) ) {
                exitStatus = INVALID_SIZING_VALUE;
                return false;
            }
        }


        // Compute the centroid
        c( m_mesh->m_vertices, m_mesh->m_triangles, Lx, C );

        // Check the centroids
        for( const auto& centroid : C ) {
            if( !centroid.allFinite() ) {
                exitStatus = INVALID_CENTROID;
                return false;
            }
        }
    for( uint smooth_iter = 0; smooth_iter < m_param.m_iteration; ++smooth_iter ) {

        // Compute the relaxation
        tangentialRelaxation( m_mesh->m_vertices, m_mesh->m_normals, C, tmp );

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



bool DVBB2013Relaxation::postprocessing( uint& exitStatus ) {
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
