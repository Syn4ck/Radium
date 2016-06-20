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

        enum graphstatus {GRAPH_VALID=0, GRAPH_ERROR, GRAPH_UPDATE, GRAPH_NEW};

        /**
          * \class MultiGraph
          *
          * @brief MultiGraph is an oriented and acyclic graph whose nodes
          * contain T objects.
          *
          * The multigraph is a structure to store T objects as graph nodes.
          * The graph could have multiple edges.
          *
          * Each node is a wrapper for a T objects that manages memory for it,
          * and add a children and parents lists.
          *
          * The graph will use the nodes' levels to sort the node list.
          *
          */

        template <typename T>
        class RA_CORE_API MultiGraph
        {

        public:
            class RA_CORE_API Node
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
                     T* data, uint nbIn, uint nbOut);

                ~Node() {}

                /// Connect the local slot to other's slot.
                /// @param reverse is it needed to create the connection backward
                void setParent(uint local, Node* other, uint slot, bool reverse = true);

                /// Disconnect other's slot from local if it was previously connected.
                void removeParent(uint local, Node* other, uint slot, bool reverse = true);

                /// Connect the local slot to other's slot.
                void setChild(uint local, Node* other, uint slot, bool reverse = true);

                /// Disconnect other's slot from local if it was previously connected.
                void removeChild(uint local, Node* other, uint slot, bool reverse = true);

                /// Update every parent parameters.
                void updateParameters() const;

                /// Get input slot's name.
                const std::string& getSlotNameIn(uint slot);

                /// Get output slot's name.
                const std::string& getSlotNameOut(uint slot);

                /// Update the level attribute of this if parents are all positively levelled.
                void updateLevel();

                /// Compare the nodes with their levels.
                bool operator<(const Node& other) const;


            public:
                std::unique_ptr<T>      m_data;    ///< Data stored by the node.
                std::vector<Connection> m_parents; ///< Parents of the node.
                std::vector<Connection> m_childs;  ///< Childs  of the node.
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

            /// Add a node to the graph.
            Node* addNode(T* data);

            /// Delete a node from the graph.
            /// @note handles the connections removal
            void deleteNode(Node* node);

            /// Update parameters of every node.
            void updateNodes() const;

            /// Update the nodes' levels taking in account every source.
            void levelize(bool sortByLevel = false);

            ///{@
            /// Access a node pointer directly through the graph.
            Node* operator[](const std::string& name);
            Node* operator[](const int index);
            ///@}

            /// Redefine iterator begin to map over m_graph.
            typename std::vector<std::unique_ptr<Node>>::iterator begin();

            /// Redefine iterator end to map over m_graph.
            typename std::vector<std::unique_ptr<Node>>::iterator end();

            /// Redefine size() function.
            uint size() const;

            /// Redefine non-const access to the last node.
            T& back();

            /// Connection function pointer.
            bool (*m_connect)(T* a, uint ia, T* b, uint ib);

            ///{@
            /// Get the name associated with a slot for T type
            /// @warning To be defined externally
            const std::string& (*m_slotname_in )(T* node, uint slot);
            const std::string& (*m_slotname_out)(T* node, uint slot);
            ///@}

        public:
            /// Keep track of the status of the graph:
            /// (updated, valid, error, ...)
            int m_status;

        private:
            std::vector<std::unique_ptr<Node>> m_graph;
            std::map<std::string, Node*>       m_names;

        }; // end of class MultiGraph

        #include "MultiGraph.tpp"

    }
}

#endif // RADIUMENGINE_MULTIGRAPH_H
