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

#include <iostream>

class ImGuiApplicationFileSystemLayer : public ImGuiApplicationLayer
{
public:

    // constructors
    ImGuiApplicationFileSystemLayer(std::filesystem::path _RootPath, std::string _Title = "ImGuiApplicationFileSystemLayer") : m_Title(_Title)
    {
        std::filesystem::current_path(_RootPath);
    }

    // destructor
    virtual ~ImGuiApplicationFileSystemLayer()
    {
        std::cout << "~ImGuiApplicationFileSystemLayer() \n";
    }

    // ImGuiApplicationLayer
    virtual void Update() override
    {
        ImGui::Begin(m_Title.c_str(), &m_Opened, ImGuiWindowFlags_None);

        std::cout << "m_Opened " << m_Opened << "\n";

        if(ImGui::IsMouseClicked(ImGuiMouseButton_::ImGuiMouseButton_Left) &&
            !ImGui::IsKeyDown(ImGuiKey::ImGuiKey_LeftCtrl))
            m_SelectedPaths.clear();

        // cd
        if(ImGui::Button("cd ../") ||
            ImGui::IsKeyPressed(ImGuiKey::ImGuiKey_Backspace))
        {
            if(std::filesystem::exists(std::filesystem::current_path().parent_path()))
                std::filesystem::current_path(std::filesystem::current_path().parent_path());
        }

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

        if(ImGui::Button("rmdir") || ImGui::IsKeyPressed(ImGuiKey::ImGuiKey_Delete))
        {
            for(auto selectedDirectoryEntry : m_SelectedPaths)
                std::filesystem::remove_all(selectedDirectoryEntry);
        }

        // pwd
        ImGui::SameLine();
        ImGui::Text("Current directory: %s", std::filesystem::current_path().string().c_str());

        // dirs
        if (ImGui::BeginTable(
                "Directories",
                3,
                ImGuiTableFlags_ScrollY | ImGuiTableFlags_RowBg | ImGuiTableFlags_BordersOuter | ImGuiTableFlags_BordersV | ImGuiTableFlags_Resizable | ImGuiTableFlags_Reorderable | ImGuiTableFlags_Hideable,
                ImVec2(0.0, ImGui::GetContentRegionAvail().y - ImGui::GetTextLineHeightWithSpacing())))
        {
            ImGui::TableSetupColumn("Name", ImGuiTableColumnFlags_::ImGuiTableColumnFlags_WidthStretch);
            ImGui::TableSetupColumn("Last write time", ImGuiTableColumnFlags_::ImGuiTableColumnFlags_WidthStretch);
            ImGui::TableSetupColumn("Type", ImGuiTableColumnFlags_::ImGuiTableColumnFlags_WidthFixed);
            ImGui::TableHeadersRow();

            // fill table
            for(const auto& directoryEntry :
                 std::filesystem::directory_iterator(std::filesystem::current_path().make_preferred()))
            {
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
                        std::filesystem::current_path(directoryEntry.path());

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

        ImGui::End();
    }

protected:

    // info
    std::set<std::filesystem::path> m_SelectedPaths;
    std::string m_Title;
};

#endif // IMGUIAPPLICATIONFILESYSTEMLAYER_H
