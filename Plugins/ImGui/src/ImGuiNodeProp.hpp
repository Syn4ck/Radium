#include <Engine/Renderer/Passes/Passes.hpp>

#include <Core/GraphStructures/MultiGraph.hpp>
#include <Core/Math/LinearAlgebra.hpp>

#include <imgui.h>

namespace ImGui {

    /// @brief the structure used to represent the graphic properties of a node
    template <typename T>
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
            m_id = id_prop ++;
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

    protected:
        static unsigned int id_prop;
    }; // end of NodeProp



    template <typename T>
    class NodePropVec2 : public NodeProp<T> {
    public:
        /// @brief instantiate the struct required to draw a node on screen
        NodePropVec2( typename Ra::Core::MultiGraph<T>::Node* node, const char* name )
            : NodeProp<T>( node, 1, name , 0, 1)
        {
        }

        virtual void drawNode() override;

    public:
        Scalar m_value[2];
    }; // NodePropVec2



    template <typename T>
    class NodePropVec3 : public NodeProp<T> {
    public:
        /// @brief instantiate the struct required to draw a node on screen
        NodePropVec3( typename Ra::Core::MultiGraph<T>::Node* node, const char* name )
            : NodeProp<T>( node, 1, name , 0, 1)
        {
        }

        virtual void drawNode() override;

    public:
        Scalar m_value[3];
    }; // NodePropVec3



    template <typename T>
    class NodePropVec4 : public NodeProp<T> {
    public:
        /// @brief instantiate the struct required to draw a node on screen
        NodePropVec4( typename Ra::Core::MultiGraph<T>::Node* node, const char* name )
            : NodeProp<T>( node, 1, name , 0, 1)
        {
        }

        virtual void drawNode() override;

    public:
        Scalar m_value[4];
    }; // NodePropVec4



    template <typename T>
    class NodePropMat2 : public NodeProp<T> {
    public:
        /// @brief instantiate the struct required to draw a node on screen
        NodePropMat2( typename Ra::Core::MultiGraph<T>::Node* node, const char* name )
            : NodeProp<T>( node, 1, name , 0, 1)
        {
        }

        virtual void drawNode() override;

    public:
        Scalar m_value[4];
    }; // NodePropMat2



    template <typename T>
    class NodePropMat3 : public NodeProp<T> {
    public:
        /// @brief instantiate the struct required to draw a node on screen
        NodePropMat3( typename Ra::Core::MultiGraph<T>::Node* node, const char* name )
            : NodeProp<T>( node, 1, name , 0, 1)
        {
        }

        virtual void drawNode() override;

    public:
        Scalar m_value[9];
    }; // NodePropMat3



    template <typename T>
    class NodePropMat4 : public NodeProp<T> {
    public:
        /// @brief instantiate the struct required to draw a node on screen
        NodePropMat4( typename Ra::Core::MultiGraph<T>::Node* node, const char* name )
            : NodeProp<T>( node, 1, name , 0, 1)
        {
        }

        virtual void drawNode() override;

    public:
        Scalar m_value[16];
    }; // NodePropMat4

}


#include "ImGuiNodeProp.tpp"
