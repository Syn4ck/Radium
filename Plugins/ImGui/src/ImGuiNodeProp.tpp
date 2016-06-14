namespace
{


    inline ImVec2 operator+(const ImVec2& lhs, const ImVec2& rhs) { return ImVec2(lhs.x+rhs.x, lhs.y+rhs.y); }
    inline ImVec2 operator-(const ImVec2& lhs, const ImVec2& rhs) { return ImVec2(lhs.x-rhs.x, lhs.y-rhs.y); }

    Ra::Core::Vector2 createVec2(float array[])
    {
        Ra::Core::Vector2 v;
        v(0) = array[0];
        v(1) = array[1];
        return v;
    }

    Ra::Core::Vector3 createVec3(float array[])
    {
        Ra::Core::Vector3 v;
        v(0) = array[0];
        v(1) = array[1];
        v(2) = array[2];
        return v;
    }

    Ra::Core::Vector4 createVec4(float array[])
    {
        Ra::Core::Vector4 v;
        v(0) = array[0];
        v(1) = array[1];
        v(2) = array[2];
        v(3) = array[3];
        return v;
    }

    Ra::Core::Matrix2 createMat2(float array[])
    {
        Ra::Core::Matrix2 m;
        m(0,0) = array[0];   m(1,0) = array[1];
        m(0,1) = array[2];   m(1,1) = array[3];
        return m;
    }

    Ra::Core::Matrix3 createMat3(float array[])
    {
        Ra::Core::Matrix3 m;
        m(0,0) = array[0];    m(1,0) = array[1];    m(2,0) = array[2];
        m(0,1) = array[3];    m(1,1) = array[4];    m(2,1) = array[5];
        m(0,2) = array[6];    m(1,2) = array[7];    m(2,2) = array[8];
        return m;
    }

    Ra::Core::Matrix4 createMat4(float array[])
    {
        Ra::Core::Matrix4 m;
        m(0,0) = array[0];  m(1,0) = array[1];  m(2,0) = array[2];  m(3,0) = array[3];
        m(0,1) = array[4];  m(1,1) = array[5];  m(2,1) = array[6];  m(3,1) = array[7];
        m(0,2) = array[8];  m(1,2) = array[9];  m(2,2) = array[10]; m(3,2) = array[11];
        m(0,3) = array[12]; m(1,3) = array[13]; m(2,3) = array[14]; m(3,3) = array[15];
        return m;
    }
}


template <typename T> unsigned int ImGui::NodeProp<T>::id_prop = 0;


template <typename T>
ImVec2 ImGui::NodeProp<T>::getInputPos( unsigned int idx )
{
    float posY = getSlotPosY(idx, m_nbin);
    return ImVec2(m_pos.x - 8.0f, posY);
}


template <typename T>
ImVec2 ImGui::NodeProp<T>::getOutputPos( unsigned int idx )
{
    float posY = getSlotPosY(idx, m_nbout);
    return ImVec2(m_pos.x + m_size.x + 8.0f, posY);
}



template <typename T>
float ImGui::NodeProp<T>::getSlotPosY( unsigned int idx, unsigned int total )
{
    // and the distance that will separe two connectors
    float d = ((m_pos.y + m_size.y) - (m_pos.y + 16.f)) / ((float) total);

    // process the real Y position of the connector
    return m_pos.y + (0.5 * d) + (idx * d) + 16.f;
}

template <typename T>
void ImGui::NodeProp<T>::drawNode()
{
    PushID(m_id);

    const float nodeRadius = 3.5f;
    ImColor bgBox(40,40,40), bgEdge(180,180,180);
    ImVec2 offset = GetWindowPos() - ImVec2(GetScrollX(), GetScrollY());

    // draw the nodes on the foreground (on the top of links)
    ImDrawList* draw_list = ImGui::GetWindowDrawList();
    draw_list->ChannelsSetCurrent(1);

    ImVec2 bottom = offset + m_pos;
    ImVec2 top    = offset + m_pos + m_size;

    // if hovered:
    if (IsMouseHoveringRect(bottom, top))
    {
        bgBox = ImColor(60,60,60); // ligthen background
    }

    // actual background draw
    draw_list->AddRectFilled(bottom, top, (m_draggable) ? bgBox : ImColor(255,0,0), 2.4f);

    SetCursorPos(m_pos + ImVec2(5.f,5.f));
    BeginGroup();

    // the node labels
    Text("(%d)  %s", m_node->m_level, m_name);

    // input connectors
    for (int i = 0; i < m_nbin; ++i)
    {
        top = this->getInputPos(i);

        SetCursorPos(top + ImVec2(14.f, -6.f));
        Text("%s", m_node->getSlotNameIn(i));

        draw_list->AddCircleFilled(top + offset, nodeRadius, bgEdge);
    }

    // output connectors
    for (int i = 0; i < m_nbout; ++i)
    {
        top = this->getOutputPos(i);

        SetCursorPos(top + ImVec2(12.f, -6.f));
        Text("%s", m_node->getSlotNameOut(i));

        draw_list->AddCircleFilled(top + offset, nodeRadius, bgEdge);
    }

    EndGroup();
    PopID();
}



