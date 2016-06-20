#ifndef IMGUI_NODE_GRAPH_HPP
#define IMGUI_NODE_GRAPH_HPP

#include <ImGuiNodeProp.hpp>

namespace ImGui {

    /**
     *
     * \class GraphViewer
     *
     * @brief ImGui Widget to view and edit the post-processing graph.
     *
     * This class will build a graphical representation of the graph he
     * was passed at instantiation. Each node of the graph will be
     * represented by a @see NodeProp which will handle the drawing, with
     * positions and other needed informations.
     *
     * It will also manage the dragging system to provide a
     * blender-like-but-not-so-cool interaction.
     *
     * It is also responsible for deleting/creating the nodes.
     *
     */

    template <typename T>
    class GraphViewer
    {

        ///{@
        /// Dragging management.
        enum dragtype : unsigned int {DRAG_NONE=0, DRAG_NODE, DRAG_SLOT};
        enum dragside : unsigned int {DRAG_OUT=0, DRAG_IN};
        ///@}

        /// \struct DragStruct
        /// Contains the whole dragging state for imgui to remember.
        static struct DragStruct {
            dragtype     m_type;    // determines wether we are dragging node or link
            NodeProp<T>* m_node;    // the node concerned with current dragging
            NodeProp<T>* m_last;    // the last dragged node, to delete for example
            int          m_slot;    // the slot associated when DRAG_SLOT
            int          m_side;    // the side of the slot (input/output)
        } dragstate;

    public:
        /// Constructor.
        GraphViewer(Ra::Core::MultiGraph<T>* gr = nullptr) : m_gr(gr) {}
        ~GraphViewer() {}

        /// @brief create the graphical representation from the graph.
        ///
        /// Initiate the graph representation with current graph
        /// it is assumed that nothing will modify the graph structure
        /// outside of the viewer:
        ///
        /// This enables the grah representation to parse the graph only
        /// once and to update himself automatically while telling the
        /// graph structure to change.
        void Init();

        /// Window that displays nodes and links.
        /// @warning The init() must have been called prior to this.
        void Show( bool* open = nullptr );

        /// Window used as Node factory.
        void NodeCreator( bool* open );

        /// Display link between 2 nodes.
        void drawLink( NodeProp<T>& node_a, unsigned int slot_a, NodeProp<T>& node_b, unsigned int slot_b );

        /// Create a <br>new</br> link.
        void createLink();

        /// Create a <br>new</br> NodeProp with attributes of the Multigraph::Node passed.
        void createNode( typename Ra::Core::MultiGraph<T>::Node* node );

        /// Remove a node.
        void removeNode( NodeProp<T>* prop );

    private:
        /// Find a node or slot the mouse would be hovered.
        /// @return true if a slot was found
        /// @param node is used to locate which node was hit
        /// @param slot is used to know the slot selected
        /// @param side indicates if DRAG_IN or DRAG_OUT
        /// @warning node could be filled even if return is false, if the node was selected
        bool findMouseSlot( NodeProp<T>** node, unsigned int* slot, unsigned int* side );

        /// Update the dragging state.
        inline void updateDragging();

        /// Update the position of a node with dragging status.
        void updateNodePos(NodeProp<T>* prop);

        /// Draw a Hermite spline.
        /// @note This function was written by
        /// <a href="https://gist.github.com/emoon/b8ff4b4ce4f1b43e79f2">emoon</a>
        void draw_hermite( ImDrawList* draw_list, ImVec2 p1, ImVec2 p2, int STEPS,
                           const ImColor &col, float thickness = 2.f );

    private:
        /// The graph structure.
        Ra::Core::MultiGraph<T>* m_gr;

        /// The representation we build from this graph structure.
        std::vector<std::unique_ptr<NodeProp<T>>> m_props;

        /// Hashmap to retrieve nodeProp from node.
        std::map<typename Ra::Core::MultiGraph<T>::Node*, NodeProp<T>*> m_reference;

        static int propsIds;

    }; // end of MultiGraphViewer

} // namespace ImGui

#include "ImGuiGraphViewer.tpp"

#endif // IMGUI_NODE_GRAPH_HPP
