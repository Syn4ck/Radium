// ImGui GLFW binding with OpenGL3 + shaders
// In this binding, ImTextureID is used to store an OpenGL 'GLuint' texture identifier. Read the FAQ about ImTextureID in imgui.cpp.

// If you are new to ImGui, see examples/README.txt and documentation at the top of imgui.cpp.
// https://github.com/ocornut/imgui

// This is a stripped down version of imgui_impl_glfw_gl3.cpp which suited really well to this needs
// Thanks to the authors

#include <imgui.h>
#include <ImGuiGl3.hpp>

#include <Core/Event/Key.hpp>
#include <Core/Event/EventEnums.hpp>
#include <QRadium/EventTranslator.hpp>

#include <iostream>

static char keymap(int key, Ra::Engine::InputStatus& inputstts)
{
    if ((inputstts.modifiers & Ra::Core::Modifier_Shift) == 0)
    {
        switch (key) {
        // letters
        case 16: return 'q'; break;
        case 22: return 'w'; break;
        case 4 : return 'e'; break;
        case 17: return 'r'; break;
        case 19: return 't'; break;
        case 24: return 'y'; break;
        case 20: return 'u'; break;
        case 8 : return 'i'; break;
        case 14: return 'o'; break;
        case 15: return 'p'; break;
        case 0 : return 'a'; break;
        case 18: return 's'; break;
        case 3 : return 'd'; break;
        case 5 : return 'f'; break;
        case 6 : return 'g'; break;
        case 7 : return 'h'; break;
        case 9 : return 'j'; break;
        case 10: return 'k'; break;
        case 11: return 'l'; break;
        case 25: return 'z'; break;
        case 23: return 'x'; break;
        case 2 : return 'c'; break;
        case 21: return 'v'; break;
        case 1 : return 'b'; break;
        case 13: return 'n'; break;
        case 12: return 'm'; break;
        // numbers
        case 26: return '0'; break;
        case 27: return '1'; break;
        case 28: return '2'; break;
        case 29: return '3'; break;
        case 30: return '4'; break;
        case 31: return '5'; break;
        case 32: return '6'; break;
        case 33: return '7'; break;
        case 34: return '8'; break;
        case 35: return '9'; break;
        default: break;
        }
    }
    else
    {
        switch (key) {
        // letters
        case 16: return 'Q'; break;
        case 22: return 'W'; break;
        case 4 : return 'E'; break;
        case 17: return 'R'; break;
        case 19: return 'T'; break;
        case 24: return 'Y'; break;
        case 20: return 'U'; break;
        case 8 : return 'I'; break;
        case 14: return 'O'; break;
        case 15: return 'P'; break;
        case 0 : return 'A'; break;
        case 18: return 'S'; break;
        case 3 : return 'D'; break;
        case 5 : return 'F'; break;
        case 6 : return 'G'; break;
        case 7 : return 'H'; break;
        case 9 : return 'J'; break;
        case 10: return 'K'; break;
        case 11: return 'L'; break;
        case 25: return 'Z'; break;
        case 23: return 'X'; break;
        case 2 : return 'C'; break;
        case 21: return 'V'; break;
        case 1 : return 'B'; break;
        case 13: return 'N'; break;
        case 12: return 'M'; break;
        default: break;
        }
    }

    // control
    switch (key)
    {
    case 48: return ','; break;
    case 49: return '.'; break;
    case 58: return ' '; break;
    default: return -1;
    }
}

