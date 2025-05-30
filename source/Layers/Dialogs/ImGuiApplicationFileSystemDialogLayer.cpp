#include <ImGuiApplicationFileSystemDialogLayer.h>

#include <set>
#include <iostream>

// ImGuiApplicationFileSystemPathsRenamerDialogLayer
ImGuiApplicationFileSystemPathsRenamerDialogLayer::ImGuiApplicationFileSystemPathsRenamerDialogLayer(
    const std::vector<std::filesystem::path>& _Items) :
    ImGuiApplicationDialogLayer("ImGuiApplicationFileSystemPathsRenamerPopupLayer")
{
    for(auto& item : _Items)
    {
        auto filename = pugi::as_utf8(item.filename().wstring());
        filename.push_back('\0');
        m_Paths.push_back(Path(item, filename));
    }
}

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
        for(auto& path : m_Paths)
        {
            if(!std::filesystem::exists(path.path))
                continue;

            ImGui::TableNextRow();

            // configure 'Type' column
            bool isDirectory = std::filesystem::exists(path.path);

            ImGui::TableSetColumnIndex(0);
            ImGui::PushID(path.path.c_str());
            ImGui::PushStyleColor(
                ImGuiCol_::ImGuiCol_Text,
                isDirectory ? IM_COL32(128, 128, 128, 255) : IM_COL32(255, 255, 255, 255));

            if(ImGui::Selectable(
                    isDirectory ? "Directory" : "File",
                    true,
                    ImGuiSelectableFlags_::ImGuiSelectableFlags_SpanAllColumns      |
                        ImGuiSelectableFlags_::ImGuiSelectableFlags_DontClosePopups |
                        ImGuiSelectableFlags_::ImGuiSelectableFlags_AllowItemOverlap)){}

            ImGui::PopID();
            ImGui::PopStyleColor();

            // configure 'Name' column
            ImGui::TableSetColumnIndex(1);
            ImGui::PushItemWidth(ImGui::GetContentRegionAvail().x);
            ImGui::PushID("FinemaneEditor");
            ImGui::InputText(
                std::string("").c_str(),
                &path.filename[0],
                path.filename.size(),
                ImGuiInputTextFlags_::ImGuiInputTextFlags_ElideLeft);
            ImGui::PopID();
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
        for(auto& item : m_Paths)
        {
            if(!std::filesystem::exists(item.path))
                continue;

            std::filesystem::rename(
                item.path,
                item.path.parent_path().wstring()
                    .append(L"/")
                    .append(pugi::as_wide(item.filename)));
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
    auto windowFlags =
        ImGuiWindowFlags_::ImGuiWindowFlags_None | ImGuiWindowFlags_::ImGuiWindowFlags_NoScrollbar;

    if(ImGui::BeginTable(
            "FileSystemDialogSplitter",
            2,
            ImGuiTableFlags_ScrollY | ImGuiTableFlags_RowBg | ImGuiTableFlags_BordersOuter | ImGuiTableFlags_BordersV | ImGuiTableFlags_Resizable | ImGuiTableFlags_Reorderable | ImGuiTableFlags_Hideable))
    {
        ImGui::TableSetupColumn("Tree", ImGuiTableColumnFlags_::ImGuiTableColumnFlags_WidthStretch | ImGuiTableColumnFlags_::ImGuiTableColumnFlags_PreferSortAscending);
        ImGui::TableSetupColumn("Browser", ImGuiTableColumnFlags_::ImGuiTableColumnFlags_WidthFixed | ImGuiTableColumnFlags_::ImGuiTableColumnFlags_PreferSortAscending);
        //ImGui::TableHeadersRow();

        ImGui::TableNextRow();

        // configure 'Name' column
        ImGui::TableSetColumnIndex(0);
       DrawPathsTree(std::filesystem::current_path().root_path());

        // configure 'Last write time' column
        ImGui::TableSetColumnIndex(1);
        DrawBrowser();

        ImGui::EndTable();
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

bool ImGuiApplicationFileSystemDialogLayer::ChangeCurrentPath(std::filesystem::path _Path)
{
    if(!std::filesystem::exists(_Path))
        return false;

    // change current path
    try
    {
        std::filesystem::current_path(_Path);
    }
    catch(...)
    {
        return false;
    }

    // clear selection
    m_SelectedPaths.clear();

    // setup format filter
    SetupFormatFilter();

    return true;
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
         std::filesystem::directory_iterator(std::filesystem::current_path().make_preferred(), std::filesystem::directory_options::skip_permission_denied))
    {
        auto extention = directoryEntry.path().extension().string();

        if(!extention.empty() &&
            m_FormatFilter.find(extention) == m_FormatFilter.end())
            m_FormatFilter[extention] = true;
    }
}

void ImGuiApplicationFileSystemDialogLayer::DrawContextMenu()
{
    if(Contains<ImGuiApplicationFileSystemPathsRenamerDialogLayer>())
        return;

    if(m_SelectedPaths.empty())
    {
        if (ImGui::BeginPopupContextItem())
        {
            if(ImGui::MenuItem("CreateFolder"))
                OnCreateFolderAction();

            if(ImGui::MenuItem("Paste"))
                OnPasteFilesOrFoldersAction();

            ImGui::EndPopup();
        }

        return;
    }

    // Popup context menu
    if (ImGui::BeginPopupContextItem())
    {
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

void ImGuiApplicationFileSystemDialogLayer::DrawFormatFilter()
{
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

void ImGuiApplicationFileSystemDialogLayer::DrawBrowser()
{
    // <--
    if(ImGui::Button("<--"))
        OnLevelUpAction();

    // -->
    ImGui::SameLine();
    if(ImGui::Button("-->"))
        OnLevelDownAction();

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
    m_CurrentFolder = pugi::as_utf8(std::filesystem::current_path().wstring());
    size_t size = m_CurrentFolder.size();
    for(size_t i = 0; i < std::max<size_t>(size, 512); i++)
        m_CurrentFolder.push_back('\0');

    if(!m_CurrentFolder.empty())
    {
        ImGui::PushID("CurrentDirectory");
        ImGui::SetNextItemWidth(ImGui::GetContentRegionAvail().x);

        if(ImGui::InputText(
                std::string("").c_str(),
                &m_CurrentFolder[0],
                m_CurrentFolder.size(),
                ImGuiInputTextFlags_::ImGuiInputTextFlags_EnterReturnsTrue | ImGuiInputTextFlags_::ImGuiInputTextFlags_ElideLeft))
        {
            m_VisitedPathsStack.push(std::filesystem::current_path());
            ChangeCurrentPath(std::filesystem::path(m_CurrentFolder));
        }

        ImGui::PopID();
    }

    // file
    if(m_NewFolder.empty())
        m_NewFolder = pugi::as_utf8(std::filesystem::current_path().filename().wstring());
    m_NewFolder.push_back('\0');

    ImGui::PushID("NewFolder");
    ImGui::SetNextItemWidth(ImGui::GetContentRegionAvail().x);
    ImGui::InputText(
        std::string("").c_str(),
        &m_NewFolder[0],
        m_NewFolder.size(),
        ImGuiInputTextFlags_::ImGuiInputTextFlags_EnterReturnsTrue | ImGuiInputTextFlags_::ImGuiInputTextFlags_ElideLeft);
    ImGui::PopID();

    // draw table
    if (ImGui::BeginTable(
            "CurrentDirectoryContents",
            3,
            ImGuiTableFlags_ScrollY | ImGuiTableFlags_RowBg | ImGuiTableFlags_BordersOuter | ImGuiTableFlags_BordersV | ImGuiTableFlags_Resizable | ImGuiTableFlags_Reorderable | ImGuiTableFlags_Hideable,
            ImVec2(0.0, ImGui::GetContentRegionAvail().y - 4.0 * ImGui::GetTextLineHeightWithSpacing())))
    {
        ImGui::TableSetupColumn("Name", ImGuiTableColumnFlags_::ImGuiTableColumnFlags_WidthStretch | ImGuiTableColumnFlags_::ImGuiTableColumnFlags_PreferSortAscending);
        ImGui::TableSetupColumn("Last write time", ImGuiTableColumnFlags_::ImGuiTableColumnFlags_WidthStretch);
        ImGui::TableSetupColumn("Type", ImGuiTableColumnFlags_::ImGuiTableColumnFlags_WidthFixed | ImGuiTableColumnFlags_::ImGuiTableColumnFlags_PreferSortAscending);
        ImGui::TableHeadersRow();

        // draw paths in alphabetical order
        try
        {
        }
        catch(...)
        {

        }

        std::set<std::filesystem::path> paths;

        for(const auto& directory :
             std::filesystem::directory_iterator(std::filesystem::current_path().make_preferred(), std::filesystem::directory_options::skip_permission_denied))
        {
            // apply a format filter
            auto iterator = m_FormatFilter.find(directory.path().extension().string());

            if(!directory.is_directory() &&
                (iterator == m_FormatFilter.end() || !iterator->second))
                continue;

            paths.insert(directory.path());
        }

        for(const auto& path : paths)
        {
            // apply a format filter
            auto iterator = m_FormatFilter.find(path.extension().string());

            if(!std::filesystem::is_directory(path) &&
                (iterator == m_FormatFilter.end() || !iterator->second))
                continue;

            ImGui::TableNextRow();

            // configure 'Name' column
            ImGui::TableSetColumnIndex(0);
            ImGui::TextUnformatted(pugi::as_utf8(path.filename().wstring()).c_str());

            // configure 'Last write time' column
            ImGui::TableSetColumnIndex(1);

            try
            {
                auto time    = std::filesystem::last_write_time(path);
                auto seconds = std::chrono::duration_cast<std::chrono::seconds>(time.time_since_epoch()).count();
                ImGui::TextUnformatted(asctime(std::localtime(&seconds)));
            }
            catch(...)
            {
            }

            // configure 'Type' column
            bool isDirectory = std::filesystem::is_directory(path);

            ImGui::TableSetColumnIndex(2);
            ImGui::PushID(pugi::as_utf8(path.wstring()).c_str());
            ImGui::PushStyleColor(
                ImGuiCol_::ImGuiCol_Text,
                isDirectory? IM_COL32(128, 128, 128, 255) : IM_COL32(255, 255, 255, 255));

            if(ImGui::Selectable(
                    (isDirectory ? "Directory" : "File"),
                    std::find(m_SelectedPaths.begin(), m_SelectedPaths.end(), path) != m_SelectedPaths.end(),
                    ImGuiSelectableFlags_::ImGuiSelectableFlags_AllowDoubleClick   |
                        ImGuiSelectableFlags_::ImGuiSelectableFlags_SpanAllColumns |
                        ImGuiSelectableFlags_::ImGuiSelectableFlags_AllowOverlap))
            {
                if(isDirectory)
                {
                    if(ImGui::IsMouseDoubleClicked(ImGuiMouseButton_::ImGuiMouseButton_Left) || ImGui::IsKeyPressed(ImGuiKey::ImGuiKey_Enter))
                        ChangeCurrentPath(path);
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

                // update new folder name
                m_NewFolder = pugi::as_utf8(path.filename().wstring());
            }

            ImGui::PopStyleColor();

            DrawContextMenu();

            ImGui::SetItemTooltip("Select and then right click onto an item to call context popup menu");

            ImGui::PopID();
        }

        ImGui::EndTable();

        HandleKeyInputs();
        DrawFormatFilter();
    }
}

void ImGuiApplicationFileSystemDialogLayer::DrawPathsTree(std::filesystem::path _Path, const bool& _IsRoot)
{
    ImGuiTreeNodeFlags flags =
        ImGuiTreeNodeFlags_::ImGuiTreeNodeFlags_SpanAvailWidth | ImGuiTreeNodeFlags_::ImGuiTreeNodeFlags_DrawLinesFull | ImGuiTreeNodeFlags_OpenOnDoubleClick;

    auto drawChildFolders = [this, &flags](std::filesystem::path _Path)
    {
        try
        {
            for(const auto& directory :
                 std::filesystem::directory_iterator(_Path.make_preferred(), std::filesystem::directory_options::skip_permission_denied))
            {
                if(!directory.is_directory())
                    continue;

                if(ImGui::TreeNodeEx(pugi::as_utf8(directory.path().filename().wstring()).c_str(), flags))
                {
                    DrawPathsTree(directory.path(), false);
                    ImGui::TreePop();
                }

                if(ImGui::IsItemClicked(ImGuiMouseButton_::ImGuiMouseButton_Left))
                    ChangeCurrentPath(directory.path());
            }
        }
        catch(...)
        {
        }
    };

    if(_IsRoot)
    {
        if(ImGui::BeginChild("TreeViewChildNode", ImVec2(), ImGuiChildFlags_::ImGuiChildFlags_None))
        {
            if(ImGui::TreeNodeEx(pugi::as_utf8(_Path.wstring()).c_str(), flags))
            {
                drawChildFolders(_Path);
                ImGui::TreePop();
            }

            if(ImGui::IsItemClicked(ImGuiMouseButton_::ImGuiMouseButton_Left))
                ChangeCurrentPath(_Path);

            ImGui::EndChild();
        }
    }
    else
    {
        drawChildFolders(_Path);
    }
};

void ImGuiApplicationFileSystemDialogLayer::HandleKeyInputs()
{
    if(Contains<ImGuiApplicationFileSystemPathsRenamerDialogLayer>())
        return;

    // delete
    if(ImGui::IsKeyPressed(ImGuiKey::ImGuiKey_Delete))
        OnRemoveFilesOrFoldersAction();

    // select all
    if(ImGui::IsKeyDown(ImGuiKey::ImGuiKey_LeftCtrl) &&
        ImGui::IsKeyPressed(ImGuiKey::ImGuiKey_A))
    {
        m_SelectedPaths.clear();

        for(const auto& directory :
             std::filesystem::directory_iterator(std::filesystem::current_path().make_preferred(), std::filesystem::directory_options::skip_permission_denied))
        {
            // apply a format filter
            auto iterator = m_FormatFilter.find(directory.path().extension().string());

            if(!directory.is_directory() &&
                (iterator == m_FormatFilter.end() || !iterator->second))
                continue;

            m_SelectedPaths.push_back(directory.path());
        }
    }

    // deselect all
    if(ImGui::IsKeyPressed(ImGuiKey::ImGuiKey_Escape))
        m_SelectedPaths.clear();

    // copy
    if(ImGui::IsKeyDown(ImGuiKey::ImGuiKey_LeftCtrl) &&
        ImGui::IsKeyPressed(ImGuiKey::ImGuiKey_C))
        OnCopyFilesOrFoldersAction();

    // paste
    if(ImGui::IsKeyDown(ImGuiKey::ImGuiKey_LeftCtrl) &&
        ImGui::IsKeyPressed(ImGuiKey::ImGuiKey_V))
        OnPasteFilesOrFoldersAction();

}

void ImGuiApplicationFileSystemDialogLayer::OnLevelUpAction()
{
    if(Contains<ImGuiApplicationFileSystemPathsRenamerDialogLayer>())
        return;

    // push current path onto stack
    m_VisitedPathsStack.push(std::filesystem::current_path());

    // go higher
    ChangeCurrentPath(std::filesystem::current_path().parent_path());
}

void ImGuiApplicationFileSystemDialogLayer::OnLevelDownAction()
{
    if(Contains<ImGuiApplicationFileSystemPathsRenamerDialogLayer>())
        return;

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
    if(Contains<ImGuiApplicationFileSystemPathsRenamerDialogLayer>())
        return;

    std::wstring newFolderName;
    std::wstring newFolderNameCache = pugi::as_wide(m_NewFolder);

    for(size_t i = 0; i < newFolderNameCache.size(); i++)
    {
        if(newFolderNameCache[i] != '\0')
            newFolderName.push_back(newFolderNameCache[i]);
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

    m_NewFolder = pugi::as_utf8(newFolderName);
    m_NewFolder.push_back('\0');
}

void ImGuiApplicationFileSystemDialogLayer::OnRemoveFilesOrFoldersAction()
{
    if(Contains<ImGuiApplicationFileSystemPathsRenamerDialogLayer>())
        return;

    for(auto& path : m_SelectedPaths)
    {
        try
        {
            std::filesystem::remove_all(path);
        }
        catch(...)
        {

        }
    }
}

void ImGuiApplicationFileSystemDialogLayer::OnRenameFilesOrFoldersAction()
{
    if(!Contains<ImGuiApplicationFileSystemPathsRenamerDialogLayer>())
        Push<ImGuiApplicationFileSystemPathsRenamerDialogLayer>(m_SelectedPaths);
}

void ImGuiApplicationFileSystemDialogLayer::OnCopyFilesOrFoldersAction()
{
    if(Contains<ImGuiApplicationFileSystemPathsRenamerDialogLayer>())
        return;

    m_ReadyToCopyPaths = m_SelectedPaths;
}

void ImGuiApplicationFileSystemDialogLayer::OnPasteFilesOrFoldersAction()
{
    if(Contains<ImGuiApplicationFileSystemPathsRenamerDialogLayer>())
        return;

    struct FileInfo
    {
        std::wstring name      = std::wstring();
        std::string  extention = std::string();
    };

    auto getFileInfo = [](std::filesystem::path path, int _Depth = 2)->FileInfo
    {
        std::string extention = std::string();

        for(int i = 0; i < _Depth; i++)
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

    for(auto& fileToCopy : m_ReadyToCopyPaths)
    {
        auto fileInfo = getFileInfo(fileToCopy);

        auto source = fileToCopy;

        auto target = std::filesystem::path(fileToCopy.parent_path().wstring()
                                                .append(L"/")
                                                .append(fileInfo.name)
                                                .append(pugi::as_wide(fileInfo.extention))).make_preferred();

        while(std::filesystem::exists(target))
        {
            target = std::filesystem::path(fileToCopy.parent_path().wstring()
                                               .append(L"/")
                                               .append(fileInfo.name.append(L"_Copy"))
                                               .append(pugi::as_wide(fileInfo.extention))).make_preferred();
        }

        std::filesystem::copy(source, target);
    }
}
