#ifndef IMGUI_NODE_GRAPH_HPP
#define IMGUI_NODE_GRAPH_HPP

#include "imgui.h"

#include <Core/GraphStructures/MultiGraph.hpp>


/**
 * @brief define some functions to represent a node widget and
 * useful things like spline rendering (hermite ?)
 */

namespace ImGui {

    template <typename T>
    class GraphViewer
    {

    /// the structure used to represent the graphic properties of a node
    struct NodeProp {
        const typename T::Node* m_node;

        char   m_name[32];
        int    m_levelx, m_levely;
        ImVec2 m_pos   , m_size;
        int    m_nbout , m_nbin;

        NodeProp(const typename T::Node* node, int level, const char* name, int nbin, int nbout)
            : m_node  ( node   )
            , m_levelx( level  )
            , m_size  ( 96, 48 )
            , m_nbout ( nbout  )
            , m_nbin  ( nbin   )
        {
            strncpy(m_name, name, 32);
            autoPos(); // automatically place the node on screen
        }

    private:
        /// estimate the best position of a node (dumbiest possible method)
        void autoPos()
        {
            m_pos.x = (m_levelx+1)*60.f + (m_levelx * m_size.x);
            m_pos.y = (m_levely+1)*60.f + (m_levely * m_size.y);
        }
    }; // end of NodeProp

    public:
        /// constructor
        GraphViewer(const Ra::Core::MultiGraph<T>* gr) : m_gr(gr) {}
        ~GraphViewer() {}

        /// window that displays nodes and links
        /// @note: specialized ImGui::Begin() function
        void Begin( bool* opened );
        void End();

        /// one node display
        void drawNode( const NodeProp& prop );

        /// display link between 2 nodes
        void linkNode(const NodeProp& node_a, unsigned int slot_a, const NodeProp& node_b, unsigned int slot_b);

    private:
        /// get input/output slot position
        ImVec2 getInputPos(  const NodeProp& node, unsigned int idx );
        ImVec2 getOutputPos( const NodeProp& node, unsigned int idx );

        /// get slot position on y-axis
        float getSlotPosY( const NodeProp& info, unsigned int idx, unsigned int total );

        /// draw a Hermite spline
        void draw_hermite(ImDrawList* draw_list, ImVec2 p1, ImVec2 p2, int STEPS, const ImColor &col, float thickness = 2.f);

    private:
        /// the graph we draw
        const Ra::Core::MultiGraph<T>* m_gr;

    }; // end of MultiGraphViewer

#include "imgui_node_graph.tpp"

}

#endif // IMGUI_NODE_GRAPH_HPP
