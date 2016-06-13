#ifndef IMGUI_NODE_GRAPH_HPP
#define IMGUI_NODE_GRAPH_HPP

#include <imgui.h>

#include <Engine/Renderer/Passes/Passes.hpp>

#include <Core/GraphStructures/MultiGraph.hpp>
#include <Core/Math/LinearAlgebra.hpp>


/**
 * @brief define some functions to represent a node widget and
 * useful things like spline rendering, and manage the viewer
 */

namespace ImGui {

    template <typename T>
    class GraphViewer
    {

    /// @brief the structure used to represent the graphic properties of a node
    class NodeProp {
    public:
        /// @brief instantiate the struct required to draw a node on screen
        NodeProp(typename Ra::Core::MultiGraph<T>::Node* node, int level,
                 const char* name, int nbin, int nbout)
            : m_node  ( node    )
            , m_levelx( level   )
            , m_size  ( 128,78  )
            , m_nbout ( nbout   )
            , m_nbin  ( nbin    )
            , m_draggable( true )
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

        /// @brief one node display
        virtual void drawNode();

        /// @brief get input slot position
        /// @warning the coordinates are unaware of window offset
        ImVec2 getInputPos( unsigned int idx );

        /// @brief get output slot position
        /// @warning the coordinates are unaware of window offset
        ImVec2 getOutputPos( unsigned int idx );

        /// @brief get slot position on y-axis
        float getSlotPosY( unsigned int idx, unsigned int total );

    public:
        typename Ra::Core::MultiGraph<T>::Node* m_node;

        // graphic properties
        int    m_id;
        char   m_name[32];
        int    m_levelx, m_levely;
        ImVec2 m_pos   , m_size;
        int    m_nbout , m_nbin;
        bool   m_draggable;
    }; // end of NodeProp



    class NodePropVec2 : public NodeProp {
    public:
        /// @brief instantiate the struct required to draw a node on screen
        NodePropVec2( typename Ra::Core::MultiGraph<T>::Node* node, const char* name )
            : NodeProp( node, 1, name , 0, 1)
        {
        }

        virtual void drawNode() override;

    public:
        Scalar m_value[2];
    }; // NodePropVec2



    class NodePropVec3 : public NodeProp {
    public:
        /// @brief instantiate the struct required to draw a node on screen
        NodePropVec3( typename Ra::Core::MultiGraph<T>::Node* node, const char* name )
            : NodeProp( node, 1, name , 0, 1)
        {
        }

        virtual void drawNode() override;

    public:
        Scalar m_value[3];
    }; // NodePropVec3



    class NodePropVec4 : public NodeProp {
    public:
        /// @brief instantiate the struct required to draw a node on screen
        NodePropVec4( typename Ra::Core::MultiGraph<T>::Node* node, const char* name )
            : NodeProp( node, 1, name , 0, 1)
        {
        }

        virtual void drawNode() override;

    public:
        Scalar m_value[4];
    }; // NodePropVec4



    class NodePropMat2 : public NodeProp {
    public:
        /// @brief instantiate the struct required to draw a node on screen
        NodePropMat2( typename Ra::Core::MultiGraph<T>::Node* node, const char* name )
            : NodeProp( node, 1, name , 0, 1)
        {
        }

        virtual void drawNode() override;

    public:
        Scalar m_value[4];
    }; // NodePropMat2



    class NodePropMat3 : public NodeProp {
    public:
        /// @brief instantiate the struct required to draw a node on screen
        NodePropMat3( typename Ra::Core::MultiGraph<T>::Node* node, const char* name )
            : NodeProp( node, 1, name , 0, 1)
        {
        }

        virtual void drawNode() override;

    public:
        Scalar m_value[9];
    }; // NodePropMat3



    class NodePropMat4 : public NodeProp {
    public:
        /// @brief instantiate the struct required to draw a node on screen
        NodePropMat4( typename Ra::Core::MultiGraph<T>::Node* node, const char* name )
            : NodeProp( node, 1, name , 0, 1)
        {
        }

        virtual void drawNode() override;

    public:
        Scalar m_value[16];
    }; // NodePropMat4



    /// @brief dragging management
    enum dragtype : unsigned int {DRAG_NONE=0, DRAG_NODE, DRAG_SLOT};
    enum dragside : unsigned int {DRAG_OUT=0, DRAG_IN};

    /// @brief contains the whole dragging state for imgui to remember
    static struct DragStruct {
        dragtype  m_type;    // determines wether we are dragging node or link
        NodeProp* m_node;    // the node concerned with current dragging
        int       m_slot;    // the slot associated when DRAG_SLOT
        int       m_side;    // the side of the slot (input/output)
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
        void drawLink( NodeProp& node_a, unsigned int slot_a, NodeProp& node_b, unsigned int slot_b );

        /// @brief create a <br>new</br> link
        void createLink();

    private:
        /// @brief find a node and slot to connect to
        /// @return true if a slot was found
        /// @param node is used to locate which node was hit
        /// @param slot is used to know the slot selected
        /// @param side indicates if DRAG_IN or DRAG_OUT
        /// @warning node could be filled even if return is false, if the node was selected
        bool findMouseSlot( NodeProp** node, unsigned int* slot, unsigned int* side );

        /// @brief update the dragging state
        void updateDragging();

        /// @brief draw a Hermite spline
        /// @note this function was written by
        /// <a href="https://gist.github.com/emoon/b8ff4b4ce4f1b43e79f2">emoon</a>
        void draw_hermite( ImDrawList* draw_list, ImVec2 p1, ImVec2 p2, int STEPS,
                           const ImColor &col, float thickness = 2.f );

    private:
        /// @brief the graph structure
        Ra::Core::MultiGraph<T>* m_gr;

        /// @brief the representation we build from this graph structure
        std::vector<std::unique_ptr<NodeProp>> m_props;

        /// @brief hashmap to retrieve nodeProp from node
        std::map<typename Ra::Core::MultiGraph<T>::Node*, NodeProp*> m_reference;

        static int propsIds;

    }; // end of MultiGraphViewer

} // namespace ImGui

#include "ImGuiNodeGraph.tpp"
#include "ImGuiNodeDrawNodes.tpp"

#endif // IMGUI_NODE_GRAPH_HPP
