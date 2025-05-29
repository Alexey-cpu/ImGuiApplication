#include <ImGuiApplication.h>
#include <ImGuiApplicationFileSystemDialogLayer.h>
#include <ImGuiApplicationFileSystemWatcher.h>
#include <ImGuiApplicationDialogLayer.h>
#include <ImGuiApplicationStyleSettingsLayer.h>

#include <filesystem>

// ImGuiDemoLayer
class ImGuiDemoLayer : public ImGuiApplicationLayer
{
public:

    // constructors
    ImGuiDemoLayer() :
        ImGuiApplicationLayer("ImGuiDemoLayer"){}

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

    ImGuiApplication::Instance()->Push<ImGuiApplicationFileSystemDialogLayer>(
        std::filesystem::current_path(),
        "FileDialog",
        std::vector<std::string>({".hpp", ".cpp", ".txt", ".cmake", ".user"}));

    //(void)ImGuiApplication::Instance()->Push<ImGuiDemoLayer>("ImGuiDemoLayer");

    (void)ImGuiApplication::Instance()->Push<ImGuiApplicationStyleSettingsLayer>(
        std::filesystem::path(std::filesystem::current_path().parent_path().parent_path().string().append("/shared/")).make_preferred());

    return ImGuiApplication::Instance()->
        setTitle("ImGuiApplication")->
        setIniFileLocation(std::filesystem::current_path())->
        setLogFileLocation(std::filesystem::current_path())->
        setBackgroundColor(ImVec4(64.0 / 255.0, 64.0 / 255.0, 64.0 / 255.0, 1.0))->
        Maximize()->
        Execute();

    return 1;
}
