#include <Engine/Renderer/RenderObject/RenderObjectManager.hpp>

#include <Core/CoreMacros.hpp>

namespace Ra
{

Engine::RenderObjectManager::RenderObjectManager()
{

}

Engine::RenderObjectManager::~RenderObjectManager()
{
}

Core::Index Engine::RenderObjectManager::addRenderObject(RenderObject* renderObject)
{
	// Avoid data race in the std::maps
	std::lock_guard<std::mutex> lock(m_doubleBufferMutex);

    std::shared_ptr<RenderObject> newRenderObject(renderObject);
    Core::Index idx = m_renderObjects.insert(newRenderObject);

    newRenderObject->idx = idx;

    return idx;
}

void Engine::RenderObjectManager::removeRenderObject(const Core::Index & index)
{
	std::lock_guard<std::mutex> lock(m_doubleBufferMutex);

    // FIXME(Charly): Should we check if the render object is in the double buffer map ?
    std::shared_ptr<RenderObject> renderObject = m_renderObjects.at(index);
    m_renderObjects.remove(index);
    renderObject.reset();
}

std::vector<std::shared_ptr<Engine::RenderObject>> Engine::RenderObjectManager::getRenderObjects() const
{
    // Take the mutex
    std::lock_guard<std::mutex> lock(m_doubleBufferMutex);

    // Copy each element in m_renderObjects
    std::vector<std::shared_ptr<RenderObject>> renderObjects;

    for (uint i = 0; i < m_renderObjects.size(); ++i)
	{
        renderObjects.push_back(m_renderObjects.at(i));
	}

    return renderObjects;
}

std::shared_ptr<Engine::RenderObject> Engine::RenderObjectManager::update(uint index)
{
	Core::Index idx(index);

	return update(idx);
}

std::shared_ptr<Engine::RenderObject> Engine::RenderObjectManager::update(const Core::Index& index)
{
    // A render object should never be updated if it is already in use.
    // It might be :
    //     - update called by another component
    //     - doneUpdating not called on a previous update
    if (m_doubleBuffer.find(index) != m_doubleBuffer.end())
    {
        CORE_ERROR("The render object required for update is already "
                   "being updated by someone else. "
                   "Maybe you forgot to call RenderObjectManager::doneUpdating() ?");
    }

    // Take the mutex
    std::lock_guard<std::mutex> lock(m_doubleBufferMutex);

    // Clone the render object
    std::shared_ptr<RenderObject> copy = std::shared_ptr<RenderObject>(
                m_renderObjects[index]->clone());

    // Store it in the double buffer map
    m_doubleBuffer[index] = copy;
    return copy;
}

void Engine::RenderObjectManager::doneUpdating(uint index)
{
    // Take the mutex
    std::lock_guard<std::mutex> lock(m_doubleBufferMutex);

    std::shared_ptr<RenderObject> oldRenderObject = m_renderObjects[index];
    std::shared_ptr<RenderObject> newRenderObject = m_doubleBuffer[index];

    // We delete the old render object. If it is still in used by the renderer,
    // the pointer will still be valid for it.
    oldRenderObject.reset();

    // Buffer swapping
    m_renderObjects[index] = newRenderObject;

    // Remove the double buffer entry
    m_doubleBuffer.erase(index);
}

} // namespace Ra
