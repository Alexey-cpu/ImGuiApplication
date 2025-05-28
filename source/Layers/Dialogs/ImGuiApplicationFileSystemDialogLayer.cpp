#include <ImGuiApplicationFileSystemDialogLayer.h>

// ImGuiApplicationFileSystemPathsRenamerDialogLayer
ImGuiApplicationFileSystemPathsRenamerDialogLayer::ImGuiApplicationFileSystemPathsRenamerDialogLayer(const std::vector<ImGuiApplicationFileSystemPathItem>& m_Items) :
    ImGuiApplicationDialogLayer("ImGuiApplicationFileSystemPathsRenamerPopupLayer"),
    m_Items(m_Items){}

ImGuiApplicationFileSystemPathsRenamerDialogLayer::~ImGuiApplicationFileSystemPathsRenamerDialogLayer(){}

void ImGuiApplicationFileSystemPathsRenamerDialogLayer::DrawContent()
{
    if (ImGui::BeginTable(
            "RenameFilesAndDirectoriesList",
            2,
            ImGuiTableFlags_::ImGuiTableFlags_RowBg            |
                ImGuiTableFlags_::ImGuiTableFlags_BordersOuter |
                ImGuiTableFlags_::ImGuiTableFlags_BordersV     |
                ImGuiTableFlags_::ImGuiTableFlags_Resizable    |
                ImGuiTableFlags_::ImGuiTableFlags_Reorderable  |
                ImGuiTableFlags_::ImGuiTableFlags_Hideable,
            ImVec2(0.0, ImGui::GetContentRegionAvail().y - 2.0 * ImGui::GetTextLineHeightWithSpacing())))
    {
        ImGui::TableSetupColumn("Type", ImGuiTableColumnFlags_::ImGuiTableColumnFlags_WidthStretch);
        ImGui::TableSetupColumn("Name", ImGuiTableColumnFlags_::ImGuiTableColumnFlags_WidthStretch);
        ImGui::TableHeadersRow();

        // fill table
        int index  = 0;
        for(auto& item : m_Items)
        {
            item.m_FolderNameBuffer.push_back('\0');

            ImGui::TableNextRow();

            // configure 'Type' column
            ImGui::TableSetColumnIndex(0);
            ImGui::PushID(item.m_Path.c_str());
            ImGui::PushStyleColor(
                ImGuiCol_::ImGuiCol_Text,
                item.m_IsDirectory ? IM_COL32(128, 128, 128, 255) : IM_COL32(255, 255, 255, 255));

            if(ImGui::Selectable(
                    (item.m_IsDirectory ? "Directory" : "File"),
                    false,
                    ImGuiSelectableFlags_::ImGuiSelectableFlags_SpanAllColumns      |
                        ImGuiSelectableFlags_::ImGuiSelectableFlags_DontClosePopups |
                        ImGuiSelectableFlags_::ImGuiSelectableFlags_AllowItemOverlap)){}

            ImGui::PopID();
            ImGui::PopStyleColor();

            // configure 'Name' column
            ImGui::TableSetColumnIndex(1);
            ImGui::PushItemWidth(ImGui::GetContentRegionAvail().x);
            ImGui::PushID(item.m_Path.c_str());
            ImGui::InputText("", &m_Items[index].m_FolderNameBuffer[0], m_Items[index].m_FolderNameBuffer.size(), ImGuiInputTextFlags_::ImGuiInputTextFlags_ElideLeft);
            ImGui::PopID();
            index++;
        }

        ImGui::EndTable();
    }
}

