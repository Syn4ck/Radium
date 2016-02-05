#include <Core/Mesh/DCEL/Operation/Extract.hpp>

#include <map>

#include <Core/Mesh/DCEL/Vertex.hpp>
#include <Core/Mesh/DCEL/HalfEdge.hpp>
#include <Core/Mesh/DCEL/FullEdge.hpp>
#include <Core/Mesh/DCEL/Face.hpp>

#include <Core/Mesh/DCEL/Iterator/Vertex/VVIterator.hpp>
#include <Core/Mesh/DCEL/Iterator/Vertex/VFIterator.hpp>
#include <Core/Mesh/DCEL/Iterator/Face/FVIterator.hpp>

namespace Ra {
namespace Core {

void extract( const Vertex_ptr& v, TriangleMesh& mesh ) {
    //mesh.clear();
    std::map< Index, uint > v_table;

    // Copy vertex
    mesh.m_vertices.push_back( v->P() );
    mesh.m_normals.push_back( v->N() );
    v_table[v->idx] = mesh.m_vertices.size() - 1;

    // Find the data
    VVIterator v_it( v );
    VFIterator f_it( v );
    auto  v_list =  v_it.list();
    auto  f_list =  f_it.list();

    // Insert the vertices
    for( const auto& ptr : v_list ) {
        mesh.m_vertices.push_back( ptr->P() );
        mesh.m_normals.push_back( ptr->N() );
        v_table[ptr->idx] = mesh.m_vertices.size() - 1;
    }

    // Insert the triangles
    for( const auto& ptr : f_list ) {
        Triangle T;
        FVIterator it( ptr );
        for( uint i = 0; i < 3; ++i ) {
            T[i] = v_table[it->idx];
            ++it;
        }
        mesh.m_triangles.push_back( T );
    }
}


void extract( const FullEdge_ptr& fe, TriangleMesh& mesh ) {
    std::map< Index, uint > v_table;
    // Insert the triangles
    for( uint i = 0; i < 2; ++i ) {
        if( fe->F( i ) == nullptr ) continue;

        Triangle T;
        FVIterator it( fe->F( i ) );
        for( uint t = 0; t < 3; ++t ) {
            auto p = v_table.find( it->idx );
            if( p == v_table.end() ) {
                mesh.m_vertices.push_back( it->P() );
                mesh.m_normals.push_back( it->N() );
                v_table[it->idx] = mesh.m_vertices.size() - 1;
            }

            T[t] = v_table[it->idx];
            ++it;
        }
        mesh.m_triangles.push_back( T );
    }
}


void extract( const Face_ptr& f, TriangleMesh& mesh ) {
    //mesh.clear();
    Triangle T;
    FVIterator it( f );
    for( uint i = 0; i < 3; ++i ) {
        mesh.m_vertices.push_back( it->P() );
        mesh.m_normals.push_back( it->N() );
        T[i] = mesh.m_vertices.size() - 1;
        ++it;
    }
    mesh.m_triangles.push_back( T );
}

} // namespace Core
} // namespace Ra
