#ifndef IMGUIPLUGIN_COMPONENT_HPP_
#define IMGUIPLUGIN_COMPONENT_HPP_

#include <ImGuiPlugin.hpp>

#include <Engine/Component/Component.hpp>

namespace ImGuiPlugin
{

    class IMGUI_PLUGIN_API ImGuiComponent : public Ra::Engine::Component
    {
    public:
        ImGuiComponent(const std::string& name) : Component(name), m_contentName(name) {}
        virtual ~ImGuiComponent() {}

        virtual void initialize() override;

//        bool picked(uint drawableIdx);
//        void getProperties(Ra::Core::AlignedStdVector<Ra::Engine::EditableProperty> &propsOut) const;
//        void setProperty(const Ra::Engine::EditableProperty &prop);
        void update(Scalar dt);

    private:
        // Component communication
        void setupIO( const std::string& id );

    private:
        std::string m_contentName;
    };

} // namespace ImGuiPlugin

#endif //IMGUIPLUGIN_COMPONENT_HPP_
