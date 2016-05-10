#include "imgui_node_graph.hpp"

#include <cstring>

namespace ImGui {

// TODO(hugo) remove these definitions - from here at least
static inline ImVec2 operator+(const ImVec2& lhs, const ImVec2& rhs) { return ImVec2(lhs.x+rhs.x, lhs.y+rhs.y); }
static inline ImVec2 operator-(const ImVec2& lhs, const ImVec2& rhs) { return ImVec2(lhs.x-rhs.x, lhs.y-rhs.y); }

/// the structure used to represent the graphic properties of a node
struct NodeInfo {
    int    m_id;
    char   m_name[32];
    ImVec2 m_pos, m_size;
    int    m_nbout, m_nbin;

    // constructor
    NodeInfo(int id, const char* name, ImVec2 pos, int nbin, int nbout)
        : m_id(id),
          m_pos(pos),     m_size(96, 48),
          m_nbout(nbout), m_nbin(nbin)
    {
        strncpy(m_name, name, 32);
    }
};

void NodeWindow(bool* opened)
{
    // try to create a window
    ImGui::SetNextWindowSize(ImVec2(600,500), ImGuiSetCond_FirstUseEver);
    ImGui::SetNextWindowPos(ImVec2(32, 32));

    if (! ImGui::Begin("This window is beautiful", opened))
    {
        ImGui::End();
        return;
    }

    // event manipulation
    // ...
    // short section, I know

    // create some fake nodes for testing
    NodeInfo a(0, "A", ImVec2(200, 200), 2, 3)/*, b(1, "B", ImVec2(0, 0), 3, 0)*/;

    // display those nodes
    Node(a);
    //Node(b);

    ImGui::End();
}

void Node(const NodeInfo& info)
{
    ImGui::PushID(info.m_id);

    const float nodeRadius = 3.0f;
    const ImColor bg1(40,40,40), edge1(60,60,60), bg2(0, 153, 255), edge2(77, 184, 255);

    ImDrawList* draw_list = ImGui::GetWindowDrawList();

    // background
    draw_list->AddRectFilled(info.m_pos, info.m_pos + info.m_size, bg1,   1.5f);
    draw_list->AddRect      (info.m_pos, info.m_pos + info.m_size, edge1, 1.5f);

    draw_list->AddRectFilled(info.m_pos + ImVec2(20, 2), info.m_pos + info.m_size - ImVec2(2, 2), bg2, 1.5f);

    // input connectors
    for (int i = 0; i < info.m_nbin; ++i)
    {
        draw_list->AddCircleFilled(GetInputPos(info, i), nodeRadius, bg1);
        draw_list->AddCircle      (GetInputPos(info, i), nodeRadius, edge1);
    }

    for (int i = 0; i < info.m_nbout; ++i)
    {
        draw_list->AddCircleFilled(GetOutputPos(info, i), nodeRadius, bg1);
        draw_list->AddCircle      (GetOutputPos(info, i), nodeRadius, edge1);
    }

//    ImGui::SetCursorPos(ImVec2(800.f, 400.f));
//    ImGui::Text("Node %s", info.m_name);

    ImGui::PopID();
}

ImVec2 GetInputPos(  const NodeInfo& info, unsigned int idx )
{
    float posY = GetSlotPosY(info, idx, info.m_nbin);
    return ImVec2(info.m_pos.x - 8.0f, posY);
}

ImVec2 GetOutputPos( const NodeInfo& info, unsigned int idx )
{
    float posY = GetSlotPosY(info, idx, info.m_nbout);
    return ImVec2(info.m_pos.x + info.m_size.x + 8.0f, posY);
}

float GetSlotPosY( const NodeInfo& info, unsigned int idx, unsigned int total )
{
    // and the distance that will separe two connectors
    float d = ((info.m_pos.y + info.m_size.y) - info.m_pos.y) / ((float) total);

    // process the real Y position of the connector
    return info.m_pos.y + (0.5 * d) + (idx * d);
}

}