template <typename T>
void ImGui::NodePropVec2<T>::drawNode()
{
    PushID(this->m_id);

    const float nodeRadius = 3.5f;
    ImColor bgBox(40,40,40), bgEdge(180,180,180);
    ImVec2 offset = GetWindowPos() - ImVec2(GetScrollX(), GetScrollY());

    // draw the nodes on the foreground (on the top of links)
    ImDrawList* draw_list = ImGui::GetWindowDrawList();
    draw_list->ChannelsSetCurrent(1);

    this->m_size.x = 220;
    this->m_size.y = 2 * (GetTextLineHeightWithSpacing() + 6.f);

    ImVec2 bottom = offset + this->m_pos;
    ImVec2 top    = offset + this->m_pos + this->m_size;

    // if hovered:
    if (IsMouseHoveringRect(bottom, top))
    {
        bgBox = ImColor(60,60,90); // ligthen background
    }

    // actual background draw
    draw_list->AddRectFilled(bottom, top, bgBox, 2.4f);

    SetCursorPos(this->m_pos + ImVec2(5.f,5.f));
    BeginGroup();

    // the node labels
    Text("%s", this->m_name);

    PushItemWidth(this->m_size.x - 10.f);
    DragFloat2("", m_value, 0.01f, 0.f, 1.f);
    PopItemWidth();

    // update data
    Ra::Engine::paramType t;
    Ra::Core::Vector2* v = static_cast<Ra::Core::Vector2*>( this->m_node->m_data->getDataPtr(&t) );
    *v = createVec2(m_value);

    // output connectors
    for (int i = 0; i < this->m_nbout; ++i)
    {
        top = this->getOutputPos(i);

        SetCursorPos(top + ImVec2(12.f, -6.f));
        Text("%s", this->m_node->getSlotNameOut(i));

        draw_list->AddCircleFilled(top + offset, nodeRadius, bgEdge);
    }

    EndGroup();
    PopID();
}



template <typename T>
void ImGui::NodePropVec3<T>::drawNode()
{
    PushID(this->m_id);

    const float nodeRadius = 3.5f;
    ImColor bgBox(40,40,40), bgEdge(180,180,180);
    ImVec2 offset = GetWindowPos() - ImVec2(GetScrollX(), GetScrollY());

    // draw the nodes on the foreground (on the top of links)
    ImDrawList* draw_list = ImGui::GetWindowDrawList();
    draw_list->ChannelsSetCurrent(1);

    this->m_size.x = 238;
    this->m_size.y = 2 * (GetTextLineHeightWithSpacing() + 6.f);

    ImVec2 bottom = offset + this->m_pos;
    ImVec2 top    = offset + this->m_pos + this->m_size;

    // if hovered:
    if (IsMouseHoveringRect(bottom, top))
    {
        bgBox = ImColor(60,60,90); // ligthen background
    }

    // actual background draw
    draw_list->AddRectFilled(bottom, top, bgBox, 2.4f);

    SetCursorPos(this->m_pos + ImVec2(5.f,5.f));
    BeginGroup();

    // the node labels
    Text("%s", this->m_name);

    PushItemWidth(this->m_size.x - 10.f);
    DragFloat3("", m_value, 0.01f, 0.f, 1.f);
    PopItemWidth();

    // update data
    Ra::Engine::paramType t;
    Ra::Core::Vector3* v = static_cast<Ra::Core::Vector3*>( this->m_node->m_data->getDataPtr(&t) );
    *v = createVec3(m_value);

    // output connectors
    for (int i = 0; i < this->m_nbout; ++i)
    {
        top = this->getOutputPos(i);

        SetCursorPos(top + ImVec2(12.f, -6.f));
        Text("%s", this->m_node->getSlotNameOut(i));

        draw_list->AddCircleFilled(top + offset, nodeRadius, bgEdge);
    }

    EndGroup();
    PopID();
}



