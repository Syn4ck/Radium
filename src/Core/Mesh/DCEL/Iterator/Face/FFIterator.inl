#include <Core/Mesh/DCEL/Iterator/Face/FFIterator.hpp>

#include <Core/Mesh/DCEL/HalfEdge.hpp>
#include <Core/Mesh/DCEL/Face.hpp>

namespace Ra {
namespace Core {



/// CONSTRUCTOR
FFIterator::FFIterator( const Face_ptr& f ) : FIterator< Face_ptr >( f ) { }



/// DESTRUCTOR
FFIterator::~FFIterator() { }



/// LIST
inline FaceList FFIterator::list() const {
    FaceList L;
    HalfEdge_ptr it = m_object->HE();
    do {
        L.push_back( it->F() );
        it = it->Next();
    } while( it != m_object->HE() );
    return L;
}



/// OPERATOR
inline Face_ptr FFIterator::operator->() const {
    return m_he->Twin()->F();
}

inline Face_ptr FFIterator::operator* () const {
    return m_he->Twin()->F();
}

} // namespace Core
} // namespace Ra
