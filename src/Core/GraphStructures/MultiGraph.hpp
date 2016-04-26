#ifndef RADIUMENGINE_MULTIGRAPH_H
#define RADIUMENGINE_MULTIGRAPH_H

#include <memory>
#include <vector>

namespace Ra
{
    namespace Core
    {

        /// @brief MultiGraph is an oriented and acyclic graph
        /// whose edges are represented by each node knowing the whole
        /// list of its childrens.
        /// @note Usually the operators assume there is a unique source
        template <typename T>
        class MultiGraph
        {

            class Node
            {

                /// this is used to represent a connection from a local slot to
                /// a the dest slot of node target
                struct Connection
                {
                    uint m_local;
                    T*   m_target;
                    uint m_dest;
                };

            public:
                Node(const T& data, uint n_in, uint n_out);
                ~Node() {}

                /// connect the local slot to other's slot
                void setParent  (uint local, const T* other, uint slot);
                void setChildren(uint local, const T* other, uint slot);

                /// disconnect other's slot from local if it was previously connected
                void removeParent  (uint local, const T* other, uint slot);
                void removeChildren(uint local, const T* other, uint slot);
//                void disconnect(const Connection& c);

            public:
                std::shared_ptr<T>      m_data;    /// data stored by the node
                std::vector<Connection> m_childs;  /// edges starting from this
                std::vector<Connection> m_parents; /// edges ending to this

                uint m_deps; /// level over the graph source

            private:
                uint m_nIn;
                uint m_nOut;
            };

        public:
            MultiGraph();
            MultiGraph(const std::vector<T>& other);
            ~MultiGraph() {}

            /// update the nodes' levels
            /// default refers to the first element in m_graph
            //void levelize(uint i=0);

        private:
            std::vector<T> m_graph;
        };

    }
}

#endif // RADIUMENGINE_MULTIGRAPH_H