namespace ImGuiGL3 {

// Data
static GLuint       g_FontTexture = 0;
static int          g_ShaderHandle = 0, g_VertHandle = 0, g_FragHandle = 0;
static int          g_AttribLocationTex = 0, g_AttribLocationProjMtx = 0;
static int          g_AttribLocationPosition = 0, g_AttribLocationUV = 0, g_AttribLocationColor = 0;
static unsigned int g_VboHandle = 0, g_VaoHandle = 0, g_ElementsHandle = 0;

// This is the main rendering function that you have to implement and provide to ImGui (via setting up 'RenderDrawListsFn' in the ImGuiIO structure)
// If text or lines are blurry when integrating ImGui in your engine:
// - in your Render function, try translating your projection matrix by (0.5f,0.5f) or (0.375f,0.375f)
void renderDrawLists(ImDrawData* draw_data)
{
    // Avoid rendering when minimized, scale coordinates for retina displays (screen coordinates != framebuffer coordinates)
    ImGuiIO& io = ImGui::GetIO();
    int fb_width = (int)(io.DisplaySize.x * io.DisplayFramebufferScale.x);
    int fb_height = (int)(io.DisplaySize.y * io.DisplayFramebufferScale.y);
    if (fb_width == 0 || fb_height == 0)
        return;
    draw_data->ScaleClipRects(io.DisplayFramebufferScale);

    // Backup GL state
    GLint last_program; glGetIntegerv(GL_CURRENT_PROGRAM, &last_program);
    GLint last_texture; glGetIntegerv(GL_TEXTURE_BINDING_2D, &last_texture);
    GLint last_active_texture; glGetIntegerv(GL_ACTIVE_TEXTURE, &last_active_texture);
    GLint last_array_buffer; glGetIntegerv(GL_ARRAY_BUFFER_BINDING, &last_array_buffer);
    GLint last_element_array_buffer; glGetIntegerv(GL_ELEMENT_ARRAY_BUFFER_BINDING, &last_element_array_buffer);
    GLint last_vertex_array; glGetIntegerv(GL_VERTEX_ARRAY_BINDING, &last_vertex_array);
    GLint last_blend_src; glGetIntegerv(GL_BLEND_SRC, &last_blend_src);
    GLint last_blend_dst; glGetIntegerv(GL_BLEND_DST, &last_blend_dst);
    GLint last_blend_equation_rgb; glGetIntegerv(GL_BLEND_EQUATION_RGB, &last_blend_equation_rgb);
    GLint last_blend_equation_alpha; glGetIntegerv(GL_BLEND_EQUATION_ALPHA, &last_blend_equation_alpha);
    GLint last_viewport[4]; glGetIntegerv(GL_VIEWPORT, last_viewport);
    GLboolean last_enable_blend = glIsEnabled(GL_BLEND);
    GLboolean last_enable_cull_face = glIsEnabled(GL_CULL_FACE);
    GLboolean last_enable_depth_test = glIsEnabled(GL_DEPTH_TEST);
    GLboolean last_enable_scissor_test = glIsEnabled(GL_SCISSOR_TEST);

    // Setup render state: alpha-blending enabled, no face culling, no depth testing, scissor enabled
    glEnable(GL_BLEND);
    glBlendEquation(GL_FUNC_ADD);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glDisable(GL_CULL_FACE);
    glDisable(GL_DEPTH_TEST);
    glEnable(GL_SCISSOR_TEST);
    glActiveTexture(GL_TEXTURE0);

    // Setup viewport, orthographic projection matrix
    glViewport(0, 0, (GLsizei)fb_width, (GLsizei)fb_height);
    const float ortho_projection[4][4] =
    {
        { 2.0f/io.DisplaySize.x, 0.0f,                   0.0f, 0.0f },
        { 0.0f,                  2.0f/-io.DisplaySize.y, 0.0f, 0.0f },
        { 0.0f,                  0.0f,                  -1.0f, 0.0f },
        {-1.0f,                  1.0f,                   0.0f, 1.0f },
    };
    glUseProgram(g_ShaderHandle);
    glUniform1i(g_AttribLocationTex, 0);
    glUniformMatrix4fv(g_AttribLocationProjMtx, 1, GL_FALSE, &ortho_projection[0][0]);
    glBindVertexArray(g_VaoHandle);

    for (int n = 0; n < draw_data->CmdListsCount; n++)
    {
        const ImDrawList* cmd_list = draw_data->CmdLists[n];
        const ImDrawIdx* idx_buffer_offset = 0;

        glBindBuffer(GL_ARRAY_BUFFER, g_VboHandle);
        glBufferData(GL_ARRAY_BUFFER, (GLsizeiptr)cmd_list->VtxBuffer.size() * sizeof(ImDrawVert), (GLvoid*)&cmd_list->VtxBuffer.front(), GL_STREAM_DRAW);

        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, g_ElementsHandle);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, (GLsizeiptr)cmd_list->IdxBuffer.size() * sizeof(ImDrawIdx), (GLvoid*)&cmd_list->IdxBuffer.front(), GL_STREAM_DRAW);

