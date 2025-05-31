#ifndef IMGUIAPPLICATIONFILESYSTEMDIALOG_H
#define IMGUIAPPLICATIONFILESYSTEMDIALOG_H

// Custom
#include <ImGuiApplication.h>
#include <ImGuiApplicationLayer.h>
#include <ImGuiApplicationDialog.h>

// ImGUI
#include <imgui.h>

// STL
#include <filesystem>
#include <string>
#include <stack>
#include <map>

// pugixml
#include <pugixml.hpp>

namespace ImGuiApplication::Dialogs
{
    class FileSystemPathsRenamerDialog : public Dialog
    {
    public:

        FileSystemPathsRenamerDialog(
            const std::vector<std::filesystem::path>& _Items =
            std::vector<std::filesystem::path>());

        virtual ~FileSystemPathsRenamerDialog();

        virtual void DrawContent() override;
        virtual void DrawButtons() override;

    protected:

        struct Path
        {
            Path(std::filesystem::path _Path, std::string _Filename) :
                path(_Path),
                filename(_Filename){}

            std::filesystem::path path;
            std::string           filename;
        };

        typedef std::vector<Path> Paths;

        Paths m_Paths;
    };

    class FileSystemDialog : public Dialog
    {
    public:

        FileSystemDialog(
            const std::filesystem::path&    _RootPath = std::filesystem::current_path(),
            const std::string&              _Title    = "ImGuiApplicationFileSystemBrowserDialogLayer",
            const std::vector<std::string>& _Formats  = std::vector<std::string>());

        virtual ~FileSystemDialog();

        std::string GetFolderName() const;
        std::string GetFileName() const;
        std::vector<std::filesystem::path> GetSelectedPaths() const;

        // ImGuiApplicationDialogLayer
        virtual void DrawContent() override;
        virtual void DrawButtons() override;

    protected:

        // info
        std::string                        m_CurrentFolder     = std::string();
        std::string                        m_NewFolder         = std::string();
        std::map<std::string, bool>        m_FormatFilter      = std::map<std::string, bool>();
        std::vector<std::filesystem::path> m_SelectedPaths     = std::vector<std::filesystem::path>();
        std::vector<std::filesystem::path> m_ReadyToCopyPaths  = std::vector<std::filesystem::path>();
        std::stack<std::filesystem::path>  m_VisitedPathsStack = std::stack<std::filesystem::path>();

        // service methods
        bool ChangeCurrentPath(std::filesystem::path _Path);
        void SetupFormatFilter(const std::vector<std::string>& _Formats = std::vector<std::string>());
        void DrawBrowser();
        void DrawBrowserContextMenu();
        void DrawBrowserFormatFilter();
        void DrawPathsTree(std::filesystem::path, const bool& = true);
        void HandleKeyInputs();

        // callbacks
        void OnLevelUpAction();
        void OnLevelDownAction();
        void OnCreateFolderAction();
        void OnRemoveFilesOrFoldersAction();
        void OnRenameFilesOrFoldersAction();
        void OnCopyFilesOrFoldersAction();
        void OnPasteFilesOrFoldersAction();
    };
};

#endif // IMGUIAPPLICATIONFILESYSTEMDIALOG_H