void ImGuiApplicationFileSystemPathsRenamerDialogLayer::DrawButtons()
{
    ImGui::SameLine(0, m_ButtonsSpacing);

    if(ImGui::Button("Cancel"))
    {
        // setup state
        m_DialogState = ImGuiApplicationDialogState::Canceled;

        // hide window
        Close();
    }

    ImGui::SameLine(0, m_ButtonsSpacing);

    if(ImGui::Button("Ok"))
    {
        // rename files
        for(auto& item : m_Items)
        {
            std::filesystem::rename(
                item.m_Path,
#if defined(_WIN32) | defined(_WIN64)
                std::filesystem::path(item.m_Path.parent_path().string() + "/" + utf2cp(item.m_FolderNameBuffer)).make_preferred());
#else
                std::filesystem::path(item.m_Path.parent_path().string() + "/" + item.m_FolderNameBuffer).make_preferred());
#endif
        }

        // setup state
        m_DialogState = ImGuiApplicationDialogState::Accepted;

        // close window
        Close();
    }
}

// ImGuiApplicationFileSystemDialogLayer
ImGuiApplicationFileSystemDialogLayer::ImGuiApplicationFileSystemDialogLayer(
    const std::filesystem::path&    _RootPath,
    const std::string&              _Title,
    const std::vector<std::string>& _Formats) :
    ImGuiApplicationDialogLayer(_Title),
    m_Title(_Title)
{
    // setup current path
    if(std::filesystem::exists(_RootPath))
        std::filesystem::current_path(_RootPath);

    // setup a format filter
    SetupFormatFilter(_Formats);
}

ImGuiApplicationFileSystemDialogLayer::~ImGuiApplicationFileSystemDialogLayer(){}

