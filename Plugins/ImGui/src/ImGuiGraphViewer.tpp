#include <cstring>
#include <vector>


// dragging state definition
template <typename T> typename ImGui::GraphViewer<T>::DragStruct
    ImGui::GraphViewer<T>::dragstate = {DRAG_NONE, nullptr, nullptr, -1, false};



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

        // construct the node in function of the returned type
        createNode(node);

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
    }
}



template <typename T>
void ImGui::GraphViewer<T>::Show( bool* open )
{
    // function used to display a graph viewer
    // 1. create a window
    // 2. update dragging status
    // 3. for each node
    //    1. display the node
    //    2. display every non-dragged links
    // 4. draw the dragged link if applicable
    // 5. draw buttons


    static bool openCreator = false;

    // try to create a window
    SetNextWindowPos(ImVec2(26,26), ImGuiSetCond_FirstUseEver);
    SetNextWindowSize(ImVec2(1000,600), ImGuiSetCond_FirstUseEver);

    if (! ImGui::Begin("Post-process node editor", open, ImVec2(900,500), 0.3f,
                       ImGuiWindowFlags_NoTitleBar|ImGuiWindowFlags_NoMove|ImGuiWindowFlags_HorizontalScrollbar))
    {
        ImGui::End();
        return;
    }

    // channel splitting - used to draw node above links
    ImDrawList* draw_list = ImGui::GetWindowDrawList();
    draw_list->ChannelsSplit(2);


    // update dragging status
    updateDragging();


    // draw every nodes and their links
    for (auto const& nodeRepr : m_props)
    {
        // 1. actually draw the node
        updateNodePos(nodeRepr.get());
        nodeRepr->drawNode();

        // 2. draw the connections to its parents
        for (auto const& parent : nodeRepr->m_node->m_parents)
        {
            // for each parent find the corresponding prop
            typename Ra::Core::MultiGraph<T>::Node::Connection co = parent;
            NodeProp<T>* parentProp = m_reference[co.m_source];

            // and draw the links
            drawLink(*parentProp, co.m_slot, *(nodeRepr.get()), co.m_local);
        }
    }

    // draw the dragged link if currently clicked
    if ((dragstate.m_type == DRAG_SLOT))
    {
        createLink();
    }

    draw_list->ChannelsMerge();

    // drawing some buttons
    PushStyleColor(ImGuiCol_Button, ImVec4(0.f,0.8f,0.45f,0.75f));

    SetCursorPos(GetWindowPos() + ImVec2(GetScrollX(), GetWindowHeight() + GetScrollY() - (6*24)));
    BeginGroup();

    ImVec2 buttonSize(100,16);

    // update view
    Button("Reset view", buttonSize);
    if (IsItemClicked())
    {
        Init();
    }

    // update graph
    Button("Req. levelize", buttonSize);
    if (IsItemClicked())
    {
        m_gr->levelize( true );
    }

    // create node
    Button("Create node", buttonSize);
    if (IsItemClicked())
    {
        openCreator = true;
    }

    // delete node
    Button("Delete node", buttonSize);
    if (IsItemClicked())
    {
        removeNode(dragstate.m_last);
        dragstate.m_last = nullptr;
    }

    // display if an error is detected
    if (m_gr->m_status == Ra::Core::GRAPH_ERROR)
    {
        PushStyleColor(ImGuiCol_Button, ImVec4(1.f,0.3f,0.3f,0.8f));
        Button("Error", buttonSize);
        PopStyleColor();
    }

    EndGroup();

    if (openCreator)
    {
        NodeCreator(&openCreator);
    }

    PopStyleColor();
    ImGui::End();
}



