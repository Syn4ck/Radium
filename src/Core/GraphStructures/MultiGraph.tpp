template <typename T>
MultiGraph<T>::Node::Node( MultiGraph<T>* host, const std::string& name, const std::shared_ptr<T>& data, uint nbIn, uint nbOut)
    : m_data (data)
    , m_name (name)
    , m_level(0)
    , m_nbIn (nbIn)
    , m_nbOut(nbOut)
    , m_graph(host)
{
}



template <typename T>
bool MultiGraph<T>::Node::Connection::operator==(const MultiGraph<T>::Node::Connection& other) const
{
    return (m_slot == other.m_slot) && (m_source == other.m_source) && (m_local == other.m_local);
}



template <typename T>
void MultiGraph<T>::Node::setParent(uint slot, Node* other, uint local)
{
    Connection c = {slot, other, local};

    // if no existing connection is found
    if (std::find(m_parents.begin(), m_parents.end(), c) == m_parents.end())
    {
        // call a specific function to connect both T
        if (m_graph->m_connect(other->m_data.get(), slot, this->m_data.get(), local))
        {
            // add the new connection in case it didn't exist before
            m_parents.push_back(c);
            other->setChild(local, this, slot);

            // set the graph to be updated
            m_graph->m_status = GRAPH_UPDATE;
        }
    }
}



template <typename T>
void MultiGraph<T>::Node::removeParent(uint slot, Node* other, uint local)
{
    Connection c = {slot, other, local};
    auto ref = std::find(m_parents.begin(), m_parents.end(), c);

    // remove the edge if existant
    if (ref != m_parents.end())
    {
        m_parents.erase(ref);
        other->removeChild(local, this, slot);
        m_graph->m_status = GRAPH_UPDATE;
    }
}



template <typename T>
void MultiGraph<T>::Node::setChild(uint slot, Node* other, uint local)
{
    Connection c = {slot, other, local};

    // if no existing connection is found
    if (std::find(m_childs.begin(), m_childs.end(), c) == m_childs.end())
    {
        m_childs.push_back(c);
    }
}



template <typename T>
void MultiGraph<T>::Node::removeChild(uint slot, Node* other, uint local)
{
    Connection c = {slot, other, local};
    auto ref = std::find(m_childs.begin(), m_childs.end(), c);

    // remove the edge if existant
    if (ref != m_childs.end())
    {
        m_childs.erase(ref);
    }
}



template <typename T>
void MultiGraph<T>::Node::updateParameters() const
{
    // reset every connections
    for (auto const& c : m_parents)
    {
        m_graph->m_connect(c.m_source->m_data.get(), c.m_slot, this->m_data.get(), c.m_local);
    }
}



template <typename T>
const std::string& MultiGraph<T>::Node::getSlotNameIn(uint slot)
{
    return m_graph->m_slotname_in(this->m_data.get(), slot);
}



template <typename T>
const std::string& MultiGraph<T>::Node::getSlotNameOut(uint slot)
{
    return m_graph->m_slotname_out(this->m_data.get(), slot);
}



template <typename T>
void MultiGraph<T>::Node::updateLevel()
{
    bool ok = true;
    uint maxLevel = 0;

    // every parent must have a positive level
    for (auto& c : m_parents)
    {
        ok &= (c.m_source->m_level > 0);

        // keep track of the maximum level
        maxLevel = std::max(maxLevel, c.m_source->m_level);
    }

    if (ok)
    {
        m_level = maxLevel + 1;
    }
}



template <typename T>
bool MultiGraph<T>::Node::operator<(const MultiGraph<T>::Node& other) const
{
    return m_level < other.m_level;
}



template <typename T>
void MultiGraph<T>::Node::print() const
{
    // print parents
    for (Connection const& c : m_parents)
    {
        std::cout << c.m_source->m_name << "(" << c.m_slot << "," << c.m_local << ") -> ";
    }
    std::cout << m_name << "(level. " << m_level << ")" << std::endl;
}



template <typename T>
void MultiGraph<T>::addNode(const std::string& name, const std::shared_ptr<T>& data, uint nb_in, uint nb_out)
{
    Node* newNode = new MultiGraph<T>::Node(this, name, data, nb_in, nb_out);
    m_graph.push_back(std::unique_ptr<Node>(newNode));
    m_names[name] = newNode;
}



template <typename T>
void MultiGraph<T>::updateNodes() const
{
    // Update every nodes in the graph.
    //
    // This is required in order to satisfy parameters propagation
    // without doing recursive calls.
    //
    // This function assumes the graph was previously sorted.

    for (auto const& node : m_graph)
    {
        node->updateParameters();
    }
}



template <typename T>
void MultiGraph<T>::levelize(bool sortByLevel)
{
    uint total = m_graph.size();

    // first detect every sources and set level to 1 else reset to 0
    auto is_source = [](Node* n) { return n->m_nbIn == 0; };
    for (auto const& node : m_graph)
    {
        if (is_source(node.get()))
        {
            node->m_level = 1;
            total -= 1;
        }
        else
        {
            node->m_level = 0;
        }
    }

    // and then, while all the nodes aren't assigned with a level,
    // try to assign a level to new nodes
    while (total > 0)
    {
        for (auto const& node : m_graph)
        {
            if (node->m_level == 0)
            {
                // if no level is found try to infere it
                node->updateLevel();

                // decrease if ok
                if (node->m_level != 0)
                {
                    total -= 1;
                }
            }
        }
    }

    // finally sort if required
    if (sortByLevel)
    {
        auto comp = [](const std::unique_ptr<Node>& a, const std::unique_ptr<Node>& b)
        {
            return (a->m_level < b->m_level);
        };

        std::sort(m_graph.begin(), m_graph.end(), comp);
    }
}



template <typename T>
void MultiGraph<T>::print() const
{
    std::cout << std::endl << "DEBUG - printing graph -" << std::endl;
    for (auto const& node : m_graph)
    {
        node->print();
    }
}



template <typename T>
typename MultiGraph<T>::Node* MultiGraph<T>::operator[](const std::string& name)
{
    return m_names[name];
}



template <typename T>
typename MultiGraph<T>::Node* MultiGraph<T>::operator[](const int index)
{
    return m_graph[index].get();
}



template <typename T>
typename std::vector<std::unique_ptr<typename MultiGraph<T>::Node>>::iterator MultiGraph<T>::begin()
{
    return m_graph.begin();
}



template <typename T>
typename std::vector<std::unique_ptr<typename MultiGraph<T>::Node>>::iterator MultiGraph<T>::end()
{
    return m_graph.end();
}



template <typename T>
uint MultiGraph<T>::size() const
{
    return m_graph.size();
}
