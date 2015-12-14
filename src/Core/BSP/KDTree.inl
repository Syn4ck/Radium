#include <Core/BSP/KDTree.hpp>

#include <Core/Log/Log.hpp>
#include <Core/Time/Timer.hpp>

#include <algorithm>
#include <stack>

namespace Ra
{
    namespace Core
    {
        struct FindMedian : public std::binary_function<int, int, bool> {
        public :
            inline FindMedian(const VectorArray<Vector3> & data, int axis)
                : m_data(data)
                , m_axis(axis) {}

            inline bool operator()(const int & i1, const int & i2) const {
                return m_data[i1][m_axis] < m_data[i2][m_axis] ;
            }

            const VectorArray<Vector3> & m_data;
            int m_axis;
        };


        inline bool KDTree::build(const VectorArray<Vector3f> & data) {

            long sorting_time = 0.0 ;

            Timer::Clock clock ;

            LOG(logWARNING) << "Building KDTree for " << data.size() << " 3D-points ..." ;
            Timer::TimePoint start = clock.now() ;

            // Stack of known nodes left to compute
            std::stack<KDNode *> stackNodes;

            // Begin with the root node
            m_nodes.push_back(KDNode(0));
            stackNodes.push(&( m_nodes.back() ));

            // List of indices to the data.
            // This indirection is used to sort the data, without switching heavy memory
            int* indices = new int[data.size()];
            for (int i = 0; i < data.size(); ++i)
            {
                indices[i] = i;
            }

            // Stack of pointers to the sorted indices, such that
            // data indexed by the range of indices between a couple <begin,end>
            // is contained in the equivalent node of stackNodes
            std::stack<int *> stackBegins;
            std::stack<int *> stackEnds;

            // Begin with the full range of data
            stackBegins.push(indices);
            stackEnds.push(indices+data.size());

            // TODO Switch for longest axis based on AABB ?
            int axis = 1 % m_dim; // For 3D : 0 is x-aligned, 1 is y-aligned and 2 is z-aligned

            while ( !stackNodes.empty() )
            {
                auto current = stackNodes.top();
                stackNodes.pop();
                auto begin = stackBegins.top();
                stackBegins.pop();
                auto end = stackEnds.top();
                stackEnds.pop();

                // Compute AABB for current node
                // TODO Is there any more optimal way to compute it based on parent's AABB ?
                for (auto it = begin; it != end; ++it) 
                {
                    current->m_aabb.extend(data[*it]);
                }

                int range = (end - begin);
                int median_range = range/2 ;

                // Sort the pointers toward the data
                Timer::TimePoint sort_start = clock.now() ;
                std::nth_element(begin, begin+median_range, end, FindMedian(data, axis)) ;
                sorting_time += Timer::getIntervalMicro(sort_start, clock.now()) ;

                // Create left node. Add to todo stack if it contains
                // too many primitives to be a leaf
                m_nodes.push_back( KDNode(current->depth()+1) ) ;
                auto left = &( m_nodes.back() ) ;

                if ( median_range > m_prims_per_leaf )
                {
                    left->setIsNotLeaf() ;
                    stackNodes.push(left);
                    stackBegins.push(begin);
                    stackEnds.push(begin+median_range);
                } 
                else 
                {
                    for (auto i = begin; i != begin+median_range; ++i)
                    {
                        left->addPrimitive(data[*i]) ;
                    }
                }

                // Create right node. Idem
                m_nodes.push_back( KDNode(current->depth()+1) ) ;
                auto right = &( m_nodes.back() ) ;

                if ( ( range - median_range ) > m_prims_per_leaf )
                {
                    right->setIsNotLeaf() ;
                    stackNodes.push(right);
                    stackBegins.push(begin+median_range);
                    stackEnds.push(end);
                }
                else 
                {
                    for (auto i = begin + median_range; i != end; ++i)
                    {
                        right->addPrimitive(data[*i]) ;
                    }
                }

                axis = ( axis + 1 ) % m_dim;
            }

            m_root_aabb = m_nodes[0].m_aabb ;

            LOG(logWARNING) << "KDTree built in " << Timer::getIntervalMicro(start, clock.now()) / 1000.0 << "ms, with "
                            << sorting_time / 1000.0 << "ms to sort. Data partitioned in " << m_nodes.size() << " nodes." ;

            return true;
        }

        inline void KDTree::clear() {
            m_nodes.clear();
        }

    }
}
