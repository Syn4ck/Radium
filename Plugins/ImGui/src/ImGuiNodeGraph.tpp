#include <cstring>
#include <vector>

static inline ImVec2 operator+(const ImVec2& lhs, const ImVec2& rhs) { return ImVec2(lhs.x+rhs.x, lhs.y+rhs.y); }
static inline ImVec2 operator-(const ImVec2& lhs, const ImVec2& rhs) { return ImVec2(lhs.x-rhs.x, lhs.y-rhs.y); }


template <typename T> int ImGui::GraphViewer<T>::propsIds = 0;

template <typename T>
void ImGui::GraphViewer<T>::Show(bool* opened)
{
    // try to create a window
    SetNextWindowPos(ImVec2(26,26), ImGuiSetCond_FirstUseEver);
    SetNextWindowSize(ImVec2(1000,600), ImGuiSetCond_FirstUseEver);

    if (! ImGui::Begin("This window is beautiful", opened, ImVec2(900,500), 0.3f, ImGuiWindowFlags_NoTitleBar))
    {
        ImGui::End();
        return;
    }

    // event manipulation
    // ...
    // short section, I know

    // channel splitting
//    ImDrawList* draw_list = ImGui::GetWindowDrawList();
    //draw_list->ChannelsSplit(2);

    // here is the zone where we draw all the nodes
    for (auto const& nodeRepr : m_props)
    {
        // draw the node, actually
        drawNode(*(nodeRepr.get()));

        // and draw the connection between nodes
        for (auto const& parent : nodeRepr->m_node->m_parents)
        {
            // for each parent find the corresponding prop
            typename Ra::Core::MultiGraph<T>::Node::Connection co = parent;
            NodeProp* parentProp = m_reference[*(co.m_source)];

            // and draw, finally
            drawLink(*parentProp, co.m_slot, *(nodeRepr.get()), co.m_local);
        }
    }

    //draw_list->ChannelsMerge();
    ImGui::End();
}

template <typename T>
void ImGui::GraphViewer<T>::Init()
{
    // this is where we build the representation of the structure at the creation time
    // for every node of ther graph we create a nodeinfo struct, add it to the vector
    // and filling a hashmap of Node -> NodeProp*
    m_props.clear();

    // this vector is used to count the number of nodes that graphically appear on the same column
    std::vector<unsigned int> levely;

    for (int i = 0; i < m_gr->size(); ++ i)
    {
        const typename Ra::Core::MultiGraph<T>::Node* node = (*m_gr)[i];

        // construct the node
        m_props.push_back(std::unique_ptr<NodeProp>(new NodeProp(node, node->m_level, node->m_name.c_str(), node->m_nbIn, node->m_nbOut)));

        // set position and update levely
        if (levely.size() < node->m_level)
        {
            m_props.back()->setAutoPos(node->m_level - 1, 0);
            levely.push_back(1);
        }
        else
        {
            m_props.back()->setAutoPos(node->m_level - 1, levely[node->m_level - 1] ++);
        }

        // add to hashmap
        m_reference[*node] = m_props.back().get();
    }
}



template <typename T>
void ImGui::GraphViewer<T>::drawNode(const NodeProp& info)
{
//    BeginChild("scroll_node", ImVec2(0,0), true, ImGuiWindowFlags_NoScrollbar);
    PushID(info.m_id);

    ImVec2 offset = GetWindowPos(); // get the current window position

    const float nodeRadius = 3.5f;
    const ImColor bg1(40,40,40), edge1(180,180,180), bg2(81, 81, 151);

    ImDrawList* draw_list = ImGui::GetWindowDrawList();
    //draw_list->ChannelsSetCurrent(1);

    // background
    draw_list->AddRectFilled(offset + info.m_pos, offset + info.m_pos + info.m_size, bg1,   1.4f);
    draw_list->AddRectFilled(offset + info.m_pos + ImVec2(24, 2),
                             offset + info.m_pos + info.m_size - ImVec2(2, 2), bg2, 1.4f);

    // input connectors
    for (int i = 0; i < info.m_nbin; ++i)
    {
        draw_list->AddCircleFilled(getInputPos(info, i) + offset, nodeRadius, edge1);
    }

    for (int i = 0; i < info.m_nbout; ++i)
    {
        draw_list->AddCircleFilled(getOutputPos(info, i) + offset, nodeRadius, edge1);
    }

    // beautiful text
    SetCursorPos(info.m_pos + ImVec2(5.f,5.f));
    Text("%d", info.m_levelx);
    SetCursorPos(info.m_pos + ImVec2(30.f,5.f));
    Text("%s", info.m_name);

    PopID();
//    EndChild()
}

template <typename T>
void ImGui::GraphViewer<T>::drawLink(const NodeProp& node_a, unsigned int slot_a,
                              const NodeProp& node_b, unsigned int slot_b)
{
    ImVec2 offset = GetWindowPos();
    ImVec2 p_a = getOutputPos(node_a, slot_a) + offset;
    ImVec2 p_b = getInputPos(node_b, slot_b) + offset;

    ImDrawList* draw_list = GetWindowDrawList();
    //draw_list->ChannelsSetCurrent(0);

    draw_hermite(draw_list, p_a, p_b, 12, ImColor(180,180,180), 1.f);
//    draw_list->AddLine(p_a, p_b, ImColor(180,180,180), 1.f);
}

template <typename T>
void ImGui::GraphViewer<T>::createNode( const NodeProp& props )
{}

template <typename T>
void ImGui::GraphViewer<T>::createLink( const NodeProp& prop_a, unsigned int slot_a,
                                        const NodeProp& node_b, unsigned int slot_b )
{}



template <typename T>
ImVec2 ImGui::GraphViewer<T>::getInputPos( const NodeProp& info, unsigned int idx )
{
    float posY = getSlotPosY(info, idx, info.m_nbin);
    return ImVec2(info.m_pos.x - 8.0f, posY);
}

template <typename T>
ImVec2 ImGui::GraphViewer<T>::getOutputPos( const NodeProp& info, unsigned int idx )
{
    float posY = getSlotPosY(info, idx, info.m_nbout);
    return ImVec2(info.m_pos.x + info.m_size.x + 8.0f, posY);
}

template <typename T>
float ImGui::GraphViewer<T>::getSlotPosY( const NodeProp& info, unsigned int idx, unsigned int total )
{
    // and the distance that will separe two connectors
    float d = ((info.m_pos.y + info.m_size.y) - info.m_pos.y) / ((float) total);

    // process the real Y position of the connector
    return info.m_pos.y + (0.5 * d) + (idx * d);
}



template <typename T>
void ImGui::GraphViewer<T>::draw_hermite(ImDrawList* draw_list, ImVec2 p1, ImVec2 p2, int STEPS,
                                         const ImColor& col, float thickness)
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
