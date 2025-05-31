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
#include <filesystem>

namespace ImGuiApplication
{
    // Application
    class Application : public Layer
    {
    public:

        static Application* Instance()
        {
            static Application instance;
            return &instance;
        }

        Application(const Application &) = delete;
        Application & operator = (const Application &) = delete;

        // setters
        Application* setTitle(std::string _Title);
        Application* setSize(ImVec2 _Size);
        Application* setBackgroundColor(ImVec4 _BackgroundColor);
        Application* setConfigFlags(ImGuiConfigFlags _ConfigFlags);
        Application* setIniFileLocation(std::filesystem::path _Path);
        Application* setLogFileLocation(std::filesystem::path _Path);

        // API
        Application* Maximize();
        int Execute();

        // virtual functions to override
        virtual void OnClose() override;
        virtual void OnAwake() override;
        virtual void OnStart() override;
        virtual void OnUpdate() override;
        virtual void OnFinish() override;

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

        std::string m_IniFileLocation = std::filesystem::current_path().string();
        std::string m_LogFileLocation = std::filesystem::current_path().string();

        // constructors
        Application();

        // destructor
        ~Application();
    };
};

#endif // IMGUIAPPLICATION_H
