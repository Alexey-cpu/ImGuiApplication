#include <ImGuiApplication.h>
#include <ImGuiApplicationFileSystemLayer.h>

// ImGuiDemoLayer
class ImGuiDemoLayer : public ImGuiApplicationLayer
{
public:

    // constructors
    ImGuiDemoLayer(){}

    // virtual destructor
    virtual ~ImGuiDemoLayer(){}

    // ImGuiApplicationLayer
    virtual void Update() override
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

    bool show_another_window = false;
    bool show_demo_window    = false;

    ImVec4 clear_color = ImVec4(0.45f, 0.55f, 0.60f, 1.00f);
};

class ImGuiApplicationDemoLayer : public ImGuiApplicationLayer
{
public:

    // constructors
    ImGuiApplicationDemoLayer(){}

    // virtual destructor
    virtual ~ImGuiApplicationDemoLayer(){}

    // ImGuiApplicationLayer
    virtual void Update() override
    {
        /*
        // open Dialog Simple
        if (ImGui::Button("Open File Dialog"))
            ImGuiFileDialog::Instance()->OpenDialog("ChooseFileDlgKey", "Choose File", ".cpp,.h,.hpp");

        // display
        if (ImGuiFileDialog::Instance()->Display("ChooseFileDlgKey"))
        {
            // action if OK
            if (ImGuiFileDialog::Instance()->IsOk())
            {
                std::string filePathName = ImGuiFileDialog::Instance()->GetFilePathName();
                std::string filePath = ImGuiFileDialog::Instance()->GetCurrentPath();

                // action
            }

            // close
            ImGuiFileDialog::Instance()->Close();
        }
        */
    }

    void ShowWindow1()
    {
        ImGui::Begin("Window-1", &m_Open, ImGuiWindowFlags_::ImGuiWindowFlags_MenuBar);

        // create menubar
        if(ImGui::BeginMenuBar())
        {
            if(ImGui::BeginMenu("File", true))
            {
                if(ImGui::MenuItem("New  (Ctrl + N)            ")){}
                if(ImGui::MenuItem("Open (Ctrl + O)            ")){};
                if(ImGui::MenuItem("Save (Ctrl + S)            ")){};
                if(ImGui::MenuItem("Save as (Ctrl + Shift + O) ")){};
                if(ImGui::MenuItem("Close (Ctrl + Q)           ")){};

                ImGui::EndMenu();
            }

            ImGui::EndMenuBar();
        }

        ImGui::End();
    }

    void ShowWindow2()
    {
        ImGui::Begin("Window-2", &m_Open, ImGuiWindowFlags_::ImGuiWindowFlags_MenuBar);

        // create menubar
        if(ImGui::BeginMenuBar())
        {
            if(ImGui::BeginMenu("File", true))
            {
                if(ImGui::MenuItem("New (Ctrl + N)")){}
                if(ImGui::MenuItem("Open (Ctrl + O)")){};
                if(ImGui::MenuItem("Save (Ctrl + S)")){};
                if(ImGui::MenuItem("Save as (Ctrl + Shift + O)")){};
                if(ImGui::MenuItem("Close (Ctrl + Q)")){};

                ImGui::EndMenu();
            }

            ImGui::EndMenuBar();
        }

        ImGui::End();
    }

protected:

    bool m_Open = true;
};

int main(int, char**)
{
    //(void)ImGuiApplication::Instance()->Push<ImGuiDemoLayer>();

    (void)ImGuiApplication::Instance()->Push<ImGuiApplicationFileSystemLayer>(
        std::filesystem::current_path(),
        "FileDialog",
        std::vector<std::string>({".hpp", ".cpp", ".txt", ".cmake", ".user"}));

    (void)ImGuiApplication::Instance()->Push<ImGuiDemoLayer>();

    return ImGuiApplication::Instance()->
        setTitle("ImGuiApplication")->
        setIniFileLocation(std::filesystem::current_path())->
        setLogFileLocation(std::filesystem::current_path())->
        setBackgroundColor(ImVec4(64.0 / 255.0, 64.0 / 255.0, 64.0 / 255.0, 1.0))->
        Maximize()->
        Execute();
}
