#include <ImGuiApplication.h>
#include <ImGuiApplicationFileSystemDialog.h>
#include <ImGuiApplicationSettingsFonts.h>
#include <ImGuiApplicationSettingsColors.h>
#include <ImGuiApplicationDialog.h>
#include <ImGuiApplicationSettings.h>
#include <ImGuiApplicationScene2DCanvas.h>

#include <filesystem>
#include <iostream>

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

    (void)ImGuiApplication::Application::Instance()->Push<ImGuiApplication::Scene2D::Scene>();


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

    return ImGuiApplication::Application::Instance()->
        setTitle("ImGuiApplication")->
        setIniFileLocation(std::filesystem::current_path())->
        setLogFileLocation(std::filesystem::current_path())->
        setBackgroundColor(ImVec4(64.0 / 255.0, 64.0 / 255.0, 64.0 / 255.0, 1.0))->
        Maximize()->
        Execute();

    return 1;
}
