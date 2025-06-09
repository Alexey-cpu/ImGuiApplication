#include <ImGuiApplication.h>
#include <FunctionalBlockExecutionEnvironment.h>
#include <ImGuiApplicationFileSystemDialog.h>
#include <ImGuiApplicationSettingsFonts.h>
#include <ImGuiApplicationSettingsColors.h>
#include <ImGuiApplicationDialog.h>
#include <ImGuiApplicationSettings.h>
#include <ImGuiApplicationScene2DCanvas.h>

#include <FactoryObjectRect.h>

#include <filesystem>
#include <iostream>

#include <imgui_impl_glfw.h>

// ImGuiDemoLayer
class ImGuiDemoLayer : public ImGuiApplication::Layer
{
public:

    // constructors
    ImGuiDemoLayer() :
        Layer("ImGuiDemoLayer"){}

    // virtual destructor
    virtual ~ImGuiDemoLayer(){}

    // ImGuiApplicationLayer
    virtual void OnUpdate() override
    {
        ImGuiIO& io = ImGui::GetIO();

        if (show_demo_window)
            ImGui::ShowDemoWindow(&show_demo_window);

        {
            static float f = 0.0f;
            static int counter = 0;

            ImGui::Begin("Hello, world!");

            ImGui::Text("This is some useful text.");
            ImGui::Checkbox("Demo Window", &show_demo_window);
            ImGui::Checkbox("Another Window", &show_another_window);

            ImGui::SliderFloat("float", &f, 0.0f, 1.0f);
            ImGui::ColorEdit3("clear color", (float*)&clear_color);

            if (ImGui::Button("Button"))
                counter++;
            ImGui::SameLine();
            ImGui::Text("counter = %d", counter);

            ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / io.Framerate, io.Framerate);
            ImGui::End();
        }

        if (show_another_window)
        {
            ImGui::Begin("Another Window", &show_another_window);
            ImGui::Text("Hello from another window!");
            if (ImGui::Button("Close Me"))
                show_another_window = false;
            ImGui::End();
        }
    }

protected:

    bool show_another_window = false;
    bool show_demo_window    = false;

    ImVec4 clear_color = ImVec4(0.45f, 0.55f, 0.60f, 1.00f);
};

// ImGuiDemoLayer
class FBDLayer : public ImGuiApplication::Layer
{
public:

    // constructors
    FBDLayer() : Layer("FBDLayer")
    {
        // create block
        // {
        //     auto parent = new FunctionalBlock(m_Environment.get(), "SomeBlock-1", std::string());
        //     parent->set_world_rect(ImRect(ImVec2(512.f, 512.f), ImVec2(512.f, 512.f) + ImVec2(128.f, 128.f)));
        //     //parent->set_geometry(FactoryObjectHierarchy::Geometry(ImVec2(32.f, 32.f), ImVec2(128.f, 128.f)));

        //     // add ports
        //     for(int i = 0; i < 4; i++)
        //         parent->add_input<double>("In-" + std::to_string(i))->set_orientation(FunctionalBlockPort::Orientation::Horizontal);

        //     for(int i = 0; i < 2; i++)
        //         parent->add_output<double>("O-" + std::to_string(i))->set_orientation(FunctionalBlockPort::Orientation::Horizontal);
        // }

        // // create block
        // {
        //     auto parent = new FunctionalBlock(m_Environment.get(), "SomeBlock-2", std::string());
        //     parent->set_world_rect(ImRect(ImVec2(128.f, 128.f), ImVec2(128, 128.f) + ImVec2(128.f, 128.f)));

        //     // add ports
        //     for(int i = 0; i < 4; i++)
        //         parent->add_input<double>("In-" + std::to_string(i))->set_orientation(FunctionalBlockPort::Orientation::Horizontal);

        //     for(int i = 0; i < 2; i++)
        //         parent->add_output<double>("O-" + std::to_string(i))->set_orientation(FunctionalBlockPort::Orientation::Horizontal);
        // }

        // ImVec2 init = ImVec2(0.f, 0.f);

        // int k  = 0;
        // int N  = 1e5;
        // int dN = std::max(0.01f * (float)N, 1.f);

        // for(int i =0; i < N; i++)
        // {
        //     for(int j = 0; j < dN; j++)
        //     {
        //         auto item = new FunctionalBlock(m_Environment.get(), "SomeBlock-1");

        //         item->set_rect(ImRect(init, init + ImVec2(128.f, 128.f)));

        //         // add ports
        //         for(int i = 0; i < 4; i++)
        //             item->add_input<double>("In-" + std::to_string(i));

        //         for(int i = 0; i < 2; i++)
        //             item->add_output<double>("O-" + std::to_string(i));

        //         init.x = (j + 1) * 256;

        //         i++;
        //     }

        //     init.y = (k + 1) * 256;
        //     init.x = 0.f;

        //     k++;
        // }
    }