template <typename T>
void ImGui::GraphViewer<T>::NodeCreator( bool* open )
{
    using Ra::Engine::PARAM_INT;
    using Ra::Engine::PARAM_UINT;
    using Ra::Engine::PARAM_VEC2;
    using Ra::Engine::PARAM_VEC3;
    using Ra::Engine::PARAM_VEC4;
    using Ra::Engine::PARAM_MAT2;
    using Ra::Engine::PARAM_MAT3;
    using Ra::Engine::PARAM_MAT4;
    using Ra::Engine::PARAM_SCALAR;

    using Ra::Engine::PassT;

    // every needed informations to spawn a node

    Ra::Engine::Pass* p = nullptr;

    const  ImGuiIO io       = ImGui::GetIO();
    const  unsigned int w = io.DisplaySize.x, h = io.DisplaySize.y;

    const  int     namesize = 32;
    const  char*   pass[]   = {"Regular", "Blur", "PingPong", "Redux", "Generator"};
    const  char*   type[]   = {"Int",     "Uint",    "Scalar",
                               "Vector2", "Vector3", "Vector4",
                               "Matrix2", "Matrix3", "Matrix4"};

    // selection of a type and type of pass
    static int selectedPass, selectedType;

    static int  nbParameters[2] = {0,0};  // in/out
    static char name[namesize], shader[namesize];

    static int   loops = 1;
    static float ratio = 0.5f;

    SetNextWindowPos(GetMousePos(), ImGuiSetCond_FirstUseEver);

    // create a (beautiful) window
    if (! ImGui::Begin("Node creation", open, ImVec2(400,400)))
    {
        ImGui::End();
        return;
    }


    // widgets !!
    Combo(" : pass", &selectedPass, pass, 5);
    InputText(" :  name", name, namesize);

    switch (selectedPass)
    {
    case 0: // regular
        Text("Regular");
        InputText(" : shader", shader, namesize);
        InputInt2(" : in/out", nbParameters);
        break;
    case 1: // blur
        Text("Blur");
        SliderInt(" : amount", &loops, 1, 1024);
        break;
    case 2: // ping-pong
        Text("Ping-Pong");
        SliderInt(" : loops", &loops, 1, 1024);
        InputText(" : shader", shader, namesize);
        InputInt2(" : in/out", nbParameters);
        break;
    case 3: // redux
        Text("Redux");
        SliderFloat(" : size factor", &ratio, 0.f, 1.f);
        InputText(" : shader", shader, namesize);
        InputInt2(" : in/out", nbParameters);
        break;
    default:
        Combo(" : type", &selectedType, type, 9);
        break;
    }

    // creation part
    Button("Create");
    if (IsItemClicked())
    {
        if (selectedPass == 0) // regular
        {
            p = new Ra::Engine::PassRegular(name, w, h, nbParameters[0], nbParameters[1], shader);
        }
        else if (selectedPass == 1) // blur
        {
            p = new Ra::Engine::PassBlur(name, w, h, loops);
        }
        else if (selectedPass == 2) // ping-pong
        {
            p = new Ra::Engine::PassPingPong(name, w, h, nbParameters[0], nbParameters[1], loops, shader);
        }
        else if (selectedPass == 3) // redux
        {
            p = new Ra::Engine::PassRedux(name, w, h, nbParameters[0], nbParameters[1], shader, ratio);
        }
        else // generator
        {
            switch (selectedType)
            {
            case PARAM_INT:
                p = new PassT<int>(name, 0, PARAM_INT);
                break;
            case PARAM_UINT:
                p = new PassT<unsigned int>(name, 0, PARAM_UINT);
                break;
            case PARAM_VEC2:
                p = new PassT<Ra::Core::Vector2>(name, Ra::Core::Vector2(), PARAM_VEC2);
                break;
            case PARAM_VEC3:
                p = new PassT<Ra::Core::Vector3>(name, Ra::Core::Vector3(), PARAM_VEC3);
                break;
            case PARAM_VEC4:
                p = new PassT<Ra::Core::Vector4>(name, Ra::Core::Vector4(), PARAM_VEC4);
                break;
            case PARAM_MAT2:
                p = new PassT<Ra::Core::Matrix2>(name, Ra::Core::Matrix2(), PARAM_MAT2);
                break;
            case PARAM_MAT3:
                p = new PassT<Ra::Core::Matrix3>(name, Ra::Core::Matrix3(), PARAM_MAT3);
                break;
            case PARAM_MAT4:
                p = new PassT<Ra::Core::Matrix4>(name, Ra::Core::Matrix4(), PARAM_MAT4);
                break;
            case PARAM_SCALAR:
                p = new PassT<Scalar>(name, 0, PARAM_SCALAR);
                break;
            }
        }

        createNode( m_gr->addNode( p ) );
    }

    ImGui::End();
}



template <typename T>
void ImGui::GraphViewer<T>::drawLink(NodeProp<T>& node_a, unsigned int slot_a,
                                     NodeProp<T>& node_b, unsigned int slot_b)
{
    ImVec2 offset = GetWindowPos() - ImVec2(GetScrollX(), GetScrollY());
    ImVec2 p_a, p_b;

    // draw below the nodes boxes
    ImDrawList* draw_list = GetWindowDrawList();
    draw_list->ChannelsSetCurrent(0);

    bool dragged_b;

    // don't draw the link if it's dragged
    dragged_b =  (dragstate.m_type ==  DRAG_SLOT)
              && (dragstate.m_node == &node_a)
              && (dragstate.m_slot ==  slot_b)
              && (dragstate.m_side ==  DRAG_IN);

    if (dragged_b)
    {
        return;
    }

    p_a = node_a.getOutputPos( slot_a ) + offset;
    p_b = node_b.getInputPos(  slot_b ) + offset;

    draw_hermite(draw_list, p_a, p_b, 12, ImColor(180,180,180), 1.f);
}



