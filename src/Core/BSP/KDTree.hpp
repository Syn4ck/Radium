#ifndef RADIUMENGINE_CORE_KDTREE_HPP_
#define RADIUMENGINE_CORE_KDTREE_HPP_
#include <Core/RaCore.hpp>
#include <Core/Containers/VectorArray.hpp>
#include <Core/Math/LinearAlgebra.hpp>
#include <Core/BSP/BSP.hpp>

namespace Ra
{
    namespace Core
    {
        class KDNode
        {
        public:
            KDNode(int depth)
                :m_is_leaf(true), m_depth(depth) {}

            KDNode(const KDNode & n)
                :m_is_leaf(n.m_is_leaf), m_depth(n.m_depth) {}

            int depth() const { return m_depth; }

            void setIsNotLeaf() { m_is_leaf = false ; }

            void addPrimitives(std::vector<Vector3> data) { m_points.insert(m_points.end(), data.begin(), data.end()) ; }

        public:
            std::vector<Vector3> m_points;
            bool m_is_leaf;
            int m_depth;
            Aabb m_aabb;
        };

        class KDTree : public BSP
        {
        public:

            bool build(const VectorArray<Vector3> & data) override;

            void clear() override;

        protected:
            std::vector<KDNode> m_nodes;
            int m_dim = 3;
            int m_prims_per_leaf = 3;
        };
    }
}

#include <Core/BSP/KDTree.inl>

#endif // RADIUMENGINE_CORE_KDTREE_HPP_
