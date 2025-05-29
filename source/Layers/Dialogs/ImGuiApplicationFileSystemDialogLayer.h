#ifndef IMGUIAPPLICATIONFILESYSTEMDIALOGLAYER_H
#define IMGUIAPPLICATIONFILESYSTEMDIALOGLAYER_H

// Custom
#include <ImGuiApplication.h>
#include <ImGuiApplicationLayer.h>
#include <ImGuiApplicationDialogLayer.h>

// ImGUI
#include <imgui.h>

// STL
#include <filesystem>
#include <string>
#include <stack>
#include <map>

// pugixml
#include <pugixml.hpp>

// ImGuiApplicationFileSystemPathItem
class ImGuiApplicationFileSystemPathItem
{
public:

    // constructor
    ImGuiApplicationFileSystemPathItem(std::filesystem::path _Path = std::filesystem::path()) : m_Path(_Path)
    {
        // setup bufferes content
        m_PathBuffer     = pugi::as_utf8(_Path.wstring());
        m_FileNameBuffer = pugi::as_utf8(_Path.filename().wstring());

        // add empty symbols here
        for(size_t i = 0; i < 4 * std::max<size_t>(_Path.wstring().size(), 64); i++)
        {
            m_PathBuffer.push_back('\0');
            m_FileNameBuffer.push_back('\0');
        }
    }

    // destryctor
    ~ImGuiApplicationFileSystemPathItem(){}

    // operators override
    bool operator == (ImGuiApplicationFileSystemPathItem _Other)
    {
        return m_Path == _Other.m_Path;
    }

    bool isDirectory() const
    {
        return std::filesystem::is_directory(m_Path);
    }

    // info
    std::filesystem::path m_Path;
    std::string           m_PathBuffer;
    std::string           m_FileNameBuffer;
    bool                  m_Selected = false;
};

// ImGuiApplicationFileSystemPathsRenamerDialogLayer
class ImGuiApplicationFileSystemPathsRenamerDialogLayer : public ImGuiApplicationDialogLayer
{
public:

    // constructors
    ImGuiApplicationFileSystemPathsRenamerDialogLayer(
        const std::vector<ImGuiApplicationFileSystemPathItem>& _Items =
        std::vector<ImGuiApplicationFileSystemPathItem>());

    // destructor
    virtual ~ImGuiApplicationFileSystemPathsRenamerDialogLayer();

    // ImGuiApplicationDialogLayer
    virtual void DrawContent() override;
    virtual void DrawButtons() override;

protected:

    std::vector<ImGuiApplicationFileSystemPathItem> m_Items;
};

// ImGuiApplicationFileSystemDialogLayer
class ImGuiApplicationFileSystemDialogLayer : public ImGuiApplicationDialogLayer
{
public:

    // constructors
    ImGuiApplicationFileSystemDialogLayer(
        const std::filesystem::path&    _RootPath = std::filesystem::current_path(),
        const std::string&              _Title    = "ImGuiApplicationFileSystemBrowserDialogLayer",
        const std::vector<std::string>& _Formats  = std::vector<std::string>());

    // destructor
    virtual ~ImGuiApplicationFileSystemDialogLayer();

    // ImGuiApplicationDialogLayer
    virtual void DrawContent() override;
    virtual void DrawButtons() override;

protected:

    // info
    ImGuiApplicationFileSystemPathItem              m_CurrentFolder     = ImGuiApplicationFileSystemPathItem(std::filesystem::current_path().string());
    ImGuiApplicationFileSystemPathItem              m_CurrentFile       = ImGuiApplicationFileSystemPathItem(std::filesystem::current_path().string());
    ImGuiApplicationFileSystemPathItem              m_NewFolder         = ImGuiApplicationFileSystemPathItem();
    std::map<std::string, bool>                     m_FormatFilter      = std::map<std::string, bool>();
    std::vector<ImGuiApplicationFileSystemPathItem> m_SelectedPaths     = std::vector<ImGuiApplicationFileSystemPathItem>();
    std::vector<ImGuiApplicationFileSystemPathItem> m_FilesToCopy       = std::vector<ImGuiApplicationFileSystemPathItem>();
    std::stack<std::filesystem::path>               m_VisitedPathsStack = std::stack<std::filesystem::path>();

    // service methods
    void ChangeCurrentPath(std::filesystem::path _Path);
    void SetupFormatFilter(const std::vector<std::string>& _Formats = std::vector<std::string>());
    void DrawContextMenu();

    // callbacks
    void OnGoUpperAction();
    void OnGoDeeperAction();
    void OnCreateFolderAction();
    void OnRemoveFilesOrFoldersAction();
    void OnRenameFilesOrFoldersAction();
    void OnCopyFilesOrFoldersAction();
    void OnPasteFilesOrFoldersAction();
};

#endif // IMGUIAPPLICATIONFILESYSTEMDIALOGLAYER_H
