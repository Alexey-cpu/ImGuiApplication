#ifndef IMGUIAPPLICATION_H
#define IMGUIAPPLICATION_H

// Custom
#include <ImGuiApplicationLayer.h>

// ImGUI
#include <imgui.h>
#define GL_SILENCE_DEPRECATION
#if defined(IMGUI_IMPL_OPENGL_ES2)
#include <GLES2/gl2.h>
#endif
#define GLFW_INCLUDE_NONE
#include <glad/glad.h>
#include <GLFW/glfw3.h> // Will drag system OpenGL headers

// STL
#include <string>
#include <list>
#include <filesystem>

// ImGuiApplication
class ImGuiApplication
{
public:

    static ImGuiApplication* Instance()
    {
        static ImGuiApplication instance;
        return &instance;
    }

    ImGuiApplication(const ImGuiApplication &) = delete;
    ImGuiApplication & operator = (const ImGuiApplication &) = delete;

    // setters
    ImGuiApplication* setTitle(std::string _Title);
    ImGuiApplication* setSize(ImVec2 _Size);
    ImGuiApplication* setBackgroundColor(ImVec4 _BackgroundColor);
    ImGuiApplication* setConfigFlags(ImGuiConfigFlags _ConfigFlags);
    ImGuiApplication* setIniFileLocation(std::filesystem::path _Path);
    ImGuiApplication* setLogFileLocation(std::filesystem::path _Path);

    ImGuiApplication* Maximize();
    int Execute();

    template<typename __type, typename ... __parameters>
    std::shared_ptr<__type> Push(__parameters... _Parameters)
    {
        // create layer
        auto layer = std::make_shared<__type>(_Parameters ...);

        // push layer into rendering queue
        m_RenderingQueue.push_back(layer);

        // return previously create layer
        return layer;
    }

    void Pop(std::shared_ptr<ImGuiApplicationLayer> _Layer)
    {
        // find layer
        auto iterator =
            std::find(
            m_RenderingQueue.begin(),
            m_RenderingQueue.end(),
            _Layer);

        // remove layer
        if(iterator != m_RenderingQueue.end())
            m_RenderingQueue.erase(iterator);
    }

private:

    // info
    GLFWwindow*      m_MainWindow                = nullptr;
    std::string      m_MainWindowTitle           = "ImGuiApplication";
    ImVec2           m_MainWindowSize            = ImVec2(1280, 720);
    ImVec4           m_MainWindowBackgroundColor = ImVec4(0.0, 0.0, 0.0, 0.0);
    ImGuiConfigFlags m_MainWindowConfigFlags     =
        ImGuiConfigFlags_::ImGuiConfigFlags_NavEnableKeyboard |
        ImGuiConfigFlags_::ImGuiConfigFlags_NavEnableGamepad  |
        ImGuiConfigFlags_::ImGuiConfigFlags_DockingEnable     |
        ImGuiConfigFlags_::ImGuiConfigFlags_ViewportsEnable;

    std::list<std::shared_ptr<ImGuiApplicationLayer>> m_RenderingQueue;

    std::string m_IniFileLocation = std::filesystem::current_path().string();
    std::string m_LogFileLocation = std::filesystem::current_path().string();

    // constructors
    ImGuiApplication();

    // destructor
    ~ImGuiApplication();
};

#endif // IMGUIAPPLICATION_H
