#include <ImGuiApplication.h>

#include <imgui_impl_glfw.h>
#include <imgui_impl_opengl3.h>

// GLFW helpers
static void glfwErrorCallback(int _Error, const char* _Eescription)
{
    fprintf(stderr, "GLFW Error %d: %s\n", _Error, _Eescription);
}

// ImGuiApplication
ImGuiApplication* ImGuiApplication::setTitle(std::string _Title)
{
    m_MainWindowTitle = _Title;
    return Instance();
}

ImGuiApplication* ImGuiApplication::setSize(ImVec2 _Size)
{
    m_MainWindowSize = _Size;
    return Instance();
}

ImGuiApplication* ImGuiApplication::setBackgroundColor(ImVec4 _BackgroundColor)
{
    m_MainWindowBackgroundColor = _BackgroundColor;
    return Instance();
}

ImGuiApplication* ImGuiApplication::setConfigFlags(ImGuiConfigFlags _ConfigFlags)
{
    m_MainWindowConfigFlags = _ConfigFlags;
    return Instance();
}

ImGuiApplication* ImGuiApplication::setIniFileLocation(std::filesystem::path _Path)
{
    m_IniFileLocation = std::filesystem::path(_Path.string() + "/" + "imgui.ini").make_preferred().string();
    return Instance();
}

ImGuiApplication* ImGuiApplication::setLogFileLocation(std::filesystem::path _Path)
{
    m_LogFileLocation = std::filesystem::path(_Path.string() + "/" + "imgui_log.txt").make_preferred().string();
    return Instance();
}

ImGuiApplication* ImGuiApplication::Maximize()
{
    if(Instance()->m_MainWindow != nullptr)
        glfwMaximizeWindow(Instance()->m_MainWindow);

    return Instance();
}

int ImGuiApplication::Execute()
{
    // Check if the window has been instantiated
    if(m_MainWindow == nullptr)
        return 0;

    for(auto it = m_RenderingQueue.begin(); it != m_RenderingQueue.end(); it++)
        (*it)->Start();

    //auto& io = ImGui::GetIO();
    //auto font = io.Fonts->AddFontFromFileTTF("C:\\Windows\\Fonts\\Tahoma.ttf", 24);

    //auto font = io.Fonts->AddFontFromFileTTF("C:\\Windows\\Fonts\\Verdana.ttf", 24, NULL, io.Fonts->GetGlyphRangesCyrillic());

    //io.Fonts->Build();

    //io.Fonts->AddCustomRectFontGlyph(io.Fonts->GetGlyphRangesCyrillic());

    //io.Fonts->AddFontFromMemoryTTF(&inter, sizeof inter, 16 * dpi_scale, NULL, io.Fonts->GetGlyphRangesCyrillic());

    //io.GetGlyphRangesCyrillic()

    //ImGui::Text("Hello"); // use the default font (which is the first loaded font)
    //ImGui::PushFont(font);
    //ImGui::Text("Hello with another font");
    //ImGui::PopFont();

    // Process main loop
    while (!glfwWindowShouldClose(m_MainWindow))
    {
        // update .ini and .log files locations before starting a rendering frame
        ImGuiIO& io = ImGui::GetIO();
        io.IniFilename = m_IniFileLocation.c_str();
        io.LogFilename = m_LogFileLocation.c_str();

        // Poll events
        glfwPollEvents();

        if (glfwGetWindowAttrib(m_MainWindow, GLFW_ICONIFIED) != 0)
        {
            ImGui_ImplGlfw_Sleep(1);
            continue;
        }

        // Start the Dear ImGui frame
        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();

        // setup dockspace over the whole viewport
        ImGui::DockSpaceOverViewport(0, ImGui::GetMainViewport());

        // render children
        //ImGui::PushFont(font);

        for(auto it = m_RenderingQueue.begin(); it != m_RenderingQueue.end(); it++)
        {
            // remove closed layer
            if((*it)->isClosed())
            {
                auto rm = it;
                it++;
                m_RenderingQueue.erase(rm);

                // stop if there's nothing to render
                if(it == m_RenderingQueue.end())
                    break;
            }

            // begin render next child layer
            (*it)->BeforeUpdate();
        }

        for(auto it = m_RenderingQueue.begin(); it != m_RenderingQueue.end(); it++)
            (*it)->Render();

        for(auto it = m_RenderingQueue.begin(); it != m_RenderingQueue.end(); it++)
            (*it)->AfterUpdate();

        //ImGui::PopFont();

        // Render contents
        ImGui::Render();
        int display_w, display_h;
        glfwGetFramebufferSize(m_MainWindow, &display_w, &display_h);
        glViewport(0, 0, display_w, display_h);

        glClearColor(
            m_MainWindowBackgroundColor.x * m_MainWindowBackgroundColor.w,
            m_MainWindowBackgroundColor.y * m_MainWindowBackgroundColor.w,
            m_MainWindowBackgroundColor.z * m_MainWindowBackgroundColor.w,
            m_MainWindowBackgroundColor.w);

        glClear(GL_COLOR_BUFFER_BIT);
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

        // Update and Render additional Platform Windows
        if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
        {
            GLFWwindow* backup_current_context = glfwGetCurrentContext();
            ImGui::UpdatePlatformWindows();
            ImGui::RenderPlatformWindowsDefault();
            glfwMakeContextCurrent(backup_current_context);
            gladLoadGL();
        }

        // swap buffers
        glfwSwapBuffers(m_MainWindow);
    }

    return 1;
}

