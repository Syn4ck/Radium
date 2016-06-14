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
void ImGui::NodeProp<T>::drawBackground( const std::string& title,
                                         const ImVec2&      offset,
                                         const ImColor&     bg )
{
    const ImColor edge( 180, 180, 180 );
    const float   radius = 3.5f;

    ImDrawList* draw_list = ImGui::GetWindowDrawList();
    ImVec2 slotPos;
    ImVec2 cursorPos;
    ImVec2 bottom = offset + m_pos;
    ImVec2 top    = offset + m_pos + m_size;


    // actual background draw
    // if hovered:
    if (IsMouseHoveringRect(bottom, top))
    {
        draw_list->AddRectFilled(bottom, top, ImColor(53,50,80,200), 2.4f);
    }
    else
    {
        draw_list->AddRectFilled(bottom, top, bg, 2.4f);
    }

    // the node labels
    Text("%s", title.c_str());

    cursorPos = GetCursorPos();

    // input connectors
    for (int i = 0; i < m_nbin; ++i)
    {
        slotPos = this->getInputPos(i);

        SetCursorPos(slotPos + ImVec2(14.f, -6.f));
        Text("%s", m_node->getSlotNameIn(i).c_str());

        draw_list->AddCircleFilled(slotPos + offset, radius, edge);
    }

    // output connectors
    for (int i = 0; i < m_nbout; ++i)
    {
        slotPos = this->getOutputPos(i);

        SetCursorPos(slotPos + ImVec2(12.f, -6.f));
        Text("%s", m_node->getSlotNameOut(i).c_str());

        draw_list->AddCircleFilled(slotPos + offset, radius, edge);
    }


    // reset cursor position
    SetCursorPos(cursorPos);
}



template <typename T>
void ImGui::NodeProp<T>::drawNode()
{
    const ImVec2  offset = GetWindowPos() - ImVec2(GetScrollX(), GetScrollY());
    ImDrawList*   draw_list = ImGui::GetWindowDrawList();

    SetCursorPos(m_pos + ImVec2(5.f,5.f));

    PushID(m_id);
    BeginGroup();
    draw_list->ChannelsSetCurrent(1);

    drawBackground( this->m_name, offset );

    // do nothing for a basic node

    draw_list->ChannelsSetCurrent(0);
    EndGroup();
    PopID();
}



template <typename T>
void ImGui::NodePropInt<T>::drawNode()
{
    const ImVec2  offset = GetWindowPos() - ImVec2(GetScrollX(), GetScrollY());
    ImDrawList*   draw_list = ImGui::GetWindowDrawList();

    this->m_size.x = 128;
    this->m_size.y = 2 * (GetTextLineHeightWithSpacing() + 6.f);

    SetCursorPos(this->m_pos + ImVec2(5.f,5.f));

    PushID(this->m_id);
    BeginGroup();
    draw_list->ChannelsSetCurrent(1);

    this->drawBackground( this->m_name, offset );

    // draw a slider
    PushItemWidth(this->m_size.x - 10.f);
    DragInt("", &m_value);
    PopItemWidth();

    // update data
    Ra::Engine::paramType t;
    int* x = static_cast<int*>( this->m_node->m_data->getDataPtr(&t) );
    *x = m_value;

    draw_list->ChannelsSetCurrent(0);
    EndGroup();
    PopID();
}



template <typename T>
void ImGui::NodePropUint<T>::drawNode()
{
    const ImVec2  offset = GetWindowPos() - ImVec2(GetScrollX(), GetScrollY());
    ImDrawList*   draw_list = ImGui::GetWindowDrawList();

    this->m_size.x = 128;
    this->m_size.y = 2 * (GetTextLineHeightWithSpacing() + 6.f);

    SetCursorPos(this->m_pos + ImVec2(5.f,5.f));

    PushID(this->m_id);
    BeginGroup();
    draw_list->ChannelsSetCurrent(1);

    this->drawBackground( this->m_name, offset );

    // draw a slider
    PushItemWidth(this->m_size.x - 10.f);
    DragInt("", &m_value, this->m_rangeStep, this->m_rangeMin, this->m_rangeMax);
    PopItemWidth();

    // update data
    Ra::Engine::paramType t;
    unsigned int* x = static_cast<unsigned int*>( this->m_node->m_data->getDataPtr(&t) );
    *x = static_cast<unsigned int>(m_value);

    draw_list->ChannelsSetCurrent(0);
    EndGroup();
    PopID();
}



