#include <cstring>
#include <vector>

static inline ImVec2 operator+(const ImVec2& lhs, const ImVec2& rhs) { return ImVec2(lhs.x+rhs.x, lhs.y+rhs.y); }
static inline ImVec2 operator-(const ImVec2& lhs, const ImVec2& rhs) { return ImVec2(lhs.x-rhs.x, lhs.y-rhs.y); }

// dragging state definition
template <typename T> typename ImGui::GraphViewer<T>::DragStruct
    ImGui::GraphViewer<T>::draggingState = {DRAG_NONE, nullptr, -1, false};

template <typename T> int ImGui::GraphViewer<T>::propsIds = 0;



template <typename T>
void ImGui::GraphViewer<T>::Show(bool* opened)
{
    // try to create a window
    SetNextWindowPos(ImVec2(26,26), ImGuiSetCond_FirstUseEver);
    SetNextWindowSize(ImVec2(1000,600), ImGuiSetCond_FirstUseEver);

    if (! ImGui::Begin("This window is beautiful", opened, ImVec2(900,500), 0.3f,
                       ImGuiWindowFlags_NoTitleBar|ImGuiWindowFlags_NoMove|ImGuiWindowFlags_HorizontalScrollbar))
    {
        ImGui::End();
        return;
    }

//    // dragging status print
//    Text("type : %d", draggingState.m_type);
//    Text("node : %p", draggingState.m_node);
//    Text("slot : %d", draggingState.m_slot);
//    Text("side : %s", (draggingState.m_side == DRAG_OUT)?"OUT":"IN");

    // channel splitting
    ImDrawList* draw_list = ImGui::GetWindowDrawList();
    draw_list->ChannelsSplit(2);

    // draw every nodes and their links
    for (auto const& nodeRepr : m_props)
    {
        // 1. actually draw the node
        drawNode(*(nodeRepr.get()));

        // 2. draw the connections to its parents
        for (auto const& parent : nodeRepr->m_node->m_parents)
        {
            // for each parent find the corresponding prop
            typename Ra::Core::MultiGraph<T>::Node::Connection co = parent;
            NodeProp* parentProp = m_reference[co.m_source];

            // and draw
            drawLink(*parentProp, co.m_slot, *(nodeRepr.get()), co.m_local);
        }
    }

    draw_list->ChannelsMerge();

    PushStyleColor(ImGuiCol_Button, ImVec4(0.f,0.9f,0.45f,0.7f));

    // drawing some buttons
    SetCursorPos(GetWindowPos() + ImVec2(-13 + GetScrollX(), GetWindowHeight() - 100 + GetScrollY()));
    Button("Reset view");

    // used to update the graph
    if (IsItemClicked())
    {
        Init();
    }

    SetCursorPos(GetWindowPos() + ImVec2(GetScrollX() + 80, GetWindowHeight() - 100 + GetScrollY()));
    Button("Req. update");

    if (IsItemClicked())
    {
        m_gr->m_status = Ra::Core::GRAPH_UPDATE;
    }

    if (m_gr->m_status == Ra::Core::GRAPH_ERROR)
    {
        PushStyleColor(ImGuiCol_Button, ImVec4(1.f,0.3f,0.3f,0.8f));
        SetCursorPos(GetWindowPos() + ImVec2(GetScrollX() + 180, GetWindowHeight() - 100 + GetScrollY()));
        Button("Error");
        PopStyleColor();
    }

    PopStyleColor();
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
        typename Ra::Core::MultiGraph<T>::Node* node = (*m_gr)[i];

        // construct the node
        m_props.push_back(std::unique_ptr<NodeProp>(new NodeProp(node, node->m_level, node->m_name.c_str(),
                                                                 node->m_nbIn, node->m_nbOut)));

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
        m_reference[node] = m_props.back().get();
    }
}