        for (const ImDrawCmd* pcmd = cmd_list->CmdBuffer.begin(); pcmd != cmd_list->CmdBuffer.end(); pcmd++)
        {
            if (pcmd->UserCallback)
            {
                pcmd->UserCallback(cmd_list, pcmd);
            }
            else
            {
                glBindTexture(GL_TEXTURE_2D, (GLuint)(intptr_t)pcmd->TextureId);
                glScissor((int)pcmd->ClipRect.x, (int)(fb_height - pcmd->ClipRect.w), (int)(pcmd->ClipRect.z - pcmd->ClipRect.x), (int)(pcmd->ClipRect.w - pcmd->ClipRect.y));
                glDrawElements(GL_TRIANGLES, (GLsizei)pcmd->ElemCount, sizeof(ImDrawIdx) == 2 ? GL_UNSIGNED_SHORT : GL_UNSIGNED_INT, idx_buffer_offset);
            }
            idx_buffer_offset += pcmd->ElemCount;
        }
    }

    // Restore modified GL state
    glUseProgram(last_program);
    glActiveTexture(last_active_texture);
    glBindTexture(GL_TEXTURE_2D, last_texture);
    glBindVertexArray(last_vertex_array);
    glBindBuffer(GL_ARRAY_BUFFER, last_array_buffer);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, last_element_array_buffer);
    glBlendEquationSeparate(last_blend_equation_rgb, last_blend_equation_alpha);
    glBlendFunc(last_blend_src, last_blend_dst);
    if (last_enable_blend) glEnable(GL_BLEND); else glDisable(GL_BLEND);
    if (last_enable_cull_face) glEnable(GL_CULL_FACE); else glDisable(GL_CULL_FACE);
    if (last_enable_depth_test) glEnable(GL_DEPTH_TEST); else glDisable(GL_DEPTH_TEST);
    if (last_enable_scissor_test) glEnable(GL_SCISSOR_TEST); else glDisable(GL_SCISSOR_TEST);
    glViewport(last_viewport[0], last_viewport[1], (GLsizei)last_viewport[2], (GLsizei)last_viewport[3]);
}

bool createFontsTexture()
{
    // Build texture atlas
    ImGuiIO& io = ImGui::GetIO();
    unsigned char* pixels;
    int width, height;
    io.Fonts->GetTexDataAsRGBA32(&pixels, &width, &height);   // Load as RGBA 32-bits for OpenGL3 demo because it is more likely to be compatible with user's existing shader.

    // Upload texture to graphics system
    GLint last_texture;
    glGetIntegerv(GL_TEXTURE_BINDING_2D, &last_texture);
    glGenTextures(1, &g_FontTexture);
    glBindTexture(GL_TEXTURE_2D, g_FontTexture);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, pixels);

    // Store our identifier
    io.Fonts->TexID = (void *)(intptr_t)g_FontTexture;

    // Restore state
    glBindTexture(GL_TEXTURE_2D, last_texture);

    return true;
}