template <typename T>
void ImGui::NodePropScalar<T>::drawNode()
{
    const ImVec2  offset = GetWindowPos() - ImVec2(GetScrollX(), GetScrollY());
    ImDrawList*   draw_list = ImGui::GetWindowDrawList();

    this->m_size.x = 128;
    this->m_size.y = 2 * (GetTextLineHeightWithSpacing() + 6.f);

    SetCursorPos(this->m_pos + ImVec2(5.f,5.f));

    PushID(this->m_id);
    BeginGroup();
    draw_list->ChannelsSetCurrent(1);

    this->drawBackground( this->m_name, offset );

    PushItemWidth(this->m_size.x - 10.f);
    DragFloat("", &m_value, this->m_rangeStep, this->m_rangeMin, this->m_rangeMax);
    PopItemWidth();

    // update data
    Ra::Engine::paramType t;
    Scalar* x = static_cast<Scalar*>( this->m_node->m_data->getDataPtr(&t) );
    *x = m_value;

    draw_list->ChannelsSetCurrent(0);
    EndGroup();
    PopID();
}



template <typename T>
void ImGui::NodePropVec2<T>::drawNode()
{
    const ImVec2  offset = GetWindowPos() - ImVec2(GetScrollX(), GetScrollY());
    ImDrawList*   draw_list = ImGui::GetWindowDrawList();

    this->m_size.x = 220;
    this->m_size.y = 2 * (GetTextLineHeightWithSpacing() + 6.f);

    SetCursorPos(this->m_pos + ImVec2(5.f,5.f));

    PushID(this->m_id);
    BeginGroup();
    draw_list->ChannelsSetCurrent(1);

    this->drawBackground( this->m_name, offset );

    PushItemWidth(this->m_size.x - 10.f);
    DragFloat2("", m_value, this->m_rangeStep, this->m_rangeMin, this->m_rangeMax);
    PopItemWidth();

    // update data
    Ra::Engine::paramType t;
    Ra::Core::Vector2* v = static_cast<Ra::Core::Vector2*>( this->m_node->m_data->getDataPtr(&t) );
    *v = createVec2(m_value);

    draw_list->ChannelsSetCurrent(0);
    EndGroup();
    PopID();
}



template <typename T>
void ImGui::NodePropVec3<T>::drawNode()
{
    const ImVec2  offset = GetWindowPos() - ImVec2(GetScrollX(), GetScrollY());
    ImDrawList*   draw_list = ImGui::GetWindowDrawList();

    this->m_size.x = 238;
    this->m_size.y = 2 * (GetTextLineHeightWithSpacing() + 6.f);

    SetCursorPos(this->m_pos + ImVec2(5.f,5.f));

    PushID(this->m_id);
    BeginGroup();
    draw_list->ChannelsSetCurrent(1);

    this->drawBackground( this->m_name, offset );

    PushItemWidth(this->m_size.x - 10.f);
    DragFloat3("", m_value, this->m_rangeStep, this->m_rangeMin, this->m_rangeMax);
    PopItemWidth();

    // update data
    Ra::Engine::paramType t;
    Ra::Core::Vector3* v = static_cast<Ra::Core::Vector3*>( this->m_node->m_data->getDataPtr(&t) );
    *v = createVec3(m_value);

    draw_list->ChannelsSetCurrent(0);
    EndGroup();
    PopID();
}



template <typename T>
void ImGui::NodePropVec4<T>::drawNode()
{
    const ImVec2  offset = GetWindowPos() - ImVec2(GetScrollX(), GetScrollY());
    ImDrawList*   draw_list = ImGui::GetWindowDrawList();

    this->m_size.x = 256;
    this->m_size.y = 2 * (GetTextLineHeightWithSpacing() + 6.f);

    SetCursorPos(this->m_pos + ImVec2(5.f,5.f));

    PushID(this->m_id);
    BeginGroup();
    draw_list->ChannelsSetCurrent(1);

    this->drawBackground( this->m_name, offset );

    PushItemWidth(this->m_size.x - 10.f);
    DragFloat4("", m_value, this->m_rangeStep, this->m_rangeMin, this->m_rangeMax);
    PopItemWidth();

    // update data
    Ra::Engine::paramType t;
    Ra::Core::Vector4* v = static_cast<Ra::Core::Vector4*>( this->m_node->m_data->getDataPtr(&t) );
    *v = createVec4(m_value);

    draw_list->ChannelsSetCurrent(0);
    EndGroup();
    PopID();
}