    // virtual destructor
    virtual ~FBDLayer(){}

    // ImGuiApplicationLayer
    virtual void OnUpdate() override
    {
        FactoryObjectRect r1(
            glm::vec2(512.f, 512.f), 
            glm::vec2(512.f, 512.f) + glm::vec2(512.f, 256.f), 
            glm::vec2(0.0f, 0.0f));

        FactoryObjectRect r2(
            glm::vec2(0.f, 0.f), 
            glm::vec2(0.f, 0.f) + glm::vec2(256.f, 256.f), 
            glm::vec2(0.5f, 0.5f), 
            &r1);

        FactoryObjectRect r3(
            glm::vec2(0.f, 0.f), 
            glm::vec2(0.f, 0.f) + glm::vec2(128.f, 64.f), 
            glm::vec2(0.5f, 0.5f), 
            &r2);

        FactoryObjectRect r4(
            glm::vec2(0.f, 0.f), 
            glm::vec2(0.f, 0.f) + glm::vec2(63.f, 32.f), 
            glm::vec2(0.5f, 0.5f), 
            &r2);

        r2.rotate((float)glfwGetTime(), glm::vec3(0.f, 0.f, 1.f));

        r1.render(IM_COL32(255, 0, 0, 255));
        r2.render(IM_COL32(0, 255, 0, 255));
        r3.render(IM_COL32(0, 0, 255, 255));
        r4.render(IM_COL32(0, 255, 255, 255));
    }

protected:

    std::shared_ptr<FunctionalBlockExecutionEnvironment> m_Environment =
        std::shared_ptr<FunctionalBlockExecutionEnvironment>(new FunctionalBlockExecutionEnvironment());
};


int main(int, char**)
{

#ifdef __APPLE__
    setlocale(LC_NUMERIC,"C");
#else
    std::setlocale(LC_NUMERIC,"C");
#endif

    //ImGuiApplication::Application::Instance()->Push<ImGuiApplication::Dialogs::FileSystemDialog>(
    //    std::filesystem::current_path(),
    //    "FileDialog",
    //    std::vector<std::string>({".hpp", ".cpp", ".txt", ".cmake", ".user"}));

    //(void)ImGuiApplication::Application::Instance()->Push<ImGuiDemoLayer>();
    //(void)ImGuiApplication::Application::Instance()->Push<ImGuiApplication::Scene2D::Scene>();

    (void)ImGuiApplication::Application::Instance()->Push<FBDLayer>();


    /*
    std::cout << std::filesystem::path(std::filesystem::current_path()) << "\n";
    std::cout << std::filesystem::path(std::filesystem::current_path().parent_path().wstring().append(L"/shared")).make_preferred() << "\n";

    (void)ImGuiApplication::Application::Instance()->
        Push<ImGuiApplication::Settings::Settings>(
        "Settings",
        std::filesystem::path(std::filesystem::current_path().parent_path().parent_path().wstring().append(L"/shared")).make_preferred(),
        std::list<std::shared_ptr<ImGuiApplication::Layer>>(
            {
                std::shared_ptr<ImGuiApplication::Settings::Fonts>(new ImGuiApplication::Settings::Fonts()),
                std::shared_ptr<ImGuiApplication::Settings::Colors>(new ImGuiApplication::Settings::Colors()),
            }
        )
    );
    */

    return ImGuiApplication::Application::Instance()->
        setTitle("ImGuiApplication")->
        setIniFileLocation(std::filesystem::current_path())->
        setLogFileLocation(std::filesystem::current_path())->
        setBackgroundColor(ImVec4(64.0 / 255.0, 64.0 / 255.0, 64.0 / 255.0, 1.0))->
        Maximize()->
        Execute();

    return 1;
}
