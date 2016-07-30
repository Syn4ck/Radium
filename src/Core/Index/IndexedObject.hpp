#ifndef RADIUMENGINE_INDEXEDOBJECT_HPP
#define RADIUMENGINE_INDEXEDOBJECT_HPP

#include <Core/RaCore.hpp>
#include <Core/Index/Index.hpp>

namespace Ra
{
    namespace Core
    {

        class DLL_EXPORT IndexedObject
        {
        public:
            /// CONSTRUCTOR
            explicit inline  IndexedObject( const Index& index = Index::INVALID_IDX() )
                : m_index(index)
            {
            }
            inline IndexedObject( const IndexedObject& id_obj )
            {
                m_index = id_obj.m_index;
            }

            virtual void setIndex(Index index)
            {
                m_index = index;
            }

            /// DESTRUCTOR
            virtual inline ~IndexedObject() { }

            /// VARIABLE
            Index m_index;
        };

    }
} // namespace Ra::Core

#endif // RADIUMENGINE_INDEXEDOBJECT_HPP
