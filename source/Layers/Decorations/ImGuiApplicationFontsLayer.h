#ifndef IMGUIAPPLICATIONFONTSLAYER_H
#define IMGUIAPPLICATIONFONTSLAYER_H

// Custom
#include <ImGuiApplicationLayer.h>

// STL
#include <filesystem>

// imgui
#include <imgui.h>

// pugixml
#include <pugixml.hpp>

// ImGuiApplicationFontsLayer
class ImGuiApplicationFontsLayer : public ImGuiApplicationLayer
{
public:

    // constrcutors
    ImGuiApplicationFontsLayer(std::filesystem::path _Path) :
        ImGuiApplicationLayer("ImGuiApplicationFontsProvider"), m_Path(_Path)
    {
    }

    // virtual destructor
    virtual ~ImGuiApplicationFontsLayer(){}

    // ImGuiApplicationLayer
    virtual void Start() override
    {
        if(!std::filesystem::exists(m_Path))
            return;

        auto& io = ImGui::GetIO();

        // recursivelly scan path for .ttf fonts
        for(const auto& directory :
             std::filesystem::recursive_directory_iterator(m_Path))
        {
            if(directory.is_directory() ||
                directory.path().extension() != ".ttf")
                continue;

            m_AvailableFonts.push_back(io.Fonts->AddFontFromFileTTF(
                pugi::as_utf8(directory.path().wstring()).c_str(),
                4.0/3.0 * m_FontSize,
                NULL,
                io.Fonts->GetGlyphRangesCyrillic()));
        }

        // build fonts
        io.Fonts->Build();

        // setup current font
        //m_CurrentFont = m_AvailableFonts.front();
    }

    virtual void BeforeUpdate() override
    {
        //if(!m_AvailableFonts.empty() && m_CurrentFont != nullptr)
        //    ImGui::PushFont(m_CurrentFont);
    }

    virtual void AfterUpdate() override
    {
        //if(!m_AvailableFonts.empty() && m_CurrentFont != nullptr)
        //    ImGui::PopFont();
    }

protected:

    // info
    std::filesystem::path m_Path           = std::filesystem::current_path();
    int                   m_FontSize       = 24;
    ImFont*               m_CurrentFont    = nullptr;
    std::vector<ImFont*>  m_AvailableFonts = std::vector<ImFont*>();
};

#endif // IMGUIAPPLICATIONFONTSLAYER_H
