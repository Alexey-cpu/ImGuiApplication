#ifndef IMGUIAPPLICATIONFILESYSTEMLAYER_H
#define IMGUIAPPLICATIONFILESYSTEMLAYER_H

// Custom
#include <ImGuiApplication.h>
#include <ImGuiApplicationLayer.h>

// ImGUI
#include <imgui.h>

// STL
#include <filesystem>
#include <string>
#include <ctime>
#include <set>
#include <map>

// ImGuiApplicationFileSystemLayer
class ImGuiApplicationFileSystemLayer : public ImGuiApplicationLayer
{
public:

    enum PressedButton
    {
        Button_None,
        Button_OK,
        Button_Cancel
    };

    // constructors
    ImGuiApplicationFileSystemLayer(
        std::filesystem::path    _RootPath = std::filesystem::current_path(),
        std::string              _Title    = "ImGuiApplicationFileSystemLayer",
        std::vector<std::string> _Formats  = std::vector<std::string>()) :
        m_Title(_Title)
    {
        // setup current path
        std::filesystem::current_path(_RootPath);

        // instantiate format filter
        for(auto format : _Formats)
        {
            if(!format.empty())
                m_FormatFilter[format] = true;
        }
    }

    // destructor
    virtual ~ImGuiApplicationFileSystemLayer(){}

    // getters
    ImGuiApplicationFileSystemLayer::PressedButton PressedButton()
    {
        return m_PressedButton;
    }

