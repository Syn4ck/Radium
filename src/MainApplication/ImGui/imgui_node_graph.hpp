#ifndef IMGUI_NODE_GRAPH_HPP
#define IMGUI_NODE_GRAPH_HPP

#include "imgui.h"

/**
 * @brief define some functions to represent a node widget and
 * useful things like spline rendering (hermite ?)
 */

namespace ImGui {

    struct NodeInfo; // forward declare

    /// window that displays nodes and links
    void NodeWindow( bool* opened );

    /// one node display
    void Node( const NodeInfo& info );

    /// get input/output slot position
    ImVec2 GetInputPos(  const NodeInfo& info, unsigned int idx );
    ImVec2 GetOutputPos( const NodeInfo& info, unsigned int idx );

    /// get slot position on y-axis
    float GetSlotPosY( const NodeInfo& info, unsigned int idx, unsigned int total );

}

#endif // IMGUI_NODE_GRAPH_HPP