// note: events management is inspired from imgui's author's code
template <typename T>
void ImGui::GraphViewer<T>::drawNode(NodeProp& info)
{
    PushID(info.m_id);

    const float nodeRadius = 3.5f;
    ImColor bgBox(40,40,40), bgEdge(180,180,180), bgSubBox(81, 81, 151);
    ImVec2 offset = GetWindowPos() - ImVec2(GetScrollX(), GetScrollY());

    // draw the nodes on the foreground (on the top of links)
    ImDrawList* draw_list = ImGui::GetWindowDrawList();
    draw_list->ChannelsSetCurrent(1);

    ImVec2 bottom = offset + info.m_pos, top = offset + info.m_pos + info.m_size;

    // event management
    bool hovered = false, dragged = false;

    // check if node is hovered by mouse and eventually if it's already dragged
    hovered = IsMouseHoveringRect(bottom, top);
    dragged = (draggingState.m_type == DRAG_NODE && draggingState.m_node == &info);

    // if hovered:
    // . change color to a lighter value
    // . test if mouse is pressed and if no other widget was candidate
    // . if last is true just track this node
    if (hovered)
    {
        bgBox = ImColor(60,60,60); // ligthen background

        // update dragging state if nothing is dragged and node is clicked
        if ((! dragged) && (draggingState.m_type == DRAG_NONE) && (IsMouseDown(0)))
        {
            dragged = true;
            draggingState.m_type = DRAG_NODE;
            draggingState.m_node = &info;
        }
    }

    // disable dragging if released, and set position accordingly
    if (dragged) {

        if (! IsMouseDown(0))
        {
            draggingState.m_type = DRAG_NONE;
        }

        info.m_pos = info.m_pos + ImGui::GetMouseDragDelta();
        ImGui::ResetMouseDragDelta();
    }

    // actual background draw
    draw_list->AddRectFilled(bottom, top, bgBox, 1.4f);
    draw_list->AddRectFilled(bottom + ImVec2(24, 2), top - ImVec2(2, 2), bgSubBox, 1.4f);

    // input connectors
    for (int i = 0; i < info.m_nbin; ++i)
    {
        draw_list->AddCircleFilled(getInputPos(info, i) + offset, nodeRadius, bgEdge);
    }

    // output connectors
    for (int i = 0; i < info.m_nbout; ++i)
    {
        draw_list->AddCircleFilled(getOutputPos(info, i) + offset, nodeRadius, bgEdge);
    }

    // the node labels
    SetCursorPos(info.m_pos + ImVec2(5.f,5.f));
    Text("%d", info.m_levelx);
    SetCursorPos(info.m_pos + ImVec2(30.f,5.f));
    Text("%s", info.m_name);

    PopID();
}


template <typename T>
void ImGui::GraphViewer<T>::drawLink(NodeProp& node_a, unsigned int slot_a,
                                     NodeProp& node_b, unsigned int slot_b)
{
    ImVec2 offset = GetWindowPos() - ImVec2(GetScrollX(), GetScrollY());
    ImVec2 p_a, p_b;

    // draw below the nodes boxes
    ImDrawList* draw_list = GetWindowDrawList();
    draw_list->ChannelsSetCurrent(0);

    // event management
    // all the tests are done in b-then-a order because the user is most likely to drag the
    // right side of a link, thus the first test has great chances of being the good one

    bool hovered_a = false, dragged_a = false;
    bool hovered_b = false, dragged_b = false;

    dragged_b =  (draggingState.m_type ==  DRAG_SLOT)
              && (draggingState.m_node == &node_b)
              && (draggingState.m_slot ==  slot_b)
              && (draggingState.m_side ==  DRAG_IN);

    dragged_a =  (! dragged_b)
              && (draggingState.m_type ==  DRAG_SLOT)
              && (draggingState.m_node == &node_a)
              && (draggingState.m_slot ==  slot_a)
              && (draggingState.m_side ==  DRAG_OUT);

    if (dragged_b)
    {
        // move the right side of the curve
        p_a = getOutputPos(node_a, slot_a) + offset;
        p_b = GetMousePos();
    }
    else if (dragged_a)
    {
        // move the left side of the curve
        p_a = getOutputPos(node_a, slot_a) + offset;
        p_b = getInputPos(node_b, slot_b) + offset;

        draw_hermite(draw_list, p_a, GetMousePos(), 12, ImColor(180,180,180), 1.f);
    }
    else
    {
        // base case
        p_a = getOutputPos(node_a, slot_a) + offset;
        p_b = getInputPos(node_b, slot_b) + offset;
    }

    hovered_b =                  IsMouseHoveringRect(p_b - ImVec2(3.5f, 3.5f), p_b + ImVec2(3.5f, 3.5f));
    hovered_a = (! hovered_b) && IsMouseHoveringRect(p_a - ImVec2(3.5f, 3.5f), p_a + ImVec2(3.5f, 3.5f));

    // check if a or b could be dragged, if available
    if (IsMouseDown(0) && (draggingState.m_type == DRAG_NONE))
    {
        if (hovered_b)
        {
            draggingState.m_type =  DRAG_SLOT;
            draggingState.m_node = &node_b;
            draggingState.m_slot =  slot_b;
            draggingState.m_side =  DRAG_IN;
            dragged_b = true;
        }
        else if (hovered_a)
        {
            draggingState.m_type =  DRAG_SLOT;
            draggingState.m_node = &node_a;
            draggingState.m_slot =  slot_a;
            draggingState.m_side =  DRAG_OUT;
            dragged_a = true;
        }
    }

    // release
    if ((dragged_a || dragged_b) && (! IsMouseDown(0)))
    {
        draggingState.m_type = DRAG_NONE;

        // react accordingly to the point the link was dropped to
        if (dragged_b)
        {
            ImVec2 slot_pos = getInputPos(node_b, slot_b);
            if (! IsMouseHoveringRect(slot_pos - ImVec2(3.5, 3.5) + offset, slot_pos + ImVec2(3.5, 3.5) + offset))
            {
                node_b.m_node->removeParent(slot_a, node_a.m_node, slot_b);
            }
        }
        else if (dragged_a)
        {
            NodeProp*    node_c = nullptr;
            unsigned int slot_c = 0;
            findMouseSlot(&node_c, &slot_c);
            if (node_c != nullptr)
            {
                node_c->m_node->setParent(slot_a, node_a.m_node, slot_c);
            }
        }
    }

    draw_hermite(draw_list, p_a, p_b, 12, ImColor(180,180,180), 1.f);
}