void ImGuiApplicationFileSystemDialogLayer::DrawContent()
{
    // <--
    ImGui::SameLine();
    if(ImGui::Button("<--"))
        OnBackwardButtonClicked();

    // -->
    ImGui::SameLine();
    if(ImGui::Button("-->"))
        OnForwardButtonClicked();

    // mkdir
    ImGui::SameLine();
    if(ImGui::Button("mkdir"))
        OnMakeDirectioryButtonClicked();

    // rmdir
    ImGui::SameLine();
    if(ImGui::Button("rmdir") ||
        ImGui::IsKeyPressed(ImGuiKey::ImGuiKey_Delete))
        OnRemoveDirectoryButtonClicked();

    // mv
    ImGui::SameLine();
    if(ImGui::Button("mv"))
        OnRenameDirectoryButtonClicked();

    // pwd
    ImGui::SameLine();
    m_CurrentDirectory = ImGuiApplicationFileSystemPathItem(std::filesystem::current_path());
    if(!m_CurrentDirectory.m_DirectoryBuffer.empty())
    {
        ImGui::PushID("CurrentDirectory");

        if(ImGui::InputText("",
                             &m_CurrentDirectory.m_DirectoryBuffer[0],
                             m_CurrentDirectory.m_DirectoryBuffer.size(),
                             ImGuiInputTextFlags_::ImGuiInputTextFlags_EnterReturnsTrue | ImGuiInputTextFlags_ElideLeft))
            ChangeCurrentPath(std::filesystem::path(m_CurrentDirectory.m_DirectoryBuffer));

        ImGui::PopID();
    }
    else
    {

    }

    // catch mouse event and clear selection
    if(ImGui::IsKeyPressed(ImGuiKey::ImGuiKey_Escape))
        m_SelectedPaths.clear();

    // apply format filter
    std::vector<ImGuiApplicationFileSystemPathItem> filteredItems;

    for(const auto& directoryEntry :
         std::filesystem::directory_iterator(std::filesystem::current_path().make_preferred()))
    {
        auto iterator = m_FormatFilter.find(directoryEntry.path().extension().string());

        if(!directoryEntry.is_directory() &&
            (iterator == m_FormatFilter.end() || !iterator->second))
            continue;

        filteredItems.push_back(ImGuiApplicationFileSystemPathItem(directoryEntry.path()));
    }

    // draw table
    if (ImGui::BeginTable(
            "Directories",
            3,
            ImGuiTableFlags_ScrollY | ImGuiTableFlags_RowBg | ImGuiTableFlags_BordersOuter | ImGuiTableFlags_BordersV | ImGuiTableFlags_Resizable | ImGuiTableFlags_Reorderable | ImGuiTableFlags_Hideable,
            ImVec2(0.0, ImGui::GetContentRegionAvail().y - 4.0 * ImGui::GetTextLineHeightWithSpacing())))
    {
        ImGui::TableSetupColumn("Name", ImGuiTableColumnFlags_::ImGuiTableColumnFlags_WidthStretch);
        ImGui::TableSetupColumn("Last write time", ImGuiTableColumnFlags_::ImGuiTableColumnFlags_WidthStretch);
        ImGui::TableSetupColumn("Type", ImGuiTableColumnFlags_::ImGuiTableColumnFlags_WidthFixed);
        ImGui::TableHeadersRow();

        // fill table
        for(const auto& directoryEntry : filteredItems)
        {
            ImGui::TableNextRow();

            // configure 'Name' column
            ImGui::TableSetColumnIndex(0);
            ImGui::TextUnformatted(directoryEntry.m_FolderNameBuffer.c_str());

            // configure 'Last write time' column
            ImGui::TableSetColumnIndex(1);

            auto tp = std::filesystem::last_write_time(directoryEntry.m_Path);
            time_t secs = std::chrono::duration_cast<std::chrono::seconds>(tp.time_since_epoch()).count();
            ImGui::TextUnformatted(asctime(std::localtime(&secs)));

            // configure 'Type' column
            ImGui::TableSetColumnIndex(2);
            ImGui::PushID(directoryEntry.m_Path.string().c_str());
            ImGui::PushStyleColor(
                ImGuiCol_::ImGuiCol_Text,
                directoryEntry.m_IsDirectory ? IM_COL32(128, 128, 128, 255) : IM_COL32(255, 255, 255, 255));

            if(ImGui::Selectable(
                    (directoryEntry.m_IsDirectory ? "Directory" : "File"),
                    std::find(m_SelectedPaths.begin(), m_SelectedPaths.end(), directoryEntry) != m_SelectedPaths.end(),
                    ImGuiSelectableFlags_::ImGuiSelectableFlags_AllowDoubleClick |
                        ImGuiSelectableFlags_::ImGuiSelectableFlags_SpanAllColumns |
                        ImGuiSelectableFlags_::ImGuiSelectableFlags_AllowOverlap))
            {
                if(directoryEntry.m_IsDirectory &&
                    (ImGui::IsMouseDoubleClicked(ImGuiMouseButton_::ImGuiMouseButton_Left) || ImGui::IsKeyPressed(ImGuiKey::ImGuiKey_Enter)))
                    ChangeCurrentPath(directoryEntry.m_Path);

                if(ImGui::IsKeyDown(ImGuiKey::ImGuiKey_LeftCtrl))
                {
                    m_SelectedPaths.push_back(directoryEntry);
                }
                else
                {
                    m_SelectedPaths.clear();
                    m_SelectedPaths.push_back(directoryEntry);
                }


                m_CurrentFile =
                    directoryEntry.m_IsDirectory ?
                        ImGuiApplicationFileSystemPathItem() :
                            ImGuiApplicationFileSystemPathItem(directoryEntry.m_Path);
            }

            if (ImGui::BeginPopupContextItem()) // <-- use last item id as popup id
            {
                //selected = n;
                //ImGui::Text("This a popup for \"%s\"!", names[n]);
                if (ImGui::Button("Close"))
                    ImGui::CloseCurrentPopup();
                ImGui::EndPopup();
            }
            ImGui::SetItemTooltip("Right-click to open popup");

            ImGui::PopID();
            ImGui::PopStyleColor();
        }

        ImGui::EndTable();
    }

    // create format filter
    ImGui::TextUnformatted((m_CurrentFile.m_FileNameBuffer.empty() ? "Directory selected" : m_CurrentFile.m_DirectoryBuffer.c_str()));

    std::string preview = std::string();

    for(auto& supportedFormats : m_FormatFilter)
    {
        if(!supportedFormats.first.empty() && supportedFormats.second)
            preview += supportedFormats.first + " ";
    }

    if(preview.empty())
        preview = "none";

    if(ImGui::Button("All"))
    {
        for(auto& supportedFormats : m_FormatFilter)
        {
            if(!supportedFormats.first.empty())
                supportedFormats.second = true;
        }
    }

    ImGui::SameLine();

    if(ImGui::Button("None"))
    {
        for(auto& supportedFormats : m_FormatFilter)
        {
            if(!supportedFormats.first.empty())
                supportedFormats.second = false;
        }
    }

    ImGui::SameLine();

    if(ImGui::BeginCombo("FormatFilter", preview.c_str()))
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
}