template <typename T>
void ImGui::NodePropVec4<T>::drawNode()
{
    PushID(this->m_id);

    const float nodeRadius = 3.5f;
    ImColor bgBox(40,40,40), bgEdge(180,180,180);
    ImVec2 offset = GetWindowPos() - ImVec2(GetScrollX(), GetScrollY());

    // draw the nodes on the foreground (on the top of links)
    ImDrawList* draw_list = ImGui::GetWindowDrawList();
    draw_list->ChannelsSetCurrent(1);

    this->m_size.x = 256;
    this->m_size.y = 2 * (GetTextLineHeightWithSpacing() + 6.f);

    ImVec2 bottom = offset + this->m_pos;
    ImVec2 top    = offset + this->m_pos + this->m_size;

    // if hovered:
    if (IsMouseHoveringRect(bottom, top))
    {
        bgBox = ImColor(60,60,90); // ligthen background
    }

    // actual background draw
    draw_list->AddRectFilled(bottom, top, bgBox, 2.4f);

    SetCursorPos(this->m_pos + ImVec2(5.f,5.f));
    BeginGroup();

    // the node labels
    Text("%s", this->m_name);

    PushItemWidth(this->m_size.x - 10.f);
    DragFloat4("", m_value, 0.01f, 0.f, 1.f);
    PopItemWidth();

    // update data
    Ra::Engine::paramType t;
    Ra::Core::Vector4* v = static_cast<Ra::Core::Vector4*>( this->m_node->m_data->getDataPtr(&t) );
    *v = createVec4(m_value);

    // output connectors
    for (int i = 0; i < this->m_nbout; ++i)
    {
        top = this->getOutputPos(i);

        SetCursorPos(top + ImVec2(12.f, -6.f));
        Text("%s", this->m_node->getSlotNameOut(i));

        draw_list->AddCircleFilled(top + offset, nodeRadius, bgEdge);
    }

    EndGroup();
    PopID();
}



template <typename T>
void ImGui::NodePropMat2<T>::drawNode()
{
    PushID(this->m_id);

    const float nodeRadius = 3.5f;
    ImColor bgBox(40,40,40), bgEdge(180,180,180);
    ImVec2 offset = GetWindowPos() - ImVec2(GetScrollX(), GetScrollY());

    // draw the nodes on the foreground (on the top of links)
    ImDrawList* draw_list = ImGui::GetWindowDrawList();
    draw_list->ChannelsSetCurrent(1);

    this->m_size.x = 220;
    this->m_size.y = 3 * (GetTextLineHeightWithSpacing() + 6.f);

    ImVec2 bottom = offset + this->m_pos;
    ImVec2 top    = offset + this->m_pos + this->m_size;

    // if hovered:
    if (IsMouseHoveringRect(bottom, top))
    {
        bgBox = ImColor(60,60,90); // ligthen background
    }

    // actual background draw
    draw_list->AddRectFilled(bottom, top, bgBox, 2.4f);

    SetCursorPos(this->m_pos + ImVec2(5.f,5.f));
    BeginGroup();

    // the node labels
    Text("%s", this->m_name);

    PushItemWidth(this->m_size.x - 10.f);
    DragFloat2("", m_value,   0.01f, 0.f, 1.f);
    DragFloat2("", m_value+2, 0.01f, 0.f, 1.f);
    PopItemWidth();

    // update data
    Ra::Engine::paramType t;
    Ra::Core::Matrix2* v = static_cast<Ra::Core::Matrix2*>( this->m_node->m_data->getDataPtr(&t) );
    *v = createMat2(m_value);

    // output connectors
    for (int i = 0; i < this->m_nbout; ++i)
    {
        top = this->getOutputPos(i);

        SetCursorPos(top + ImVec2(12.f, -6.f));
        Text("%s", this->m_node->getSlotNameOut(i));

        draw_list->AddCircleFilled(top + offset, nodeRadius, bgEdge);
    }

    EndGroup();
    PopID();
}