template <typename T>
void ImGui::GraphViewer<T>::createLink()
{
    ImVec2 offset = GetWindowPos() - ImVec2(GetScrollX(), GetScrollY());
    ImDrawList* draw_list = ImGui::GetWindowDrawList();

    // draw on links level
    draw_list->ChannelsSetCurrent(0);

    // draw a simply link between the source node and the mouse
    draw_hermite(draw_list,
                 dragstate.m_node->getOutputPos(dragstate.m_slot) + offset,
                 GetMousePos(),
                 12, ImColor(200,200,200), 1.f);
}



template <typename T>
void ImGui::GraphViewer<T>::createNode( typename Ra::Core::MultiGraph<T>::Node* node )
{
    // construct the property in function of the returned type
    switch (node->m_data->generates())
    {
    case Ra::Engine::PARAM_VEC2:
        m_props.push_back(std::unique_ptr<NodePropVec2<T>>( new NodePropVec2<T>( node, node->m_name )));
        break;
    case Ra::Engine::PARAM_VEC3:
        m_props.push_back(std::unique_ptr<NodePropVec3<T>>( new NodePropVec3<T>( node, node->m_name )));
        break;
    case Ra::Engine::PARAM_VEC4:
        m_props.push_back(std::unique_ptr<NodePropVec4<T>>( new NodePropVec4<T>( node, node->m_name )));
        break;
    case Ra::Engine::PARAM_MAT2:
        m_props.push_back(std::unique_ptr<NodePropMat2<T>>( new NodePropMat2<T>( node, node->m_name )));
        break;
    case Ra::Engine::PARAM_MAT3:
        m_props.push_back(std::unique_ptr<NodePropMat3<T>>( new NodePropMat3<T>( node, node->m_name )));
        break;
    case Ra::Engine::PARAM_MAT4:
        m_props.push_back(std::unique_ptr<NodePropMat4<T>>( new NodePropMat4<T>( node, node->m_name )));
        break;
    case Ra::Engine::PARAM_SCALAR:
        m_props.push_back(std::unique_ptr<NodePropScalar<T>>( new NodePropScalar<T>( node, node->m_name )));
        break;
    case Ra::Engine::PARAM_INT:
        m_props.push_back(std::unique_ptr<NodePropInt<T>>( new NodePropInt<T>( node, node->m_name )));
        break;
    case Ra::Engine::PARAM_UINT:
        m_props.push_back(std::unique_ptr<NodePropUint<T>>( new NodePropUint<T>( node, node->m_name )));
        break;
    default:
        m_props.push_back(std::unique_ptr<NodeProp<T>>( new NodeProp<T>( node, node->m_level, node->m_name, node->m_nbIn, node->m_nbOut) ));
        break;
    }

    // add to hashmap
    m_reference[node] = m_props.back().get();
}



/// @brief remove a node
template <typename T>
void ImGui::GraphViewer<T>::removeNode( NodeProp<T>* prop )
{
    if (prop != nullptr)
    {
        m_gr->deleteNode(prop->m_node);
        Init();
    }
}



template <typename T>
bool ImGui::GraphViewer<T>::findMouseSlot( NodeProp<T>** node, unsigned int* slot, unsigned int* side )
{
    // find the node or slot under mouse position.
    // will return true only if a slot was found, but node will be filled if a node OR a slot is found.

    // 1. loop over each nodes
    //    1. look if in bounding box
    //    2. look if on node or link
    //    3. if node
    //          return
    // 2. if node's bounding box
    //    1. look at every input
    //    2. look at every output
    // done !

    bool   found = false;
    ImVec2 offset = GetWindowPos() - ImVec2(GetScrollX(), GetScrollY());
    ImVec2 mousep = GetMousePos();

    // node iterator
    auto candidate = m_props.begin();

    ImDrawList*  draw_list = GetWindowDrawList();
    NodeProp<T>* prop      = nullptr;

    // iterate on nodes until a node or slot is found
    while ((! found) && (candidate != m_props.end()))
    {
        prop = candidate->get();

        // first detect if we are in the globing rectangle the node
        if (IsMouseHoveringRect(prop->m_pos - ImVec2(12.f,0.f) + offset,
                                prop->m_pos + ImVec2(12.f,0.f) + offset + prop->m_size))
        {
            // if we are on the node (so not on a slot)
            if (IsMouseHoveringRect(prop->m_pos + offset, prop->m_pos + offset + prop->m_size))
            {
                *node = candidate->get();
                return false;
            }
            found = true;
        }

        ++ candidate;
    }

    // if a node was found seek for the good slot
    if (found)
    {
        ImVec2 slotpos, top, bottom;

        // if mouse on the left
        if (mousep.x <= (prop->m_pos.x + offset.x))
        {
            for (int i = 0; i < prop->m_nbin; ++ i)
            {
                slotpos = prop->getInputPos(i) + offset;
                top     = slotpos - ImVec2(4.5, 4.5);
                bottom  = slotpos + ImVec2(4.5, 4.5);
                if (IsMouseHoveringRect(top, bottom))
                {
                    draw_list->AddCircle(slotpos, 4.5f, ImColor(0,255,0));
                    *node = prop;
                    *slot = i;
                    *side = DRAG_IN;
                    return true;
                }
            }
        }
        else // if mouse on the right
        {
            for (int i = 0; i < prop->m_nbout; ++ i)
            {
                slotpos = prop->getOutputPos(i) + offset;
                top     = slotpos - ImVec2(4.5, 4.5);
                bottom  = slotpos + ImVec2(4.5, 4.5);
                if (IsMouseHoveringRect(top, bottom))
                {
                    draw_list->AddCircle(slotpos, 4.5f, ImColor(0,255,0));
                    *node = prop;
                    *slot = i;
                    *side = DRAG_OUT;
                    return true;
                }
            }
        }
    }

    return false;
}



