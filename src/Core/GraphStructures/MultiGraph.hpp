#ifndef RADIUMENGINE_MULTIGRAPH_H
#define RADIUMENGINE_MULTIGRAPH_H

#include <iostream>

#include <algorithm>
#include <memory>
#include <vector>
#include <map>

#include <Engine/Renderer/RenderTechnique/RenderParameters.hpp>

namespace Ra
{
    namespace Core
    {

        enum graphstatus {GRAPH_VALID=0, GRAPH_ERROR, GRAPH_UPDATE};

        /// @brief MultiGraph is an oriented and acyclic graph
        /// whose edges are represented by each node knowing the whole
        /// list of its parents.
        /// @note Usually the operators assume there is a unique source
        template <typename T>
        class MultiGraph
        {

        public:
            class Node
            {
            public:
                /// @brief this is used to represent a connection from the source's m_slot
                /// to the local slot m_local
                struct Connection
                {
                    uint  m_slot;
                    Node* m_source;
                    uint  m_local;
                    bool  operator==(const Connection& other) const;
                };

            public:
                Node(MultiGraph<T>* host, const std::string& name,
                     const std::shared_ptr<T>& data, uint nbIn, uint nbOut);

                ~Node() {}

                /// @brief connect the local slot to other's slot
                /// @param reverse is it needed to create the connection backward
                void setParent(uint local, Node* other, uint slot, bool reverse = true);

                /// @brief disconnect other's slot from local if it was previously connected
                void removeParent(uint local, Node* other, uint slot, bool reverse = true);

                /// @brief connect the local slot to other's slot
                void setChild(uint local, Node* other, uint slot, bool reverse = true);

                /// @brief disconnect other's slot from local if it was previously connected
                void removeChild(uint local, Node* other, uint slot, bool reverse = true);

                /// @brief update every parent parameters
                void updateParameters() const;

                /// @brief get input slot's name
                const std::string& getSlotNameIn(uint slot);

                /// @brief get output slot's name
                const std::string& getSlotNameOut(uint slot);

                /// @brief update the level attribute of this if parents are all positively levelled
                void updateLevel();

                /// @brief debug indicator - TODO(hugo) remove this
                void print() const;

                /// @brief for sorting
                bool operator<(const Node& other) const;


            public:
                std::shared_ptr<T>      m_data;    /// data stored by the node
                std::vector<Connection> m_parents; /// parents of the node
                std::vector<Connection> m_childs;  /// childs  of the node
                std::string m_name;
                uint m_level;
                uint m_nbIn;
                uint m_nbOut;

            private:
                MultiGraph<T>* m_graph;

            }; // end of subclass Node

        public:
            MultiGraph() {}
            MultiGraph(const MultiGraph& other)
                : m_graph(other.m_graph)
                , m_names(other.m_names)
                , m_status(GRAPH_VALID)
            {}
            ~MultiGraph() {}

            /// @brief add a node to the graph
            void addNode(const std::string& name, const std::shared_ptr<T>& data, uint nb_in, uint nb_out);

            /// @brief delete a node from the graph
            /// @note handles the connections removal
            void deleteNode(Node* node);

            /// @brief update parameters of every node
            void updateNodes() const;

            /// @brief update the nodes' levels taking in account every source
            void levelize(bool sortByLevel = false);

            /// @brief debug indicator - TODO(hugo) remove this
            void print() const;

            /// @brief access a node pointer directly through the graph
            Node* operator[](const std::string& name);
            Node* operator[](const int index);

            /// @brief eedefine iterator begin to map over m_graph
            typename std::vector<std::unique_ptr<Node>>::iterator begin();

            /// @brief redefine iterator end to map over m_graph
            typename std::vector<std::unique_ptr<Node>>::iterator end();

            /// @brief redefine size() function
            uint size() const;

            /// @brief connection function pointer
            bool (*m_connect)(T* a, uint ia, T* b, uint ib);

            /// @brief fetch names
            const std::string& (*m_slotname_in )(T* node, uint slot);
            const std::string& (*m_slotname_out)(T* node, uint slot);

        public:
            int m_status;

        private:
            std::vector<std::unique_ptr<Node>> m_graph;
            std::map<std::string, Node*> m_names;

        }; // end of class MultiGraph

        #include "MultiGraph.tpp"

    }
}

#endif // RADIUMENGINE_MULTIGRAPH_H
