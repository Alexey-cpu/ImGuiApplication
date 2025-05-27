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

// ImGuiApplicationFileSystemPathItem
class ImGuiApplicationFileSystemPathItem
{
public:

    // constructor
    ImGuiApplicationFileSystemPathItem(
        bool                  _IsDirectory,
        std::filesystem::path _Path,
        std::string           _Filename) :
        m_IsDirectory(_IsDirectory),
        m_Path(_Path),
        m_Filename(_Filename){}

    // destryctor
    ~ImGuiApplicationFileSystemPathItem(){}

    // operators override
    bool operator == (ImGuiApplicationFileSystemPathItem _Other)
    {
        return m_Path == _Other.m_Path &&
               m_Filename == _Other.m_Filename &&
               m_IsDirectory == _Other.m_IsDirectory;
    }

    // info
    bool                  m_IsDirectory;
    std::filesystem::path m_Path;
    std::string           m_Filename;
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
    std::string                                     m_Title                  = "ImGuiApplicationFileSystemBrowserDialogLayer";
    std::string                                     m_CurrentDirectoryBuffer = std::filesystem::current_path().string();
    std::string                                     m_CurrentFilenameBuffer  = std::filesystem::current_path().string();
    std::map<std::string, bool>                     m_FormatFilter           = std::map<std::string, bool>();
    std::vector<ImGuiApplicationFileSystemPathItem> m_SelectedPaths          = std::vector<ImGuiApplicationFileSystemPathItem>();
    std::stack<std::filesystem::path>               m_PathStack;

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
