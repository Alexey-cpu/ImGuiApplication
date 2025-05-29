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
    ImGuiDemoLayer(std::string _Name = "ImGuiDemoLayer") : ImGuiApplicationLayer(_Name)
    {
        //m_FileSystemWatcher = ImGuiApplication::Instance()->Push<ImGuiApplicationFileSystemWatcher>();
    }

    // virtual destructor
    virtual ~ImGuiDemoLayer(){}

    // ImGuiApplicationLayer
    virtual void OnUpdate() override
    {
        ImGuiIO& io = ImGui::GetIO();

        // 1. Show the big demo window (Most of the sample code is in ImGui::ShowDemoWindow()! You can browse its code to learn more about Dear ImGui!).
        if (show_demo_window)
            ImGui::ShowDemoWindow(&show_demo_window);

        // 2. Show a simple window that we create ourselves. We use a Begin/End pair to create a named window.
        {
            static float f = 0.0f;
            static int counter = 0;

            ImGui::Begin("Hello, world!");                                // Create a window called "Hello, world!" and append into it.

            ImGui::Text("This is some useful text.");                     // Display some text (you can use a format strings too)
            ImGui::Checkbox("Demo Window", &show_demo_window);            // Edit bools storing our window open/close state
            ImGui::Checkbox("Another Window", &show_another_window);

            ImGui::SliderFloat("float", &f, 0.0f, 1.0f);                  // Edit 1 float using a slider from 0.0f to 1.0f
            ImGui::ColorEdit3("clear color", (float*)&clear_color);       // Edit 3 floats representing a color

            if (ImGui::Button("Button"))                                  // Buttons return true when clicked (most widgets return true when edited/activated)
                counter++;
            ImGui::SameLine();
            ImGui::Text("counter = %d", counter);

            ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / io.Framerate, io.Framerate);
            ImGui::End();
        }

        // 3. Show another simple window.
        if (show_another_window)
        {
            ImGui::Begin("Another Window", &show_another_window);         // Pass a pointer to our bool variable (the window will have a closing button that will clear the bool when clicked)
            ImGui::Text("Hello from another window!");
            if (ImGui::Button("Close Me"))
                show_another_window = false;
            ImGui::End();
        }
    }

protected:

    //std::shared_ptr<ImGuiApplicationFileSystemWatcher> m_FileSystemWatcher;

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

    (void)ImGuiApplication::Instance()->Push<ImGuiApplicationFileSystemDialogLayer>(
        std::filesystem::current_path(),
        "FileDialog",
        std::vector<std::string>({".hpp", ".cpp", ".txt", ".cmake", ".user"}));

    //(void)ImGuiApplication::Instance()->Push<ImGuiDemoLayer>("ImGuiDemoLayer");

    (void)ImGuiApplication::Instance()->Push<ImGuiApplicationStyleSettingsLayer>("C:\\SDK\\Qt_Projects\\ImGuiRenderExplore\\shared");

    // ImGuiApplicationStyleLayer

    return ImGuiApplication::Instance()->
        setTitle("ImGuiApplication")->
        setIniFileLocation(std::filesystem::current_path())->
        setLogFileLocation(std::filesystem::current_path())->
        setBackgroundColor(ImVec4(64.0 / 255.0, 64.0 / 255.0, 64.0 / 255.0, 1.0))->
        Maximize()->
        Execute();

    return 1;
}
