#include <Engine/Renderer/Passes/Passes.hpp>

#include <Core/GraphStructures/MultiGraph.hpp>
#include <Core/Math/LinearAlgebra.hpp>

#include <imgui.h>

#include <float.h>
#include <limits.h>

namespace ImGui {

    /**
     *
     * \class NodeProp<T>
     *
     * @brief The structure used to represent the graphic properties of a node.
     *
     */

    template <typename T>
    class NodeProp {
    public:
        /// Instantiate the struct required to draw a node on screen.
        NodeProp(typename Ra::Core::MultiGraph<T>::Node* node, int level,
                 const std::string& name, int nbin, int nbout)
            : m_node  ( node    )
            , m_name  ( name    )
            , m_levelx( level   )
            , m_size  ( 128,78  )
            , m_nbout ( nbout   )
            , m_nbin  ( nbin    )
            , m_draggable( true )
        {
            m_id = id_prop ++;
        }

        /// Estimate the best position of a node.
        /// @note Dumb method: place every node on a pseudo-grid.
        void setAutoPos( unsigned int levelx, unsigned int levely )
        {
            m_pos.x = (levelx+1)*60.f + ((levelx) * m_size.x);
            m_pos.y = (levely+1)*60.f + ((levely) * m_size.y);
        }

        /// Draw the node's content, basically nothing.
        /// Should be overriden by a node with specific content.
        /// @note Background is always drawn by another function:
        /// @see NodeProp::drawBackground()
        virtual void drawNode();

        /// Get input slot position.
        /// @warning The coordinates are unaware of window offset.
        ImVec2 getInputPos( unsigned int idx );

        /// Get output slot position.
        /// @warning The coordinates are unaware of window offset.
        ImVec2 getOutputPos( unsigned int idx );

        /// Get slot position on y-axis.
        float getSlotPosY( unsigned int idx, unsigned int total );

    protected:
        /// Function to draw the node background
        virtual void drawBackground( const std::string& title,
                                     const ImVec2&  offset = ImVec2(0,0),
                                     const ImColor& bg     = ImColor(40,40,40,180) );

    public:
        typename Ra::Core::MultiGraph<T>::Node* m_node;

        ///{@
        /// graphic properties
        std::string m_name;
        int    m_id;
        int    m_levelx, m_levely;
        ImVec2 m_pos   , m_size;
        int    m_nbout , m_nbin;
        bool   m_draggable;
        ///@}

    protected:
        static unsigned int id_prop;
    }; // end of NodeProp



    /**
     *
     * \class NodeGeneratorProp<T,R>
     *
     * @brief Specialization of the NodeProp to draw nodes that will generate
     * something of type R.
     *
     */

    template <typename T, typename R>
    class NodeGeneratorProp : public NodeProp<T> {

    public:
        NodeGeneratorProp( typename Ra::Core::MultiGraph<T>::Node* node, const std::string& name,
                           const R& rmin, const R& rstep, const R& rmax )
            : NodeProp<T> ( node, 1, name , 0, 1)
            , m_rangeMin  ( rmin  )
            , m_rangeStep ( rstep )
            , m_rangeMax  ( rmax  )
        {
        }

    protected:
        R m_rangeMin;
        R m_rangeStep;
        R m_rangeMax;
    };



    template <typename T>
    class NodePropInt : public NodeGeneratorProp<T,int> {
    public:
        /// Instantiate the struct required to draw a node on screen.
        NodePropInt( typename Ra::Core::MultiGraph<T>::Node* node, const std::string& name,
                     const int rmin = INT_MIN, const int rstep = 1, const int rmax = INT_MAX )
            : NodeGeneratorProp<T,int>( node, name, rmin, rstep, rmax )
        {
        }

        /// Draw the node's widget.
        /// Should be overriden.
        virtual void drawNode() override;

    public:
        int m_value;
    }; // NodePropInt



    template <typename T>
    class NodePropUint : public NodeGeneratorProp<T,int> {
    public:
        /// Instantiate the struct required to draw a node on screen.
        NodePropUint( typename Ra::Core::MultiGraph<T>::Node* node, const std::string& name,
                      const int rmin = 0, const int rstep = 1, const int rmax = INT_MAX )
            : NodeGeneratorProp<T,int>( node, name, rmin, rstep, rmax )
        {
        }

        virtual void drawNode() override;

