#ifndef IMGUIAPPLICATIONSETTINGSFONTS_H
#define IMGUIAPPLICATIONSETTINGSFONTS_H

// Custom
#include <ImGuiApplication.h>
#include <ImGuiApplicationSerializable.h>
#include <ImGuiApplicationFileSystemDialog.h>

// imgui
#include <imgui.h>
#include <imgui_internal.h>

namespace ImGuiApplication::Settings
{
    class Fonts : public Layer, public IXSerializable
    {
    public:

        Fonts(const std::filesystem::path& _Path =
              std::filesystem::path(std::filesystem::current_path().wstring().append(L"/fonts")));
        virtual ~Fonts();

        virtual void OnClose() override;
        virtual void OnAwake() override;
        virtual void OnStart() override;
        virtual void OnUpdate() override;
        virtual void OnFinish() override;

        virtual pugi::xml_node Serialize(pugi::xml_node& _Parent) override;
        virtual bool Deserialize(pugi::xml_node& _Node) override;

    protected:

        std::filesystem::path m_Path;
        int                   m_DefaultFontSize = 18.0;
        int                   m_FontSize        = m_DefaultFontSize;
        std::string           m_FontName        = std::string();
        std::string           m_FontsDirectory  = std::string();

        std::shared_ptr<ImGuiApplication::Dialogs::FileSystemDialog> m_FileSystemDialog = nullptr;
    };
}

#endif // IMGUIAPPLICATIONSETTINGSFONTS_H
