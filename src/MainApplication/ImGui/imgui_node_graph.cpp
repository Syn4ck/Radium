#include "imgui_node_graph.hpp"

#include <cstring>

namespace ImGui {

// TODO(hugo) remove these definitions - from here at least
static inline ImVec2 operator+(const ImVec2& lhs, const ImVec2& rhs) { return ImVec2(lhs.x+rhs.x, lhs.y+rhs.y); }
static inline ImVec2 operator-(const ImVec2& lhs, const ImVec2& rhs) { return ImVec2(lhs.x-rhs.x, lhs.y-rhs.y); }



/// the structure used to represent the graphic properties of a node
struct NodeInfo {
    int    m_id, m_level, m_ylevel;
    char   m_name[32];
    ImVec2 m_pos, m_size;
    int    m_nbout, m_nbin;

    // constructors
    NodeInfo(int id, int level, int ylevel, const char* name, int nbin, int nbout, ImVec2 pos = ImVec2(-1, -1))
        : m_id(id)
        , m_level(level), m_ylevel(ylevel) // TODO(hugo) find a better way to remember number of nodes with same level
        , m_pos(pos),     m_size(96, 48)
        , m_nbout(nbout), m_nbin(nbin)
    {
        strncpy(m_name, name, 32);

        if (pos.x < 0 && pos.y < 0)
        {
            autoPos();
        }
    }

private:
    void autoPos()
    {
        // estimate the best position
        m_pos.x = (m_level+1)*60.f  + (m_level * 96.f);
        m_pos.y = (m_ylevel+1)*60.f + (m_ylevel * 48.f);
    }
};



void BeginNode(bool* opened)
{
    // try to create a window
    ImGui::SetNextWindowPos(ImVec2(26,26));

    if (! ImGui::Begin("This window is beautiful", opened, ImVec2(600,500), 0.3f, ImGuiWindowFlags_NoTitleBar))
    {
        ImGui::End();
        return;
    }

    // event manipulation
    // ...
    // short section, I know

    // create some fake nodes for testing
    NodeInfo a(0, 0, 0, "A", 0, 3), b(1, 1, 0, "B", 1, 1), c(2, 1, 1, "C", 2, 1), d(3, 2, 0, "D", 2, 0);

    // display those nodes
    Node(a);
    Node(b);
    Node(c);
    Node(d);

    // and the links
    NodeLink(a, 0, b, 0);
    NodeLink(a, 1, c, 0);
    NodeLink(a, 2, c, 1);
    NodeLink(b, 0, d, 0);
    NodeLink(c, 0, d, 1);
}

void EndNode()
{
    ImGui::End();
}



void Node(const NodeInfo& info)
{
    ImGui::PushID(info.m_id);

    ImVec2 offset = ImGui::GetWindowPos(); // get the current window position

    const float nodeRadius = 3.5f;
    const ImColor bg1(40,40,40), edge1(180,180,180), bg2(81, 81, 151);

    ImDrawList* draw_list = ImGui::GetWindowDrawList();

    // background
    draw_list->AddRectFilled(offset + info.m_pos, offset + info.m_pos + info.m_size, bg1,   1.5f);
    draw_list->AddRectFilled(offset + info.m_pos + ImVec2(24, 2), offset + info.m_pos + info.m_size - ImVec2(2, 2), bg2, 1.4f);

    // input connectors
    for (int i = 0; i < info.m_nbin; ++i)
    {
        draw_list->AddCircleFilled(GetInputPos(info, i) + offset, nodeRadius, edge1);
    }

    for (int i = 0; i < info.m_nbout; ++i)
    {
        draw_list->AddCircleFilled(GetOutputPos(info, i) + offset, nodeRadius, edge1);
    }

    // beautiful text
    ImGui::SetCursorPos(info.m_pos + ImVec2(5.f,5.f));
    ImGui::Text("%d", info.m_level);
    ImGui::SetCursorPos(info.m_pos + ImVec2(30.f,5.f));
    ImGui::Text("Node %s", info.m_name);

    ImGui::PopID();
}

// declaration for link use
void draw_hermite(ImDrawList* draw_list, ImVec2 p1, ImVec2 p2, int STEPS, const ImColor &col, float thickness = 2.f);

void NodeLink(const NodeInfo& node_a, unsigned int slot_a, const NodeInfo& node_b, unsigned int slot_b)
{
    ImVec2 offset = ImGui::GetWindowPos();
    ImVec2 p_a = GetOutputPos(node_a, slot_a) + offset, p_b = GetInputPos(node_b, slot_b) + offset;

    ImDrawList* draw_list = ImGui::GetWindowDrawList();
    draw_hermite(draw_list, p_a, p_b, 12, ImColor(180,180,180), 1.f);
    //draw_list->AddLine(p_a, p_b, ImColor(180,180,180), 1.f);
}



ImVec2 GetInputPos( const NodeInfo& info, unsigned int idx )
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



void draw_hermite(ImDrawList* draw_list, ImVec2 p1, ImVec2 p2, int STEPS, const ImColor& col, float thickness)
{
    ImVec2 t1 = ImVec2(+80.0f, 0.0f);
    ImVec2 t2 = ImVec2(+80.0f, 0.0f);

    for (int step = 0; step <= STEPS; step++)
    {
        float t = (float)step / (float)STEPS;
        float h1 = +2*t*t*t - 3*t*t + 1.0f;
        float h2 = -2*t*t*t + 3*t*t;
        float h3 =    t*t*t - 2*t*t + t;
        float h4 =    t*t*t -   t*t;
        draw_list->PathLineTo(ImVec2(h1*p1.x + h2*p2.x + h3*t1.x + h4*t2.x, h1*p1.y + h2*p2.y + h3*t1.y + h4*t2.y));
    }

    draw_list->PathStroke(col, false, thickness);
}

}
