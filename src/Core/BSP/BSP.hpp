#ifndef RADIUMENGINE_CORE_BSP_HPP_
#define RADIUMENGINE_CORE_BSP_HPP_
#include <Core/RaCore.hpp>
#include <Core/Containers/VectorArray.hpp>
#include <Core/Math/LinearAlgebra.hpp>

namespace Ra
{
    namespace Core
    {
        /// Class set to handle Binary Space Partitioning

        // Data need to give some point information of some way to partition so : template or interface or none ?
        //TODO template <typename _Data>
        class BSP
        {
        public:

            RA_CORE_ALIGNED_NEW

            /*using Data                      = _Data;
            static constexpr int dim        = Data::Cols;
            static_assert(dim == 3, "BSP dimension invalid");*/

            /// Build the BSP based on the given data
            /// Return false if the tree couldn't be built, true else.
            virtual bool build(const VectorArray<Vector4> & data) = 0;

            virtual void clear() = 0;



        protected:

            Aabb m_root_aabb;
        };
    }
}

#endif //RADIUMENGINE_CORE_BSP_HPP_
