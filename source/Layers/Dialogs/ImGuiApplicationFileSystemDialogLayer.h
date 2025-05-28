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

#if defined(_WIN32) | defined(_WIN64)
#include <iconvlite.h>
#endif

// ImGuiApplicationFileSystemPathItem
class ImGuiApplicationFileSystemPathItem
{
public:

    // constructor
    ImGuiApplicationFileSystemPathItem(
        std::filesystem::path _Path = std::filesystem::path()) :
        m_IsDirectory(std::filesystem::is_directory(_Path)),
        m_Path(_Path),
#if defined(_WIN32) | defined(_WIN64)
        m_DirectoryBuffer(cp2utf(_Path.string())),
        m_FolderNameBuffer(cp2utf(_Path.filename().string())),
        m_FileNameBuffer((m_IsDirectory ? std::string() : m_FolderNameBuffer))
#else
        m_DirectoryBuffer(_Path.string()),
        m_FilenameBuffer(_Path.filename().string())
#endif
    {}

    // destryctor
    ~ImGuiApplicationFileSystemPathItem(){}

    // operators override
    bool operator == (ImGuiApplicationFileSystemPathItem _Other)
    {
        return m_Path            == _Other.m_Path            &&
               m_DirectoryBuffer == _Other.m_DirectoryBuffer &&
               m_FolderNameBuffer  == _Other.m_FolderNameBuffer  &&
               m_IsDirectory     == _Other.m_IsDirectory;
    }

    // info
    bool                  m_IsDirectory;
    std::filesystem::path m_Path;
    std::string           m_DirectoryBuffer;
    std::string           m_FolderNameBuffer;
    std::string           m_FileNameBuffer;
};

// ImGuiApplicationFileSystemPathsRenamerDialogLayer
class ImGuiApplicationFileSystemPathsRenamerDialogLayer : public ImGuiApplicationDialogLayer
{
public:

    // constructors
    ImGuiApplicationFileSystemPathsRenamerDialogLayer(const std::vector<ImGuiApplicationFileSystemPathItem>& m_Items = std::vector<ImGuiApplicationFileSystemPathItem>());

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
    std::string                                     m_Title            = "ImGuiApplicationFileSystemBrowserDialogLayer";
    ImGuiApplicationFileSystemPathItem              m_CurrentDirectory = ImGuiApplicationFileSystemPathItem(std::filesystem::current_path().string());
    ImGuiApplicationFileSystemPathItem              m_CurrentFile      = ImGuiApplicationFileSystemPathItem(std::filesystem::current_path().string());
    std::map<std::string, bool>                     m_FormatFilter     = std::map<std::string, bool>();
    std::vector<ImGuiApplicationFileSystemPathItem> m_SelectedPaths    = std::vector<ImGuiApplicationFileSystemPathItem>();
    std::stack<std::filesystem::path>               m_PathStack        = std::stack<std::filesystem::path>();

    // service methods
    void ChangeCurrentPath(std::filesystem::path _Path);
    void SetupFormatFilter(const std::vector<std::string>& _Formats = std::vector<std::string>());

    // callbacks
    void OnBackwardButtonClicked();
    void OnForwardButtonClicked();
    void OnMakeDirectioryButtonClicked();
    void OnRemoveDirectoryButtonClicked();
    void OnRenameDirectoryButtonClicked();
};

#endif // IMGUIAPPLICATIONFILESYSTEMDIALOGLAYER_H
