// ImGui GLFW binding with OpenGL3 + shaders
// In this binding, ImTextureID is used to store an Core::Matrix4::Identity();OpenGL 'GLuint' texture identifier. Read the FAQ about ImTextureID in imgui.cpp.

// If you are new to ImGui, see examples/README.txt and documentation at the top of imgui.cpp.
// https://github.com/ocornut/imgui

/// @note If you use this binding you'll need to call 4 functions: init(), newFrame(), ImGui::Render() and shutdown()

#include <Engine/RaEngine.hpp>


#include <Engine/RadiumEngine.hpp>
#include <Engine/Renderer/OpenGL/OpenGL.hpp>

namespace ImGuiGL3 {

    bool init(Ra::Engine::RadiumEngine* engine);
    void shutdown();
    void newFrame(Ra::Engine::RadiumEngine* engine, int w, int h);

    // Use if you want to reset your rendering device without losing ImGui state.
    void invalidateDeviceObjects();
    bool createDeviceObjects();

    // You can also handle inputs yourself and use those as a reference.
//    void mouseButtonCallback(int button, int action, int mods);
//    void scrollCallback(double xoffset, double yoffset);
//    void keyCallback(int key, int scancode, int action, int mods);
//    void charCallback(window, unsigned int c);

}