template <typename T>
void ImGui::NodePropMat3<T>::drawNode()
{
    PushID(this->m_id);

    const float nodeRadius = 3.5f;
    ImColor bgBox(40,40,40), bgEdge(180,180,180);
    ImVec2 offset = GetWindowPos() - ImVec2(GetScrollX(), GetScrollY());

    // draw the nodes on the foreground (on the top of links)
    ImDrawList* draw_list = ImGui::GetWindowDrawList();
    draw_list->ChannelsSetCurrent(1);

    this->m_size.x = 238;
    this->m_size.y = 4 * (GetTextLineHeightWithSpacing() + 6.f);

    ImVec2 bottom = offset + this->m_pos;
    ImVec2 top    = offset + this->m_pos + this->m_size;

    // if hovered:
    if (IsMouseHoveringRect(bottom, top))
    {
        bgBox = ImColor(60,60,90); // ligthen background
    }

    // actual background draw
    draw_list->AddRectFilled(bottom, top, bgBox, 2.4f);

    SetCursorPos(this->m_pos + ImVec2(5.f,5.f));
    BeginGroup();

    // the node labels
    Text("%s", this->m_name);

    PushItemWidth(this->m_size.x - 10.f);
    DragFloat3("", m_value,   0.01f, 0.f, 1.f);
    DragFloat3("", m_value+3, 0.01f, 0.f, 1.f);
    DragFloat3("", m_value+6, 0.01f, 0.f, 1.f);
    PopItemWidth();

    // update data
    Ra::Engine::paramType t;
    Ra::Core::Matrix3* v = static_cast<Ra::Core::Matrix3*>( this->m_node->m_data->getDataPtr(&t) );
    *v = createMat3(m_value);

    // output connectors
    for (int i = 0; i < this->m_nbout; ++i)
    {
        top = this->getOutputPos(i);

        SetCursorPos(top + ImVec2(12.f, -6.f));
        Text("%s", this->m_node->getSlotNameOut(i));

        draw_list->AddCircleFilled(top + offset, nodeRadius, bgEdge);
    }

    EndGroup();
    PopID();
}



template <typename T>
void ImGui::NodePropMat4<T>::drawNode()
{
    PushID(this->m_id);

    const float nodeRadius = 3.5f;
    ImColor bgBox(40,40,40), bgEdge(180,180,180);
    ImVec2 offset = GetWindowPos() - ImVec2(GetScrollX(), GetScrollY());

    // draw the nodes on the foreground (on the top of links)
    ImDrawList* draw_list = ImGui::GetWindowDrawList();
    draw_list->ChannelsSetCurrent(1);

    this->m_size.x = 256.f;
    this->m_size.y = 5 * (GetTextLineHeightWithSpacing() + 6.f);

    ImVec2 bottom = offset + this->m_pos;
    ImVec2 top    = offset + this->m_pos + this->m_size;

    // if hovered:
    if (IsMouseHoveringRect(bottom, top))
    {
        bgBox = ImColor(60,60,90); // ligthen background
    }

    // actual background draw
    draw_list->AddRectFilled(bottom, top, bgBox, 2.4f);

    SetCursorPos(this->m_pos + ImVec2(5.f,5.f));
    BeginGroup();

    // the node labels
    Text("%s", this->m_name);

    PushItemWidth(this->m_size.x - 10.f);
    DragFloat4("", m_value,    0.01f, 0.f, 1.f);
    DragFloat4("", m_value+4,  0.01f, 0.f, 1.f);
    DragFloat4("", m_value+8,  0.01f, 0.f, 1.f);
    DragFloat4("", m_value+12, 0.01f, 0.f, 1.f);
    PopItemWidth();

    // update data
    Ra::Engine::paramType t;
    Ra::Core::Matrix4* v = static_cast<Ra::Core::Matrix4*>( this->m_node->m_data->getDataPtr(&t) );
    *v = createMat4(m_value);

    // output connectors
    for (int i = 0; i < this->m_nbout; ++i)
    {
        top = this->getOutputPos(i);

        SetCursorPos(top + ImVec2(12.f, -6.f));
        Text("%s", this->m_node->getSlotNameOut(i));

        draw_list->AddCircleFilled(top + offset, nodeRadius, bgEdge);
    }

    EndGroup();
    PopID();
}

