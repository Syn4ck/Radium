#ifndef IMGUI_NODE_GRAPH_HPP
#define IMGUI_NODE_GRAPH_HPP

#include <imgui.h>

#include <Core/GraphStructures/MultiGraph.hpp>


/**
 * @brief define some functions to represent a node widget and
 * useful things like spline rendering
 */

namespace ImGui {

    template <typename T>
    class GraphViewer
    {

    /// @brief the structure used to represent the graphic properties of a node
    struct NodeProp {
        const typename Ra::Core::MultiGraph<T>::Node* m_node;

        int    m_id;
        char   m_name[32];
        int    m_levelx, m_levely;
        ImVec2 m_pos   , m_size;
        int    m_nbout , m_nbin;

        /// @brief instantiate the struct required to draw a node on screen
        NodeProp(const typename Ra::Core::MultiGraph<T>::Node* node, int level,
                 const char* name, int nbin, int nbout)
            : m_node  ( node   )
            , m_levelx( level  )
            , m_size  ( 96, 48 )
            , m_nbout ( nbout  )
            , m_nbin  ( nbin   )
        {
            m_id = propsIds ++;
            strncpy(m_name, name, 32);
        }

        /// @brief estimate the best position of a node
        /// @note dumb method: place every node on a pseudo-grid
        void setAutoPos(unsigned int levelx, unsigned int levely)
        {
            m_pos.x = (levelx+1)*60.f + ((levelx) * m_size.x);
            m_pos.y = (levely+1)*60.f + ((levely) * m_size.y);
        }
    }; // end of NodeProp

    public:
        /// constructor
        GraphViewer(Ra::Core::MultiGraph<T>* gr = nullptr) : m_gr(gr) {}
        ~GraphViewer() {}

        /// @brief window that displays nodes and links
        /// @warning init() must have been called prior to this
        void Show( bool* opened = nullptr );

        /// @brief initiate the graph representation with current graph
        /// it is assumed that nothing will modify the graph structure
        /// outside of the viewer:
        /// this enables the grah representation to parse the graph only
        /// once and to update himself automatically while telling the
        /// graph structure to change
        void Init();

        /// @brief one node display
        void drawNode( const NodeProp& prop );

        /// @brief display link between 2 nodes
        void drawLink( const NodeProp& node_a, unsigned int slot_a, const NodeProp& node_b, unsigned int slot_b );

        /// @brief create a <br>new</br> node
        void createNode( const NodeProp& props );

        /// @brief create a <br>new</br> link
        void createLink( const NodeProp& prop_a, unsigned int slot_a, const NodeProp& node_b, unsigned int slot_b );

    private:
        /// @brief get input slot position
        /// @warning the coordinates are unaware of window offset
        ImVec2 getInputPos(  const NodeProp& node, unsigned int idx );

        /// @brief get output slot position
        /// @warning the coordinates are unaware of window offset
        ImVec2 getOutputPos( const NodeProp& node, unsigned int idx );

        /// @brief get slot position on y-axis
        float getSlotPosY( const NodeProp& info, unsigned int idx, unsigned int total );

        /// @brief draw a Hermite spline
        void draw_hermite( ImDrawList* draw_list, ImVec2 p1, ImVec2 p2, int STEPS,
                           const ImColor &col, float thickness = 2.f );

    private:
        /// @brief the graph structure
        Ra::Core::MultiGraph<T>* m_gr;

        /// @brief the representation we build from this graph structure
        std::vector<std::unique_ptr<NodeProp>> m_props;

        /// @brief hashmap to retrieve nodeProp from node
        std::map<typename Ra::Core::MultiGraph<T>::Node, NodeProp*> m_reference;

        static int propsIds;

    }; // end of MultiGraphViewer

} // namespace ImGui

#include "ImGuiNodeGraph.tpp"

#endif // IMGUI_NODE_GRAPH_HPP
