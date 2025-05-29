#include <ImGuiApplicationFileSystemDialogLayer.h>

#include <iostream>

// ImGuiApplicationFileSystemPathsRenamerDialogLayer
ImGuiApplicationFileSystemPathsRenamerDialogLayer::ImGuiApplicationFileSystemPathsRenamerDialogLayer(
    const std::vector<ImGuiApplicationFileSystemPathItem>& _Items) :
    ImGuiApplicationDialogLayer("ImGuiApplicationFileSystemPathsRenamerPopupLayer"),
    m_Items(_Items){}

ImGuiApplicationFileSystemPathsRenamerDialogLayer::~ImGuiApplicationFileSystemPathsRenamerDialogLayer(){}

void ImGuiApplicationFileSystemPathsRenamerDialogLayer::DrawContent()
{
    if (ImGui::BeginTable(
            "RenameFilesAndDirectoriesList",
            2,
            ImGuiTableFlags_::ImGuiTableFlags_RowBg        |
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
            if(!std::filesystem::exists(item.m_Path))
                continue;

            ImGui::TableNextRow();

            // configure 'Type' column
            ImGui::TableSetColumnIndex(0);
            ImGui::PushID(item.m_Path.c_str());
            ImGui::PushStyleColor(
                ImGuiCol_::ImGuiCol_Text,
                item.isDirectory() ? IM_COL32(128, 128, 128, 255) : IM_COL32(255, 255, 255, 255));

            if(ImGui::Selectable(
                    (item.isDirectory() ? "Directory" : "File"),
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
            ImGui::InputText("", &m_Items[index].m_FileNameBuffer[0], m_Items[index].m_FileNameBuffer.size(), ImGuiInputTextFlags_::ImGuiInputTextFlags_ElideLeft);
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
            if(!std::filesystem::exists(item.m_Path))
                continue;

            std::filesystem::rename(
                item.m_Path,
                item.m_Path.parent_path().wstring()
                    .append(L"/")
                    .append(pugi::as_wide(item.m_FileNameBuffer)));
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
    ImGuiApplicationDialogLayer(_Title)
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
        OnGoUpperAction();

    // -->
    ImGui::SameLine();
    if(ImGui::Button("-->"))
        OnGoDeeperAction();

    // mkdir
    ImGui::SameLine();
    if(ImGui::Button("mkdir"))
        OnCreateFolderAction();

    ImGui::SetItemTooltip("Create folder");

    // rmdir
    ImGui::SameLine();
    if(ImGui::Button("rmdir"))
        OnRemoveFilesOrFoldersAction();

    ImGui::SetItemTooltip("Remove file/folder");

    // mv
    ImGui::SameLine();
    if(ImGui::Button("mv"))
        OnRenameFilesOrFoldersAction();

    ImGui::SetItemTooltip("Rename selected files/folders");

    // cp
    ImGui::SameLine();
    if(ImGui::Button("cp"))
        OnCopyFilesOrFoldersAction();

    ImGui::SetItemTooltip("Copy selected files/folders");

    // paste
    ImGui::SameLine();
    if(ImGui::Button("paste"))
        OnPasteFilesOrFoldersAction();

    ImGui::SetItemTooltip("Paste copied files/folders");

    // pwd
    m_CurrentFolder = ImGuiApplicationFileSystemPathItem(std::filesystem::current_path());

    if(!m_CurrentFolder.m_PathBuffer.empty())
    {
        ImGui::PushID("CurrentDirectory");

        ImGui::SetNextItemWidth(ImGui::GetContentRegionAvail().x);

        if(ImGui::InputText("",
                             &m_CurrentFolder.m_PathBuffer[0],
                             m_CurrentFolder.m_PathBuffer.size(),
                             ImGuiInputTextFlags_::ImGuiInputTextFlags_EnterReturnsTrue | ImGuiInputTextFlags_ElideLeft))
        {
            // push current path onto stack
            m_VisitedPathsStack.push(std::filesystem::current_path());

            // go to directory
            ChangeCurrentPath(std::filesystem::path(m_CurrentFolder.m_PathBuffer));
        }

        ImGui::PopID();
    }
    else
    {

    }

    // file
    m_NewFolder = m_CurrentFile;

    if(!m_NewFolder.m_FileNameBuffer.empty())
    {
        ImGui::PushID("NewFolder");

        ImGui::SetNextItemWidth(ImGui::GetContentRegionAvail().x);

        if(ImGui::InputText("",
                             &m_NewFolder.m_FileNameBuffer[0],
                             m_NewFolder.m_FileNameBuffer.size(),
                             ImGuiInputTextFlags_::ImGuiInputTextFlags_EnterReturnsTrue | ImGuiInputTextFlags_ElideLeft))
        {
        }

        ImGui::PopID();
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
        for(const auto& directory :
             std::filesystem::directory_iterator(std::filesystem::current_path().make_preferred()))
        {
            // apply a format filter
            auto iterator = m_FormatFilter.find(directory.path().extension().string());

            if(!directory.is_directory() &&
                (iterator == m_FormatFilter.end() || !iterator->second))
                continue;

            ImGuiApplicationFileSystemPathItem path(directory.path());

            ImGui::TableNextRow();

            // configure 'Name' column
            ImGui::TableSetColumnIndex(0);
            ImGui::TextUnformatted(path.m_FileNameBuffer.c_str());

            // configure 'Last write time' column
            ImGui::TableSetColumnIndex(1);

            auto tp = std::filesystem::last_write_time(path.m_Path);
            time_t secs = std::chrono::duration_cast<std::chrono::seconds>(tp.time_since_epoch()).count();
            ImGui::TextUnformatted(asctime(std::localtime(&secs)));

            // configure 'Type' column
            ImGui::TableSetColumnIndex(2);
            ImGui::PushID(pugi::as_utf8(path.m_Path.wstring()).c_str());
            ImGui::PushStyleColor(
                ImGuiCol_::ImGuiCol_Text,
                path.isDirectory() ? IM_COL32(128, 128, 128, 255) : IM_COL32(255, 255, 255, 255));

            if(ImGui::Selectable(
                    (path.isDirectory() ? "Directory" : "File"),
                    std::find(m_SelectedPaths.begin(), m_SelectedPaths.end(), path) != m_SelectedPaths.end(),
                    ImGuiSelectableFlags_::ImGuiSelectableFlags_AllowDoubleClick   |
                        ImGuiSelectableFlags_::ImGuiSelectableFlags_SpanAllColumns |
                        ImGuiSelectableFlags_::ImGuiSelectableFlags_AllowOverlap))
            {
                if(path.isDirectory() &&
                    (ImGui::IsMouseDoubleClicked(ImGuiMouseButton_::ImGuiMouseButton_Left) || ImGui::IsKeyPressed(ImGuiKey::ImGuiKey_Enter)))
                {
                    ChangeCurrentPath(path.m_Path);
                }

                if(ImGui::IsKeyDown(ImGuiKey::ImGuiKey_LeftCtrl))
                {
                    m_SelectedPaths.push_back(path);
                }
                else
                {
                    m_SelectedPaths.clear();
                    m_SelectedPaths.push_back(path);
                }

                DrawContextMenu();

                m_CurrentFile = ImGuiApplicationFileSystemPathItem(path.m_Path);
            }

            // Popup context menu
            DrawContextMenu();

            ImGui::SetItemTooltip("Select and then right click onto an item to call context popup menu");

            ImGui::PopID();
            ImGui::PopStyleColor();
        }

        ImGui::EndTable();
    }

    // Keys
    if(ImGui::IsKeyPressed(ImGuiKey::ImGuiKey_Delete)) // delete
        OnRemoveFilesOrFoldersAction();

    if(ImGui::IsKeyDown(ImGuiKey::ImGuiKey_LeftCtrl) &&
        ImGui::IsKeyPressed(ImGuiKey::ImGuiKey_A)) // select all
    {
        m_SelectedPaths.clear();

        for(const auto& directory :
             std::filesystem::directory_iterator(std::filesystem::current_path().make_preferred()))
        {
            // apply a format filter
            auto iterator = m_FormatFilter.find(directory.path().extension().string());

            if(!directory.is_directory() &&
                (iterator == m_FormatFilter.end() || !iterator->second))
                continue;

            m_SelectedPaths.push_back(directory.path());
        }
    }

    if(ImGui::IsKeyPressed(ImGuiKey::ImGuiKey_Escape)) // deselect all
        m_SelectedPaths.clear();

    if(ImGui::IsKeyDown(ImGuiKey::ImGuiKey_LeftCtrl) &&
        ImGui::IsKeyPressed(ImGuiKey::ImGuiKey_C)) // copy
    {
        OnCopyFilesOrFoldersAction();
    }

    if(ImGui::IsKeyDown(ImGuiKey::ImGuiKey_LeftCtrl) &&
        ImGui::IsKeyPressed(ImGuiKey::ImGuiKey_V)) // Paste
    {
        OnPasteFilesOrFoldersAction();
    }

    // create format filter
    ImGui::TextUnformatted((m_CurrentFile.isDirectory() ? "Directory selected" : m_CurrentFile.m_PathBuffer.c_str()));

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
            m_FormatFilter[extention] = true;
    }
}

void ImGuiApplicationFileSystemDialogLayer::DrawContextMenu()
{
    if(m_SelectedPaths.empty())
        return;

    // Popup context menu
    if (ImGui::BeginPopupContextItem())
    {
        if(ImGui::MenuItem("Create"))
            OnCreateFolderAction();

        if(ImGui::MenuItem("Rename"))
            OnRenameFilesOrFoldersAction();

        if(ImGui::MenuItem("Copy"))
            OnCopyFilesOrFoldersAction();

        if(ImGui::MenuItem("Paste"))
            OnPasteFilesOrFoldersAction();

        if(ImGui::MenuItem("Remove"))
            OnRemoveFilesOrFoldersAction();

        ImGui::EndPopup();
    }
}

void ImGuiApplicationFileSystemDialogLayer::OnGoUpperAction()
{
    // push current path onto stack
    m_VisitedPathsStack.push(std::filesystem::current_path());

    // go higher
    ChangeCurrentPath(std::filesystem::current_path().parent_path());
}

void ImGuiApplicationFileSystemDialogLayer::OnGoDeeperAction()
{
    if(m_VisitedPathsStack.empty())
        return;

    // go deeper
    ChangeCurrentPath(m_VisitedPathsStack.top());

    // pop path from the stack
    if(!m_VisitedPathsStack.empty())
        m_VisitedPathsStack.pop();
}

void ImGuiApplicationFileSystemDialogLayer::OnCreateFolderAction()
{
    std::wstring newFolderName;

    auto wide = pugi::as_wide(m_NewFolder.m_FileNameBuffer);

    for(size_t i = 0; i < wide.size(); i++)
    {
        if(wide[i] != '\0')
            newFolderName.push_back(wide[i]);
    }

    if(newFolderName.empty())
        newFolderName = L"NewFolder";


    for(int i = 0; std::filesystem::exists(newFolderName); i++)
        newFolderName = newFolderName + L"_copy";

    try
    {
        std::filesystem::create_directories(std::filesystem::path(newFolderName));
    }
    catch(...)
    {

    }

    m_NewFolder.m_FileNameBuffer = pugi::as_utf8(newFolderName);
    m_NewFolder.m_FileNameBuffer.push_back('\0');
}

void ImGuiApplicationFileSystemDialogLayer::OnRemoveFilesOrFoldersAction()
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

void ImGuiApplicationFileSystemDialogLayer::OnRenameFilesOrFoldersAction()
{
    Push<ImGuiApplicationFileSystemPathsRenamerDialogLayer>(m_SelectedPaths);
}

void ImGuiApplicationFileSystemDialogLayer::OnCopyFilesOrFoldersAction()
{
    m_FilesToCopy = m_SelectedPaths;
}

void ImGuiApplicationFileSystemDialogLayer::OnPasteFilesOrFoldersAction()
{
    struct FileInfo
    {
        std::wstring name      = std::wstring();
        std::string  extention = std::string();
    };

    auto getFileInfo = [](std::filesystem::path path, int _DotsCount = 2)->FileInfo
    {
        std::string extention = std::string();

        for(int i = 0; i < _DotsCount; i++)
        {
            extention = extention.append(path.extension().string());
            path = path.stem();
        }

        return
        {
            path.filename().wstring(),
            extention

        };
    };

    for(auto& fileToCopy : m_FilesToCopy)
    {
        auto fileInfo = getFileInfo(fileToCopy.m_Path);

        auto source = fileToCopy.m_Path;

        auto target = std::filesystem::path(fileToCopy.m_Path.parent_path().wstring()
                                                .append(L"/")
                                                .append(fileInfo.name)
                                                .append(pugi::as_wide(fileInfo.extention))).make_preferred();

        while(std::filesystem::exists(target))
        {
            target = std::filesystem::path(fileToCopy.m_Path.parent_path().wstring()
                                               .append(L"/")
                                               .append(fileInfo.name.append(L"_Copy"))
                                               .append(pugi::as_wide(fileInfo.extention))).make_preferred();
        }

        std::filesystem::copy(source, target);
    }
}