ImGuiApplication::ImGuiApplication()
{
    // initialization
    glfwSetErrorCallback(glfwErrorCallback);

    if(!glfwInit())
        return;

// Decide GL+GLSL versions
#if defined(IMGUI_IMPL_OPENGL_ES2)
    // GL ES 2.0 + GLSL 100 (WebGL 1.0)
    const char* glsl_version = "#version 100";
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 2);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 0);
    glfwWindowHint(GLFW_CLIENT_API, GLFW_OPENGL_ES_API);
#elif defined(IMGUI_IMPL_OPENGL_ES3)
    // GL ES 3.0 + GLSL 300 es (WebGL 2.0)
    const char* glsl_version = "#version 300 es";
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 0);
    glfwWindowHint(GLFW_CLIENT_API, GLFW_OPENGL_ES_API);
#elif defined(__APPLE__)
    // GL 3.2 + GLSL 150
    const char* glsl_version = "#version 150";
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 2);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);  // 3.2+ only
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);            // Required on Mac
#else
    // GL 3.0 + GLSL 130
    const char* glsl_version = "#version 130";
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 0);
    //glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);  // 3.2+ only
    //glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);            // 3.0+ only
#endif

    // Create window with graphics context
    //glfwWindowHint(GLFW_VERSION_MAJOR, 3);
    //glfwWindowHint(GLFW_VERSION_MINOR, 3);
    //glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    //glfwWindowHint(GLFW_RESIZABLE, 1);
    //glfwWindowHint(GLFW_DECORATED, GLFW_FALSE);
    //glfwWindowHint(GLFW_TRANSPARENT_FRAMEBUFFER, 1);

    m_MainWindow = glfwCreateWindow(
        m_MainWindowSize.x,
        m_MainWindowSize.y,
        m_MainWindowTitle.c_str(),
        nullptr,
        nullptr
        );

    if(m_MainWindow == nullptr)
        return;

    glfwMakeContextCurrent(m_MainWindow);
    gladLoadGL();
    glfwSwapInterval(1); // Enable vsync

    // Setup Dear ImGui context
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO();
    io.ConfigFlags |= m_MainWindowConfigFlags;

    // Setup Dear ImGui style
    ImGui::StyleColorsDark();
    //ImGui::StyleColorsLight();

    // When viewports are enabled we tweak WindowRounding/WindowBg so platform windows can look identical to regular ones.
    ImGuiStyle& style = ImGui::GetStyle();
    if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
    {
        style.WindowRounding = 0.0f;
        style.Colors[ImGuiCol_WindowBg].w = 1.0f;
    }

    // Setup Platform/Renderer backends
    ImGui_ImplGlfw_InitForOpenGL(m_MainWindow, true);
    ImGui_ImplOpenGL3_Init(glsl_version);
}

ImGuiApplication::~ImGuiApplication()
{
    // Cleanup GLFW/OpenGL
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();
    glfwDestroyWindow(m_MainWindow);
    glfwTerminate();

    // Cleanup rendering queue
    m_RenderingQueue.clear();
}