    // ImGuiApplicationLayer
    virtual void Update() override
    {
        // fill-up a format filter
        for(const auto& directoryEntry :
             std::filesystem::directory_iterator(std::filesystem::current_path().make_preferred()))
        {
            auto extention = directoryEntry.path().extension().string();

            if(!extention.empty() &&
                m_FormatFilter.find(extention) == m_FormatFilter.end())
                m_FormatFilter[extention] = false;
        }

        // create window
        ImGui::Begin(m_Title.c_str(), &m_Opened, ImGuiWindowFlags_None | ImGuiWindowFlags_NoScrollbar);

        // cd
        if(ImGui::Button("cd ../") ||
            ImGui::IsKeyPressed(ImGuiKey::ImGuiKey_Backspace))
            ChangeCurrentPath(std::filesystem::current_path().parent_path());

        // mkdir
        ImGui::SameLine();

        if(ImGui::Button("mkdir"))
        {
            std::string newFolderName = "NewFolder";

            for(int i = 0; std::filesystem::exists(newFolderName); i++)
                newFolderName = "NewFolder_" + std::to_string(i);

            std::filesystem::create_directories(newFolderName);
        }

        // rmdir
        ImGui::SameLine();

        if(ImGui::Button("rmdir") ||
            ImGui::IsKeyPressed(ImGuiKey::ImGuiKey_Delete))
        {
            for(auto selectedDirectoryEntry : m_SelectedPaths)
            {
                try
                {
                    std::filesystem::remove_all(selectedDirectoryEntry);
                }
                catch(...)
                {

                }
            }
        }

        // pwd
        ImGui::SameLine();
        ImGui::Text("Current directory: %s", std::filesystem::current_path().string().c_str());

        // catch mouse event and clear selection
        if(ImGui::IsKeyPressed(ImGuiKey::ImGuiKey_Escape))
            m_SelectedPaths.clear();

        // dirs
        if (ImGui::BeginTable(
                "Directories",
                3,
                ImGuiTableFlags_ScrollY | ImGuiTableFlags_RowBg | ImGuiTableFlags_BordersOuter | ImGuiTableFlags_BordersV | ImGuiTableFlags_Resizable | ImGuiTableFlags_Reorderable | ImGuiTableFlags_Hideable,
                ImVec2(0.0, ImGui::GetContentRegionAvail().y - 2.0 * ImGui::GetTextLineHeightWithSpacing())))
        {
            ImGui::TableSetupColumn("Name", ImGuiTableColumnFlags_::ImGuiTableColumnFlags_WidthStretch);
            ImGui::TableSetupColumn("Last write time", ImGuiTableColumnFlags_::ImGuiTableColumnFlags_WidthStretch);
            ImGui::TableSetupColumn("Type", ImGuiTableColumnFlags_::ImGuiTableColumnFlags_WidthFixed);
            ImGui::TableHeadersRow();

            // fill table
            for(const auto& directoryEntry :
                 std::filesystem::directory_iterator(std::filesystem::current_path().make_preferred()))
            {
                auto iterator = m_FormatFilter.find(directoryEntry.path().extension().string());

                if(!directoryEntry.is_directory() &&
                    (iterator == m_FormatFilter.end() || !iterator->second))
                    continue;

                ImGui::TableNextRow();

                // configure 'Name' column
                ImGui::TableSetColumnIndex(0);
                ImGui::TextUnformatted(directoryEntry.path().filename().string().c_str());

                // configure 'Last write time' column
                ImGui::TableSetColumnIndex(1);

                auto tp = std::filesystem::last_write_time(directoryEntry.path());
                time_t secs = std::chrono::duration_cast<std::chrono::seconds>(tp.time_since_epoch()).count();
                ImGui::TextUnformatted(asctime(std::localtime(&secs)));

                // configure 'Type' column
                ImGui::TableSetColumnIndex(2);
                ImGui::PushID(directoryEntry.path().string().c_str());
                ImGui::PushStyleColor(
                    ImGuiCol_::ImGuiCol_Text,
                    directoryEntry.is_directory() ? IM_COL32(128, 128, 128, 255) : IM_COL32(255, 255, 255, 255));

                if(ImGui::Selectable(
                        (directoryEntry.is_directory() ? "Directory" : "File"),
                        m_SelectedPaths.find(directoryEntry.path()) != m_SelectedPaths.end(),
                        ImGuiSelectableFlags_::ImGuiSelectableFlags_AllowDoubleClick | ImGuiSelectableFlags_SpanAllColumns))
                {
                    if(directoryEntry.is_directory() &&
                        (ImGui::IsMouseDoubleClicked(ImGuiMouseButton_::ImGuiMouseButton_Left) || ImGui::IsKeyPressed(ImGuiKey::ImGuiKey_Enter)))
                        ChangeCurrentPath(directoryEntry.path());

                    if(ImGui::IsKeyDown(ImGuiKey::ImGuiKey_LeftCtrl))
                    {
                        m_SelectedPaths.insert(directoryEntry.path());
                    }
                    else
                    {
                        m_SelectedPaths.clear();
                        m_SelectedPaths.insert(directoryEntry.path());
                    }
                }

                ImGui::PopID();
                ImGui::PopStyleColor();
            }

            ImGui::EndTable();
        }

        // create dialog buttons
        ImGui::BeginChild(
            (m_Title + "_Buttons").c_str(),
            ImVec2(0.0, 2.0 * ImGui::GetTextLineHeightWithSpacing()),
            ImGuiChildFlags_::ImGuiChildFlags_Borders,
            ImGuiWindowFlags_::ImGuiWindowFlags_None | ImGuiWindowFlags_NoScrollbar);

        {
            // create dialog buttons
            std::string preview = std::string();

            for(auto& supportedFormats : m_FormatFilter)
            {
                if(!supportedFormats.first.empty() && supportedFormats.second)
                    preview += supportedFormats.first + " ";
            }

            if(preview.empty())
                preview = "none";

            if(ImGui::BeginCombo("Format filter", preview.c_str()))
            {
                for(auto& supportedFormats : m_FormatFilter)
                {
                    if(supportedFormats.first.empty())
                        continue;

                    ImGui::Checkbox(
                        supportedFormats.first.c_str(),
                        &supportedFormats.second);
                }

                ImGui::EndCombo();
            }

            // compute dialog buttons geometry
            {
                ImGuiStyle& style = ImGui::GetStyle();
                float spacing = 8.0;
                float size    = ImGui::CalcTextSize("Cancel").x + ImGui::CalcTextSize("Ok").x + style.FramePadding.x * 2.0f + 2.0 * spacing;
                float avail   = ImGui::GetContentRegionAvail().x;
                ImGui::SetCursorPosX(ImGui::GetCursorPosX() + (avail - size) * 2.0);

                ImGui::SameLine(0, spacing);

                if(ImGui::Button("Cancel"))
                    m_PressedButton = ImGuiApplicationFileSystemLayer::PressedButton::Button_Cancel;

                ImGui::SameLine(0, spacing);

                if(ImGui::Button("Ok"))
                    m_PressedButton = ImGuiApplicationFileSystemLayer::PressedButton::Button_OK;
            }
        }

        ImGui::EndChild();

        ImGui::End();
    }

protected:

    // info
    std::set<std::filesystem::path> m_SelectedPaths;
    std::map<std::string, bool> m_FormatFilter = std::map<std::string, bool>();
    std::string m_Title;

    enum ImGuiApplicationFileSystemLayer::PressedButton m_PressedButton =
        ImGuiApplicationFileSystemLayer::PressedButton::Button_None;

    void ChangeCurrentPath(std::filesystem::path _Path)
    {
        if(!std::filesystem::exists(_Path))
            return;

        // clear selection
        m_SelectedPaths.clear();

        // change current path
        std::filesystem::current_path(_Path);
    }
};

#endif // IMGUIAPPLICATIONFILESYSTEMLAYER_H
