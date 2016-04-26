#include <Core/GraphStructures/MultiGraph.hpp>

#include <algorithm>

namespace Ra
{
    namespace Core
    {

        template <typename T>
        MultiGraph<T>::Node::Node(const T& data, uint n_in, uint n_out)
            : m_data(data )
            , m_nIn (n_in )
            , m_nOut(n_out)
            , m_deps(n_in )  // deps to be resolved before being rendered
        {
        }

        template <typename T>
        MultiGraph<T>::MultiGraph()
        {
        }

        template <typename T>
        void MultiGraph<T>::Node::setParent(uint local, const T *other, uint slot)
        {
            Connection c = {local, other, slot};

            if (std::find(m_parents.begin(), m_parents.end(), c) == m_parents.end())
            {
                // add the new connection in case it didn't exist before
                m_parents.push_back(c);
            }

            // then tell the new child this is one of its new parents
            other->setChildren(slot, this, local);
        }

        template <typename T>
        void MultiGraph<T>::Node::setChildren(uint local, const T *other, uint slot)
        {
            Connection c = {local, other, slot};

            if (std::find(m_childs.begin(), m_childs.end(), c) == m_childs.end())
            {
                // add the new connection in case it didn't exist before
                m_childs.push_back(c);
            }

            // then tell the new child this is one of its new parents
            other->setParent(slot, this, local);
        }

        template <typename T>
        void MultiGraph<T>::Node::removeParent(uint local, const T *other, uint slot)
        {
            Connection c = {local, other, slot};
            auto ref = std::find(m_parents.begin(), m_parents.end(), c);

            // remove the edge if existant
            if (ref != m_parents.end())
            {
                m_parents.erase(ref);
            }

            other->removeChildren(slot, this, local);
        }

        template <typename T>
        void MultiGraph<T>::Node::removeChildren(uint local, const T *other, uint slot)
        {
            Connection c = {local, other, slot};
            auto ref = std::find(m_childs.begin(), m_childs.end(), c);

            // remove the edge if existant
            if (ref != m_childs.end())
            {
                m_childs.erase(ref);
            }

            other->removeParent(slot, this, local);
        }

        template <typename T>
        MultiGraph<T>::MultiGraph(const std::vector<T>& other)
            : m_graph(other)
        {
        }

//        template <typename T>
//        MultiGraph<T>::levelize(uint i) {
//            // ...
//        }


    }
}