void ImGuiApplicationFileSystemDialogLayer::DrawButtons()
{
    ImGui::SameLine(0, m_ButtonsSpacing);

    if(ImGui::Button("Cancel"))
    {
        // setup state
        m_DialogState = ImGuiApplicationDialogState::Canceled;

        // close window
        m_Opened  = false;
    }

    ImGui::SameLine(0, m_ButtonsSpacing);

    if(ImGui::Button("Ok"))
    {
        // setup state
        m_DialogState = ImGuiApplicationDialogState::Accepted;

        // close window
        m_Opened = false;
    }
}

void ImGuiApplicationFileSystemDialogLayer::ChangeCurrentPath(std::filesystem::path _Path)
{
    if(!std::filesystem::exists(_Path))
        return;

    // clear selection
    m_SelectedPaths.clear();

    // change current path
    std::filesystem::current_path(_Path);

    // setup format filter
    SetupFormatFilter();
}

void ImGuiApplicationFileSystemDialogLayer::SetupFormatFilter(const std::vector<std::string>& _Formats)
{
    // fill format filter by user-defined input
    for(auto& format : _Formats)
    {
        if(!format.empty())
            m_FormatFilter[format] = true;
    }

    // fill format filter by current directory
    for(const auto& directoryEntry :
         std::filesystem::directory_iterator(std::filesystem::current_path().make_preferred()))
    {
        auto extention = directoryEntry.path().extension().string();

        if(!extention.empty() &&
            m_FormatFilter.find(extention) == m_FormatFilter.end())
            m_FormatFilter[extention] = false;
    }
}

void ImGuiApplicationFileSystemDialogLayer::OnBackwardButtonClicked()
{
    // push current path onto stack
    m_PathStack.push(std::filesystem::current_path());

    // go higher
    ChangeCurrentPath(std::filesystem::current_path().parent_path());
}

void ImGuiApplicationFileSystemDialogLayer::OnForwardButtonClicked()
{
    if(m_PathStack.empty())
        return;

    // go deeper
    ChangeCurrentPath(m_PathStack.top());

    // pop path from the stack
    if(!m_PathStack.empty())
        m_PathStack.pop();
}

void ImGuiApplicationFileSystemDialogLayer::OnMakeDirectioryButtonClicked()
{
    std::string newFolderName = "NewFolder";

    for(int i = 0; std::filesystem::exists(newFolderName); i++)
        newFolderName = "NewFolder_" + std::to_string(i);

    std::filesystem::create_directories(newFolderName);
}

void ImGuiApplicationFileSystemDialogLayer::OnRemoveDirectoryButtonClicked()
{
    for(auto& selectedDirectoryEntry : m_SelectedPaths)
    {
        try
        {
            std::filesystem::remove_all(selectedDirectoryEntry.m_Path);
        }
        catch(...)
        {

        }
    }
}

void ImGuiApplicationFileSystemDialogLayer::OnRenameDirectoryButtonClicked()
{
    Push<ImGuiApplicationFileSystemPathsRenamerDialogLayer>(m_SelectedPaths);
}