template <typename T>
void ImGui::NodePropMat2<T>::drawNode()
{
    const ImVec2  offset = GetWindowPos() - ImVec2(GetScrollX(), GetScrollY());
    ImDrawList*   draw_list = ImGui::GetWindowDrawList();

    this->m_size.x = 220;
    this->m_size.y = 3 * (GetTextLineHeightWithSpacing() + 6.f);

    SetCursorPos(this->m_pos + ImVec2(5.f,5.f));

    PushID(this->m_id);
    BeginGroup();
    draw_list->ChannelsSetCurrent(1);

    this->drawBackground( this->m_name, offset );

    PushItemWidth(this->m_size.x - 10.f);
    DragFloat2("", m_value,   this->m_rangeStep, this->m_rangeMin, this->m_rangeMax);
    DragFloat2("", m_value+2, this->m_rangeStep, this->m_rangeMin, this->m_rangeMax);
    PopItemWidth();

    // update data
    Ra::Engine::paramType t;
    Ra::Core::Matrix2* v = static_cast<Ra::Core::Matrix2*>( this->m_node->m_data->getDataPtr(&t) );
    *v = createMat2(m_value);

    draw_list->ChannelsSetCurrent(0);
    EndGroup();
    PopID();
}



template <typename T>
void ImGui::NodePropMat3<T>::drawNode()
{
    const ImVec2  offset = GetWindowPos() - ImVec2(GetScrollX(), GetScrollY());
    ImDrawList*   draw_list = ImGui::GetWindowDrawList();

    this->m_size.x = 238;
    this->m_size.y = 4 * (GetTextLineHeightWithSpacing() + 6.f);

    SetCursorPos(this->m_pos + ImVec2(5.f,5.f));

    PushID(this->m_id);
    BeginGroup();
    draw_list->ChannelsSetCurrent(1);

    this->drawBackground( this->m_name, offset );

    PushItemWidth(this->m_size.x - 10.f);
    DragFloat3("", m_value,   this->m_rangeStep, this->m_rangeMin, this->m_rangeMax);
    DragFloat3("", m_value+3, this->m_rangeStep, this->m_rangeMin, this->m_rangeMax);
    DragFloat3("", m_value+6, this->m_rangeStep, this->m_rangeMin, this->m_rangeMax);
    PopItemWidth();

    // update data
    Ra::Engine::paramType t;
    Ra::Core::Matrix3* v = static_cast<Ra::Core::Matrix3*>( this->m_node->m_data->getDataPtr(&t) );
    *v = createMat3(m_value);

    draw_list->ChannelsSetCurrent(0);
    EndGroup();draw_list->ChannelsSetCurrent(1);
    PopID();
}



template <typename T>
void ImGui::NodePropMat4<T>::drawNode()
{
    const ImVec2  offset = GetWindowPos() - ImVec2(GetScrollX(), GetScrollY());
    ImDrawList*   draw_list = ImGui::GetWindowDrawList();

    this->m_size.x = 256;
    this->m_size.y = 5 * (GetTextLineHeightWithSpacing() + 6.f);

    SetCursorPos(this->m_pos + ImVec2(5.f,5.f));

    PushID(this->m_id);
    BeginGroup();
    draw_list->ChannelsSetCurrent(1);

    this->drawBackground( this->m_name, offset );

    PushItemWidth(this->m_size.x - 10.f);
    DragFloat4("", m_value,    this->m_rangeStep, this->m_rangeMin, this->m_rangeMax);
    DragFloat4("", m_value+4,  this->m_rangeStep, this->m_rangeMin, this->m_rangeMax);
    DragFloat4("", m_value+8,  this->m_rangeStep, this->m_rangeMin, this->m_rangeMax);
    DragFloat4("", m_value+12, this->m_rangeStep, this->m_rangeMin, this->m_rangeMax);
    PopItemWidth();

    // update data
    Ra::Engine::paramType t;
    Ra::Core::Matrix4* v = static_cast<Ra::Core::Matrix4*>( this->m_node->m_data->getDataPtr(&t) );
    *v = createMat4(m_value);

    draw_list->ChannelsSetCurrent(0);
    EndGroup();
    PopID();
}