    public:
        int m_value;
    }; // NodePropInt



    template <typename T>
    class NodePropScalar : public NodeGeneratorProp<T,Scalar> {
    public:
        /// Instantiate the struct required to draw a node on screen.
        NodePropScalar( typename Ra::Core::MultiGraph<T>::Node* node, const std::string& name,
                        const Scalar rmin = FLT_MIN, const Scalar rstep = 0.01, const Scalar rmax = FLT_MAX )
            : NodeGeneratorProp<T,Scalar>( node, name, rmin, rstep, rmax )
        {
        }

        virtual void drawNode() override;

    public:
        Scalar m_value;
    }; // NodePropScalar



    template <typename T>
    class NodePropVec2 : public NodeGeneratorProp<T,Scalar> {
    public:
        /// Instantiate the struct required to draw a node on screen.
        NodePropVec2( typename Ra::Core::MultiGraph<T>::Node* node, const std::string& name,
                      const Scalar rmin = FLT_MIN, const Scalar rstep = 0.01, const Scalar rmax = FLT_MAX )
            : NodeGeneratorProp<T,Scalar>( node, name, rmin, rstep, rmax )
        {
        }

        virtual void drawNode() override;

    public:
        Scalar m_value[2];
    }; // NodePropVec2



    template <typename T>
    class NodePropVec3 : public NodeGeneratorProp<T,Scalar> {
    public:
        /// Instantiate the struct required to draw a node on screen.
        NodePropVec3( typename Ra::Core::MultiGraph<T>::Node* node, const std::string& name,
                      const Scalar rmin = FLT_MIN, const Scalar rstep = 0.01, const Scalar rmax = FLT_MAX )
            : NodeGeneratorProp<T,Scalar>( node, name, rmin, rstep, rmax )
        {
        }

        virtual void drawNode() override;

    public:
        Scalar m_value[3];
    }; // NodePropVec3



    template <typename T>
    class NodePropVec4 : public NodeGeneratorProp<T,Scalar> {
    public:
        /// Instantiate the struct required to draw a node on screen.
        NodePropVec4( typename Ra::Core::MultiGraph<T>::Node* node, const std::string& name,
                      const Scalar rmin = FLT_MIN, const Scalar rstep = 0.01, const Scalar rmax = FLT_MAX )
            : NodeGeneratorProp<T,Scalar>( node, name, rmin, rstep, rmax )
        {
        }

        virtual void drawNode() override;

    public:
        Scalar m_value[4];
    }; // NodePropVec4



    template <typename T>
    class NodePropMat2 : public NodeGeneratorProp<T,Scalar> {
    public:
        /// Instantiate the struct required to draw a node on screen.
        NodePropMat2( typename Ra::Core::MultiGraph<T>::Node* node, const std::string& name,
                      const Scalar rmin = FLT_MIN, const Scalar rstep = 0.01, const Scalar rmax = FLT_MAX )
            : NodeGeneratorProp<T,Scalar>( node, name, rmin, rstep, rmax )
        {
        }

        virtual void drawNode() override;

    public:
        Scalar m_value[4];
    }; // NodePropMat2



    template <typename T>
    class NodePropMat3 : public NodeGeneratorProp<T,Scalar> {
    public:
        /// Instantiate the struct required to draw a node on screen.
        NodePropMat3( typename Ra::Core::MultiGraph<T>::Node* node, const std::string& name,
                      const Scalar rmin = FLT_MIN, const Scalar rstep = 0.01, const Scalar rmax = FLT_MAX )
            : NodeGeneratorProp<T,Scalar>( node, name, rmin, rstep, rmax )
        {
        }

        virtual void drawNode() override;

    public:
        Scalar m_value[9];
    }; // NodePropMat3



    template <typename T>
    class NodePropMat4 : public NodeGeneratorProp<T,Scalar> {
    public:
        /// Instantiate the struct required to draw a node on screen.
        NodePropMat4( typename Ra::Core::MultiGraph<T>::Node* node, const std::string& name,
                      const Scalar rmin = FLT_MIN, const Scalar rstep = 0.01, const Scalar rmax = FLT_MAX )
            : NodeGeneratorProp<T,Scalar>( node, name, rmin, rstep, rmax )
        {
        }

        virtual void drawNode() override;

    public:
        Scalar m_value[16];
    }; // NodePropMat4

}


#include "ImGuiNodeProp.tpp"