template <typename T>
void ImGui::GraphViewer<T>::updateDragging()
{
    // update the dragging state
    // manage mouse release and mouse click

    NodeProp<T>* prop = nullptr;
    unsigned int i    = 0;
    unsigned int side = DRAG_IN;

    // release management, applicable if dragging a link

    // if a newly created link is released, try to add it to a node slot if found
    if (! IsMouseDown(0))
    {
        if ((dragstate.m_type == DRAG_SLOT))
        {
            if ( findMouseSlot(&prop, &i, &side) )
            {
                // only if new node is an input and if we were on output
                if ((side == DRAG_IN) && (dragstate.m_side == DRAG_OUT))
                {
                    prop->m_node->setParent(dragstate.m_slot, dragstate.m_node->m_node, i);
                }
            }
        }

        // else just discard
        // if the links existed before, it will be removed though
        dragstate.m_type = DRAG_NONE;
        dragstate.m_side = DRAG_IN;
        dragstate.m_node = nullptr;
        dragstate.m_slot = -1;
        return;
    }

    // click management
    // 1. check if a node or slot is currently hovered by mouse
    // 2. if a slot was found we have two cases:
    //    1. the slot is an output slot
    //          basically create a new link
    //    2. the slot is an input slot
    //       1. this slot has a parent
    //             set the parent's slot to dragged state
    //             remove parent from initial slot
    //       2. no parent
    //             discard action
    // 3. drag the node if it was a node

    // else if the mouse is clicked and nothing was dragged
    if (dragstate.m_type == DRAG_NONE)
    {
        if ( findMouseSlot(&prop, &i, &side) ) // a slot was found
        {
            dragstate.m_type = DRAG_SLOT;
            dragstate.m_side = side;
            dragstate.m_slot = i;

            if (side == DRAG_OUT)
            {
                dragstate.m_node = prop;
            }
            else
            {
                // find this parent
                for (auto const& parent : prop->m_node->m_parents)
                {
                    // find the good parent (eg. good slot)
                    if (parent.m_local == i)
                    {
                        // reference the parent as dragged node
                        dragstate.m_side = DRAG_OUT;
                        dragstate.m_node = m_reference[parent.m_source];
                        dragstate.m_slot =             parent.m_slot;

                        // remove parent
                        prop->m_node->removeParent(dragstate.m_slot, dragstate.m_node->m_node, i);

                        break;
                    }
                }

                // if no child was found
                if (dragstate.m_node == nullptr)
                {
                    dragstate.m_type = DRAG_NONE;
                }

            }
        }
        else if (prop != nullptr) // a node was found, at least
        {
            dragstate.m_type = DRAG_NODE;
            dragstate.m_node = prop;
            dragstate.m_last = prop;
        }

        // if dragging disabled by other ImGui Widgets
        if (IsAnyItemHovered() || IsAnyItemActive())
        {
            dragstate.m_type = DRAG_NONE;
        }

    }
}



template <typename T>
inline void ImGui::GraphViewer<T>::updateNodePos(NodeProp<T>* prop)
{
    // move node
    if ((dragstate.m_type == DRAG_NODE) && (dragstate.m_node == prop))
    {
        prop->m_pos = prop->m_pos + ImGui::GetMouseDragDelta();
        ImGui::ResetMouseDragDelta();
    }
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
