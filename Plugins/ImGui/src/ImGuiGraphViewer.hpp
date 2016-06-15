#ifndef IMGUI_NODE_GRAPH_HPP
#define IMGUI_NODE_GRAPH_HPP

#include <ImGuiNodeProp.hpp>

/**
 * @brief define some functions to represent a node widget and
 * useful things like spline rendering, and manage the viewer
 */

namespace ImGui {

    template <typename T>
    class GraphViewer
    {

        /// @brief dragging management
        enum dragtype : unsigned int {DRAG_NONE=0, DRAG_NODE, DRAG_SLOT};
        enum dragside : unsigned int {DRAG_OUT=0, DRAG_IN};

        /// @brief contains the whole dragging state for imgui to remember
        static struct DragStruct {
            dragtype     m_type;    // determines wether we are dragging node or link
            NodeProp<T>* m_node;    // the node concerned with current dragging
            NodeProp<T>* m_last;    // the last dragged node, to delete for example
            int          m_slot;    // the slot associated when DRAG_SLOT
            int          m_side;    // the side of the slot (input/output)
        } dragstate;

    public:
        /// constructor
        GraphViewer(Ra::Core::MultiGraph<T>* gr = nullptr) : m_gr(gr) {}
        ~GraphViewer() {}

        /// @brief initiate the graph representation with current graph
        /// it is assumed that nothing will modify the graph structure
        /// outside of the viewer:
        /// this enables the grah representation to parse the graph only
        /// once and to update himself automatically while telling the
        /// graph structure to change
        void Init();

        /// @brief window that displays nodes and links
        /// @warning init() must have been called prior to this
        void Show( bool* opened = nullptr );

        /// @brief display link between 2 nodes
        void drawLink( NodeProp<T>& node_a, unsigned int slot_a, NodeProp<T>& node_b, unsigned int slot_b );

        /// @brief create a <br>new</br> link
        void createLink();

        /// @brief create a <br>new</br> node with a small interface to ask shader name, parameters...
        void createNode();

        /// @brief remove a node
        void removeNode( NodeProp<T>* prop );

    private:
        /// @brief find a node and slot to connect to
        /// @return true if a slot was found
        /// @param node is used to locate which node was hit
        /// @param slot is used to know the slot selected
        /// @param side indicates if DRAG_IN or DRAG_OUT
        /// @warning node could be filled even if return is false, if the node was selected
        bool findMouseSlot( NodeProp<T>** node, unsigned int* slot, unsigned int* side );

        /// @brief update the dragging state
        inline void updateDragging();

        /// @brief update the position of a node with dragging status
        void updateNodePos(NodeProp<T>* prop);

        /// @brief draw a Hermite spline
        /// @note this function was written by
        /// <a href="https://gist.github.com/emoon/b8ff4b4ce4f1b43e79f2">emoon</a>
        void draw_hermite( ImDrawList* draw_list, ImVec2 p1, ImVec2 p2, int STEPS,
                           const ImColor &col, float thickness = 2.f );

    private:
        /// @brief the graph structure
        Ra::Core::MultiGraph<T>* m_gr;

        /// @brief the representation we build from this graph structure
        std::vector<std::unique_ptr<NodeProp<T>>> m_props;

        /// @brief hashmap to retrieve nodeProp from node
        std::map<typename Ra::Core::MultiGraph<T>::Node*, NodeProp<T>*> m_reference;

        static int propsIds;

    }; // end of MultiGraphViewer

} // namespace ImGui

#include "ImGuiGraphViewer.tpp"

#endif // IMGUI_NODE_GRAPH_HPP
