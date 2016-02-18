#include <Core/Mesh/DCEL/Operation/Convert.hpp>

#include <map>
#include <utility>

#include <Core/Mesh/DCEL/Vertex.hpp>
#include <Core/Mesh/DCEL/HalfEdge.hpp>
#include <Core/Mesh/DCEL/FullEdge.hpp>
#include <Core/Mesh/DCEL/Face.hpp>
#include <Core/Mesh/DCEL/Dcel.hpp>


namespace Ra {
namespace Core {



void convert( const TriangleMesh& mesh, Dcel& dcel ) {
    typedef std::pair< Index, Index > Key_t;

    dcel.clear();
    // Create vertices
    for( uint i = 0; i < mesh.m_vertices.size(); ++i ) {
        const Vector3 p = mesh.m_vertices.at( i );
        const Vector3 n = mesh.m_normals.at( i );
        Vertex_ptr v = new Vertex( p, n );
        CORE_ASSERT( ( v != nullptr ), "Vertex_ptr == nullptr" );
        dcel.insert( v );
    }

    // Create HalfEdge table
    std::map< Key_t, HalfEdge_ptr > table;

    // Create the topology
    for( const auto& T : mesh.m_triangles ) {
        // Retrieve vertices
        Index id[3];
        Vertex_ptr v[3];
        for( uint i = 0; i < 3; ++i ) {
            id[i].setValue( T[i] );
            dcel.m_vertex.access( id[i], v[i] );
        }

        // Create face
        Face_ptr f = new Face();
        dcel.insert( f );

        // ------
        HalfEdge_ptr he[3];
        for( uint i = 0; i < 3; ++i ) {
            Key_t key[2];
            key[0].first  = id[i];
            key[0].second = id[( i + 1 ) % 3];
            key[1].first  = key[0].second;
            key[1].second = key[0].first;
            auto it = table.find( key[0] );

            if( it != table.end() ) {
                // Retrieve the halfedges
                he[i] = it->second;
                he[i]->setF( f );
                table.erase( it );

            } else {
                // Create halfedges
                he[i] = new HalfEdge();
                HalfEdge_ptr twin = new HalfEdge();
                FullEdge_ptr fe   = new FullEdge( he[i] );

                he[i]->setV( v[i] );
                he[i]->setTwin( twin );
                he[i]->setFE( fe );
                he[i]->setF( f );

                twin->setV( v[( i + 1 ) % 3] );
                twin->setTwin( he[i] );
                twin->setFE( fe );

                dcel.insert( he[i] );
                dcel.insert( twin );
                dcel.insert( fe );

                //table[key[0]] = he[i];
                table[key[1]] = twin;
            }
        }

        for( uint i = 0; i < 3; ++i ) {
            v[i]->setHE( he[i] );
            he[i]->setNext( he[( i + 1 ) % 3 ] );
            he[i]->setPrev( he[( i + 2 ) % 3 ] );
        }

        f->setHE( he[0] );
    }

    // Create hole chains if any
    if( !table.empty() ) {
        std::map< Index, HalfEdge_ptr > next;
        std::map< Index, HalfEdge_ptr > prev;
        for( const auto& it : table ) {
            next[ it.first.first  ] = it.second;
            prev[ it.first.second ] = it.second;
        }

        for( const auto& pair : table ) {
            HalfEdge_ptr he = pair.second;
            const Index id_prev = he->V()->idx;
            const Index id_next = he->Twin()->V()->idx;
            auto it = prev.find( id_prev );
            if( it != prev.end() ) {
                he->setPrev( it->second );
            }
            it = next.find( id_next );
            if( it != next.end() ) {
                he->setNext( it->second );
            }
        }
    }
}



void convert( const Dcel& dcel, TriangleMesh& mesh ) {
    const uint v_size = dcel.m_vertex.size();
    const uint f_size = dcel.m_face.size();
    mesh.m_vertices.resize( v_size );
    mesh.m_normals.resize( v_size );
    mesh.m_triangles.resize( f_size );
    std::map< Index, uint > v_table;
#pragma omp parallel for
    for( uint i = 0; i < v_size; ++i ) {
        const Vertex_ptr& v = dcel.m_vertex.at( i );
        const Vector3 p = v->P();
        const Vector3 n = v->N();
        mesh.m_vertices[i] = p;
        mesh.m_normals[i]  = n;
#pragma omp critical
        {
            v_table[ v->idx ] = i;
        }
    }

#pragma omp parallel for
    for( uint i = 0; i < f_size; ++i ) {
        const Face_ptr& f = dcel.m_face.at( i );
        Triangle T;
        T[0] = v_table[ f->HE()->V()->idx ];
        T[1] = v_table[ f->HE()->Next()->V()->idx ];
        T[2] = v_table[ f->HE()->Prev()->V()->idx ];
        mesh.m_triangles[i] = T;
    }
}



void convert( const Vertex_ptr& v, Vector3& p ) {
    p = v->P();
}



void convert( const VertexList& list, VectorArray< Vector3 >& point ) {
    point.clear();
    const uint size = list.size();
    point.reserve( size );
    for( const auto& item : list ) {
        if( item != nullptr ) {
            point.push_back( item->P() );
        }
    }
}






} // namespace Core
} // namespace Ra