bool createDeviceObjects()
{
    // Backup GL state
    GLint last_texture, last_array_buffer, last_vertex_array;
    glGetIntegerv(GL_TEXTURE_BINDING_2D, &last_texture);
    glGetIntegerv(GL_ARRAY_BUFFER_BINDING, &last_array_buffer);
    glGetIntegerv(GL_VERTEX_ARRAY_BINDING, &last_vertex_array);

    const GLchar *vertex_shader =
        "#version 330\n"
        "uniform mat4 ProjMtx;\n"
        "in vec2 Position;\n"
        "in vec2 UV;\n"
        "in vec4 Color;\n"
        "out vec2 Frag_UV;\n"
        "out vec4 Frag_Color;\n"
        "void main()\n"
        "{\n"
        "	Frag_UV = UV;\n"
        "	Frag_Color = Color;\n"
        "	gl_Position = ProjMtx * vec4(Position.xy,0,1);\n"
        "}\n";

    const GLchar* fragment_shader =
        "#version 330\n"
        "uniform sampler2D Texture;\n"
        "in vec2 Frag_UV;\n"
        "in vec4 Frag_Color;\n"
        "out vec4 Out_Color;\n"
        "void main()\n"
        "{\n"
        "	Out_Color = Frag_Color * texture( Texture, Frag_UV.st);\n"
        "}\n";

    g_ShaderHandle = glCreateProgram();
    g_VertHandle = glCreateShader(GL_VERTEX_SHADER);
    g_FragHandle = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(g_VertHandle, 1, &vertex_shader, 0);
    glShaderSource(g_FragHandle, 1, &fragment_shader, 0);
    glCompileShader(g_VertHandle);
    glCompileShader(g_FragHandle);
    glAttachShader(g_ShaderHandle, g_VertHandle);
    glAttachShader(g_ShaderHandle, g_FragHandle);
    glLinkProgram(g_ShaderHandle);

    g_AttribLocationTex      = glGetUniformLocation( g_ShaderHandle, "Texture" );
    g_AttribLocationProjMtx  = glGetUniformLocation( g_ShaderHandle, "ProjMtx" );
    g_AttribLocationPosition = glGetAttribLocation( g_ShaderHandle, "Position");
    g_AttribLocationUV       = glGetAttribLocation( g_ShaderHandle, "UV"   );
    g_AttribLocationColor    = glGetAttribLocation( g_ShaderHandle, "Color");

    glGenBuffers(1, &g_VboHandle);
    glGenBuffers(1, &g_ElementsHandle);

    glGenVertexArrays(1, &g_VaoHandle);
    glBindVertexArray(g_VaoHandle);
    glBindBuffer(GL_ARRAY_BUFFER, g_VboHandle);
    glEnableVertexAttribArray(g_AttribLocationPosition);
    glEnableVertexAttribArray(g_AttribLocationUV);
    glEnableVertexAttribArray(g_AttribLocationColor);

#define OFFSETOF(TYPE, ELEMENT) ((size_t)&(((TYPE *)0)->ELEMENT))
    glVertexAttribPointer(g_AttribLocationPosition, 2, GL_FLOAT, GL_FALSE, sizeof(ImDrawVert), (GLvoid*)OFFSETOF(ImDrawVert, pos));
    glVertexAttribPointer(g_AttribLocationUV, 2, GL_FLOAT, GL_FALSE, sizeof(ImDrawVert), (GLvoid*)OFFSETOF(ImDrawVert, uv));
    glVertexAttribPointer(g_AttribLocationColor, 4, GL_UNSIGNED_BYTE, GL_TRUE, sizeof(ImDrawVert), (GLvoid*)OFFSETOF(ImDrawVert, col));
#undef OFFSETOF

    createFontsTexture();

    // Restore modified GL state
    glBindTexture(GL_TEXTURE_2D, last_texture);
    glBindBuffer(GL_ARRAY_BUFFER, last_array_buffer);
    glBindVertexArray(last_vertex_array);

    return true;
}

void invalidateDeviceObjects()
{
    if (g_VaoHandle) glDeleteVertexArrays(1, &g_VaoHandle);
    if (g_VboHandle) glDeleteBuffers(1, &g_VboHandle);
    if (g_ElementsHandle) glDeleteBuffers(1, &g_ElementsHandle);
    g_VaoHandle = g_VboHandle = g_ElementsHandle = 0;

    glDetachShader(g_ShaderHandle, g_VertHandle);
    glDeleteShader(g_VertHandle);
    g_VertHandle = 0;

    glDetachShader(g_ShaderHandle, g_FragHandle);
    glDeleteShader(g_FragHandle);
    g_FragHandle = 0;

    glDeleteProgram(g_ShaderHandle);
    g_ShaderHandle = 0;

    if (g_FontTexture)
    {
        glDeleteTextures(1, &g_FontTexture);
        ImGui::GetIO().Fonts->TexID = 0;
        g_FontTexture = 0;
    }
}

