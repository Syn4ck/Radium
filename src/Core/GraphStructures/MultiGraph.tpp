template <typename T>
MultiGraph<T>::Node::Node( MultiGraph<T>* host, const std::string& name, T* data, uint nbIn, uint nbOut)
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
void MultiGraph<T>::Node::setParent(uint slot, Node* other, uint local, bool reverse)
{
    Connection c = {slot, other, local};

    // if no existing connection is found
    if (std::find(m_parents.begin(), m_parents.end(), c) == m_parents.end())
    {
        if (reverse)
        {
            // call a specific function to connect both T
            if (m_graph->m_connect(other->m_data.get(), slot, this->m_data.get(), local))
            {
                // add the new connection in case it didn't exist before
                m_parents.push_back(c);
                other->setChild(local, this, slot, false);

                // set the graph to be updated
                m_graph->m_status = GRAPH_UPDATE;
            }
        }
        else
        {
            m_parents.push_back(c);
        }
    }
}



template <typename T>
void MultiGraph<T>::Node::removeParent(uint slot, Node* other, uint local, bool reverse)
{
    Connection c = {slot, other, local};
    auto ref = std::find(m_parents.begin(), m_parents.end(), c);

    // remove the edge if existant
    if (ref != m_parents.end())
    {
        m_parents.erase(ref);

        if (reverse)
        {
            other->removeChild(local, this, slot, false);
            m_graph->m_status = GRAPH_UPDATE;
        }
    }
}



template <typename T>
void MultiGraph<T>::Node::setChild(uint slot, Node* other, uint local, bool reverse)
{
    Connection c = {slot, other, local};

    // if no existing connection is found
    if (std::find(m_childs.begin(), m_childs.end(), c) == m_childs.end())
    {
        if (reverse)
        {
            // call a specific function to connect both T
            if (m_graph->m_connect(this->m_data.get(), local, other->m_data.get(), slot))
            {
                // add the new connection in case it didn't exist before
                m_childs.push_back(c);
                other->setParent(local, this, slot, false);

                // set the graph to be updated
                m_graph->m_status = GRAPH_UPDATE;
            }
        }
        else
        {
            m_childs.push_back(c);
        }
    }
}



template <typename T>
void MultiGraph<T>::Node::removeChild(uint slot, Node* other, uint local, bool reverse)
{
    Connection c = {slot, other, local};
    auto ref = std::find(m_childs.begin(), m_childs.end(), c);

    // remove the edge if existant
    if (ref != m_childs.end())
    {
        m_childs.erase(ref);

        if (reverse)
        {
            other->removeParent(slot, this, local, false);
            m_graph->m_status = GRAPH_UPDATE;
        }
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
typename MultiGraph<T>::Node* MultiGraph<T>::addNode(T* data)
{
    Node* newNode = new MultiGraph<T>::Node(this, data->getName(), data, data->getNbIn(), data->getNbOut());

    // create a unique pointer to the data stored
    // push it to the list and give it to the map
    m_graph.push_back( std::unique_ptr<Node>(newNode) );
    m_names[data->getName()] = newNode;

    // set update status
    m_status = GRAPH_NEW;

    return newNode;
}



template <typename T>
void MultiGraph<T>::deleteNode(Node* node)
{
    // notify the graph has been updated
    m_status = GRAPH_UPDATE;

    // delete every connections from the node
    for (auto const& conn : node->m_childs)
    {
         conn.m_source->removeParent(conn.m_local, node, conn.m_slot, false);
    }

    // delete every connections to the node
    for (auto const& conn : node->m_parents)
    {
        conn.m_source->removeChild(conn.m_local, node, conn.m_slot, false);
    }

    m_names.erase(node->m_name);

    // remove the unique ptr to this node
    auto nodeitr = m_graph.begin();
    while (nodeitr != m_graph.end())
    {
        if (nodeitr->get() == node)
        {
            m_graph.erase(nodeitr);
            break;
        }
        ++ nodeitr;
    }
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

    // set the graph to update status
    m_status = GRAPH_UPDATE;

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



template <typename T>
T& MultiGraph<T>::back()
{
    return *(m_graph.back()->m_data.get());
}