template <typename T>
void ImGui::GraphViewer<T>::createNode( const NodeProp& props )
{}


template <typename T>
void ImGui::GraphViewer<T>::createLink( const NodeProp& prop_a, unsigned int slot_a,
                                        const NodeProp& node_b, unsigned int slot_b )
{}

template <typename T>
bool ImGui::GraphViewer<T>::findMouseSlot( NodeProp** node, unsigned int* slot )
{
    bool   found = false;
    ImVec2 offset = GetWindowPos() - ImVec2(GetScrollX(), GetScrollY());
    ImVec2 mousep = GetMousePos();

    auto candidate = m_props.begin();

    ImDrawList* draw_list = GetWindowDrawList();

    // for each node
    while ((! found) && (candidate != m_props.end()))
    {
        // first detect if we are in the globing rectangle the node
        if (IsMouseHoveringRect((*candidate)->m_pos - ImVec2(12.f,0.f) + offset,
                                (*candidate)->m_pos + ImVec2(12.f,0.f) + offset + (*candidate)->m_size))
        {
            found = true;
        }
        else
        {
            ++ candidate;
        }
    }

    // if a node was found seek for the good slot
    if (found)
    {
        ImVec2 slotpos, top, bottom;
        NodeProp& p = *((*candidate).get());

        if (mousep.x <= (p.m_pos.x + offset.x))
        {
            for (int i = 0; i < p.m_nbin; ++ i)
            {
                slotpos = getInputPos(p, i) + offset;
                top     = slotpos - ImVec2(3.5, 3.5);
                bottom  = slotpos + ImVec2(3.5, 3.5);
                if (IsMouseHoveringRect(top, bottom))
                {
                    draw_list->AddCircle(slotpos, 4.5f, ImColor(0,255,0));
                    *node = &p;
                    *slot =  i;
                }
            }
        }
        else
        {
            for (int i = 0; i < p.m_nbout; ++ i)
            {
                slotpos = getOutputPos(p, i) + offset;
                top     = slotpos - ImVec2(3.5, 3.5);
                bottom  = slotpos + ImVec2(3.5, 3.5);
                if (IsMouseHoveringRect(top, bottom))
                {
                    draw_list->AddCircle(slotpos, 4.5f, ImColor(0,255,0));
                    *node = &p;
                    *slot =  i;
                }
            }
        }
    }

    return found;
}

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