bool init(Ra::Engine::RadiumEngine* engine)
{
    // get structures
    ImGuiIO& io = ImGui::GetIO();

    // Keyboard mapping. ImGui will use those indices to peek into the io.KeyDown[] array
    io.KeyMap[ImGuiKey_Tab]        = Ra::Core::Key_Tab;
    io.KeyMap[ImGuiKey_LeftArrow]  = Ra::Core::Key_Left;
    io.KeyMap[ImGuiKey_RightArrow] = Ra::Core::Key_Right;
    io.KeyMap[ImGuiKey_UpArrow]    = Ra::Core::Key_Up;
    io.KeyMap[ImGuiKey_DownArrow]  = Ra::Core::Key_Down;
    io.KeyMap[ImGuiKey_PageUp]     = Ra::Core::Key_PageUp;
    io.KeyMap[ImGuiKey_PageDown]   = Ra::Core::Key_PageDown;
    io.KeyMap[ImGuiKey_Home]       = Ra::Core::Key_Home;
    io.KeyMap[ImGuiKey_End]        = Ra::Core::Key_End;
    io.KeyMap[ImGuiKey_Delete]     = Ra::Core::Key_Delete;
    io.KeyMap[ImGuiKey_Backspace]  = Ra::Core::Key_BackSpace;
    io.KeyMap[ImGuiKey_Enter]      = Ra::Core::Key_Enter;
    io.KeyMap[ImGuiKey_Escape]     = Ra::Core::Key_Escape;
    io.KeyMap[ImGuiKey_A]          = Ra::Core::Key_A;
    io.KeyMap[ImGuiKey_C]          = Ra::Core::Key_C;
    io.KeyMap[ImGuiKey_V]          = Ra::Core::Key_V;
    io.KeyMap[ImGuiKey_X]          = Ra::Core::Key_X;
    io.KeyMap[ImGuiKey_Y]          = Ra::Core::Key_Y;
    io.KeyMap[ImGuiKey_Z]          = Ra::Core::Key_Z;

    io.RenderDrawListsFn = renderDrawLists;       // Alternatively you can set this to NULL and call ImGui::GetDrawData() after ImGui::Render() to get the same ImDrawData pointer.

    return true;
}

void shutdown()
{
    invalidateDeviceObjects();
    ImGui::Shutdown();
}

void newFrame(Ra::Engine::RadiumEngine* engine, int w, int h)
{
    if (!g_FontTexture)
    {
        ImGuiGL3::createDeviceObjects();
    }

    // get IO structures
    ImGuiIO& io = ImGui::GetIO();
    Ra::Engine::InputStatus& inputstts = engine->getInputStatus();

    // Setup display size (every frame to accommodate for window resizing)
    io.DisplaySize = ImVec2((float)w, (float)h);
    io.DisplayFramebufferScale = ImVec2(1, 1);

    // Setup time step
    io.DeltaTime = (float)(1.0f/60.0f);

    // Setup inputs
    // we need to grab the mouse wheel / keyboard k

    // mouse
    io.MousePos     = ImVec2(inputstts.mousePosition[0], inputstts.mousePosition[1]);
    io.MouseDown[0] = (inputstts.buttonIsPressed[Ra::Core::MouseButton_Left]   == 0) ? false : true;
    io.MouseDown[1] = (inputstts.buttonIsPressed[Ra::Core::MouseButton_Right]  == 0) ? false : true;
    io.MouseDown[2] = (inputstts.buttonIsPressed[Ra::Core::MouseButton_Middle] == 0) ? false : true;
    io.MouseWheel   =  inputstts.wheelDelta[0];
    io.MouseDrawCursor = false;   // could be enabled

    // TODO(hugo) this is an hardcoded value from Core::Key_Count (RadiumEngine.hpp)
    // if i < 104 then when '.' is pressed the key is never released
    char symbol = -1;

    // input characters
    for ( int i = 0; i < 103; ++ i )
    {
        // if the key wasn't previously pressed or was for more than 20 frames :
        if ((io.KeysDown[i] = inputstts.keyIsPressed[i])
            && ((io.KeysDownDurationPrev[i] == 0.f)
            ||  (io.KeysDownDurationPrev[i] > (20*io.DeltaTime))))
        {
            symbol = keymap(i, inputstts);
            if (symbol != -1)
                io.AddInputCharacter(symbol);
        }
    }

    // modifier keys
    io.KeyCtrl  = ((inputstts.modifiers & Ra::Core::Modifier_Ctrl)  != 0);
    io.KeyShift = ((inputstts.modifiers & Ra::Core::Modifier_Shift) != 0);
    io.KeyAlt   = ((inputstts.modifiers & Ra::Core::Modifier_Alt)   != 0);
    io.KeySuper = false;   // TODO(hugo) find the corresponding modifier

    // there is probably a great need for resetting the status of things to capture Qt events

    // Start the frame
    ImGui::NewFrame();
}

}
