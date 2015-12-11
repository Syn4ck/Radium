#include <Core/BSP/KDTree.hpp>

#include <Core/Log/Log.hpp>
#include <Core/Time/Timer.hpp>

#include <algorithm>

namespace Ra
{
    namespace Core
    {
        struct FindMedian : public std::binary_function<int, int, bool> {
        public :
            inline FindMedian(const VectorArray<Vector3> & data, int axis)
                :m_data(data), m_axis(axis) {}

            inline bool operator()(const int & i1, const int & i2) const {
                return m_data[i1][m_axis] < m_data[i2][m_axis] ;
            }

            const VectorArray<Vector3> & m_data;
            int m_axis;
        };


        inline bool KDTree::build(const VectorArray<Vector3> & data) {

            int nb_nodes_mini = data.size()/m_prims_per_leaf ;
            long sorting_time = 0.0 ;

            LOG(logWARNING) << "Building KDTree for " << data.size() << " 3D-points ..." ;
            Timer::TimePoint start ;

            // Stack of known nodes left to compute
            std::vector<KDNode *> stackNodes(nb_nodes_mini);

            // Begin with the root node
            m_nodes.push_back(KDNode(0));
            stackNodes.push_back(&m_nodes.back());

            // List of indices to the data.
            // This indirection is used to sort the data, without switching heavy memory
            std::vector<int> indices(data.size());
            for (int i = 0; i < data.size(); ++i)
                indices[i] = i;

            // Stack of pointers to the sorted indices, such that
            // data indexed by the range of indices between a couple <begin,end>
            // is contained in the equivalent node of stackNodes
            std::vector<std::vector<int>::iterator> stackBegins(nb_nodes_mini);
            std::vector<std::vector<int>::iterator> stackEnds(nb_nodes_mini);

            // Begin with the full range of data
            stackBegins.push_back(indices.begin());
            stackEnds.push_back(indices.end());

            // TODO Switch for longest axis based on AABB ?
            int axis = 1 % m_dim; // For 3D : 0 is x-aligned, 1 is y-aligned and 2 is z-aligned

            while (!stackNodes.empty()) {
                auto current = stackNodes.back();
                stackNodes.pop_back();
                auto begin = stackBegins.back();
                stackBegins.pop_back();
                auto end = stackEnds.back();
                stackEnds.pop_back();
LOG(logWARNING) << "Stacks poped (y)" ;

                // Compute AABB for current node
                // TODO Is there any more optimal way to compute it based on parent's AABB ?
                for (auto it = begin; it != end; ++it) {
LOG(logWARNING) << *it ;
                    current->m_aabb.extend(data[*it]);
                }
LOG(logWARNING) << "AABB computed." ;

                int range = (end - begin) ;
                int median_range = range/2 ;

LOG(logWARNING) << "Gotta sort'em all." ;
                // Sort the pointers toward the data
                Timer::TimePoint sort_start ;
                std::nth_element(begin, begin+median_range, end, FindMedian(data, axis)) ;
                sorting_time += Timer::getIntervalMicro(sort_start, Timer::TimePoint()) ;
LOG(logWARNING) << "Very best !" ;

                // Create left node. Add to todo stack if there are more
                m_nodes.push_back(KDNode(current->depth()+1)) ;
                if (median_range > m_prims_per_leaf) {
                    m_nodes.back().setIsNotLeaf() ;
                    stackNodes.push_back(&m_nodes.back());
                    stackBegins.push_back(begin);
                    stackEnds.push_back(begin+median_range);

LOG(logWARNING) << "Pushed left node between " << *(begin) << " and " << *(begin+median_range) ;

                } else {

LOG(logWARNING) << "Created a left leaf with " << median_range << " primitives" ;

                    for (auto i = begin; i != begin+median_range; ++i)
                        m_nodes.back().addPrimitive(data[*i]) ;
                }


                m_nodes.push_back(KDNode(current->depth()+1)) ;
                if ((range-median_range) > m_prims_per_leaf) {
                    m_nodes.back().setIsNotLeaf() ;
                    stackNodes.push_back(&m_nodes.back());
                    stackBegins.push_back(begin+median_range);
                    stackEnds.push_back(end);

LOG(logWARNING) << "Pushed right node between " << *(begin+median_range) << " and " << *(end) ;

                } else {

LOG(logWARNING) << "Created a right leaf with " << (range-median_range) << " primitives" ;

                    for (auto i = begin+median_range; i != end; ++i)
                        m_nodes.back().addPrimitive(data[*i]) ;
                }

                axis += 1 % m_dim;
            }

            m_root_aabb = m_nodes[0].m_aabb ;

            LOG(logWARNING) << "KDTree built in " << Timer::getIntervalMicro(start, Timer::TimePoint()) << "ms, with "
                            << sorting_time << "ms to sort. Data partitioned in " << m_nodes.size() << " nodes." ;

            return true;
        }

        inline void KDTree::clear() {
            m_nodes.clear();
        }

    }
}
