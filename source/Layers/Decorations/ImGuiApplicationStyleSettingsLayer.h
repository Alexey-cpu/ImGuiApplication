#ifndef IMGUIAPPLICATIONSTYLESETTINGSLAYER_H
#define IMGUIAPPLICATIONSTYLESETTINGSLAYER_H

// Custom
#include <ImGuiApplicationLayer.h>

// STL
#include <filesystem>
#include <iostream>

// imgui
#include <imgui.h>
#include <imgui_internal.h>

// pugixml
#include <pugixml.hpp>

// ImGuiApplicationStyleSettingsLayer
class ImGuiApplicationStyleSettingsLayer : public ImGuiApplicationLayer
{
public:

    // constructors
    ImGuiApplicationStyleSettingsLayer(
        std::filesystem::path _Path                  = std::filesystem::current_path(),
        std::wstring          _StyleSettingsFileName = L"ImGuiApplicationStyleSettingsLayer") :
        ImGuiApplicationLayer("ImGuiApplicationStyleSettingsLayer"),
        m_Path(!std::filesystem::is_directory(_Path) ? _Path.parent_path() : _Path),
        m_StyleSettingsFileName(_StyleSettingsFileName){}

    // virtual destructor
    virtual ~ImGuiApplicationStyleSettingsLayer(){}

    // ImGuiApplicationLayer
    virtual void OnClose() override
    {
        SaveState();
    }

    virtual void OnAwake() override
    {
        LoadFonts();
        LoadStyle();
    }

    virtual void OnStart() override
    {
        ApplyState();
    }

    virtual void OnUpdate() override
    {
        ImGui::Begin(m_Name.c_str(), &m_Opened);

        if (ImGui::BeginTabBar("##tabs", ImGuiTabBarFlags_None))
        {
            // colors
            if (ImGui::BeginTabItem("Colors"))
            {
                DrawColorsTab();
                ImGui::EndTabItem();
            }

            // fonts
            if(ImGui::BeginTabItem("Fonts"))
            {
                DrawFontsTab();
                ImGui::EndTabItem();
            }

            ImGui::EndTabBar();
        }

        ImGui::End();
    }

    virtual void OnFinish() override
    {
        SaveState();
    }

protected:


    std::filesystem::path m_Path                  = std::filesystem::current_path();
    std::wstring          m_StyleSettingsFileName = L"ImGuiApplicationStyleSettingsLayer";
    int                   m_DefaultFontSize       = 18.0;
    int                   m_FontSize              = m_DefaultFontSize;
    std::string           m_FontName              = std::string();
    ImGuiStyle            m_ReferenceStyle;
    ImGuiTextFilter       m_ColorsTextFilter;
    ImGuiColorEditFlags   m_ColorsAlphaFlags = 0;


    // service methods
    void SaveState()
    {
        // get ImGUI IO
        auto& io = ImGui::GetIO();

        // save style settings to file
        auto styleSettingsFilePath =
            m_Path.wstring().append(L"/").append(m_StyleSettingsFileName).append(L".xml");

        // create document
        pugi::xml_document doc;
        auto style = doc.append_child("Style");
        auto font  = style.append_child("Font");
        font.append_child("Size").text().set(std::to_string(m_FontSize));
        font.append_child("Name").text().set(m_FontName.c_str());

        // save document
        doc.save_file(styleSettingsFilePath.c_str());
    }

    void ApplyState()
    {
        // apply state
        auto& io = ImGui::GetIO();
        ImFontAtlas* atlas = io.Fonts;

        for (ImFont* font : atlas->Fonts)
        {
            // font size
            m_FontSize  = std::min(std::max(m_FontSize, 12), 96);
            m_FontName  = ImGui::GetIO().FontDefault == nullptr ? "Default" : ImGui::GetIO().FontDefault->GetDebugName();
            font->Scale =  (float)m_FontSize / (float)m_DefaultFontSize;
        }
    }

    void DrawFontsTab()
    {
        auto& io = ImGui::GetIO();
        ImFontAtlas* atlas = io.Fonts;

        ImGui::TextUnformatted(m_FontName.c_str());
        ImGui::SameLine();

        ImGui::PushID(("FontSize_" + m_FontName).c_str());
        ImGui::SetNextItemWidth(ImGui::GetContentRegionAvail().x);
        ImGui::DragInt("", &m_FontSize, 0.01f, 12, 96);
        ImGui::PopID();
        ImGuiStyle& style = ImGui::GetStyle();

        // Font list
        for (ImFont* font : atlas->Fonts)
        {
            if(ImGui::TreeNode(font, "Font: \"%s\": %.2f px, %d glyphs, %d sources(s)", font->Sources ? font->Sources[0].Name : "", font->FontSize, font->Glyphs.Size, font->SourcesCount))
            {
                ImGui::PushFont(font);
                ImGui::SeparatorText("The quick brown fox jumps over the lazy dog");
                ImGui::PopFont();

                if (ImGui::Button("Select this font"))
                    ImGui::GetIO().FontDefault = font;

                // Display details
                ImGui::SameLine();
                ImGui::SetNextItemWidth(ImGui::GetFontSize() * 8);

                ImGui::SameLine();
                ImGui::Text("Ascent: %f, Descent: %f, Height: %f", font->Ascent, font->Descent, font->Ascent - font->Descent);

                char c_str[5];
                ImGui::Text("Fallback character: '%s' (U+%04X)", ImTextCharToUtf8(c_str, font->FallbackChar), font->FallbackChar);
                ImGui::SetItemTooltip("Replacement glyph that a system displays when it cannot render a specific character using its primary font or fonts");

                ImGui::Text("Ellipsis character: '%s' (U+%04X)", ImTextCharToUtf8(c_str, font->EllipsisChar), font->EllipsisChar);
                ImGui::SetItemTooltip("A punctuation mark indicating an omission or pause in text.");

                const int surface_sqrt = (int)ImSqrt((float)font->MetricsTotalSurface);
                ImGui::Text("Texture Area: about %d px ~%dx%d px", font->MetricsTotalSurface, surface_sqrt, surface_sqrt);
                ImGui::SetItemTooltip("The region of a texture where glyphs (individual characters) are stored.");

                for (int config_i = 0; config_i < font->SourcesCount; config_i++)
                {
                    if (font->Sources)
                    {
                        const ImFontConfig* src = &font->Sources[config_i];
                        int oversample_h, oversample_v;
                        ImFontAtlasBuildGetOversampleFactors(src, &oversample_h, &oversample_v);
                        ImGui::BulletText("Input %d: \'%s\', Oversample: (%d=>%d,%d=>%d), PixelSnapH: %d, Offset: (%.1f,%.1f)",
                                          config_i, src->Name, src->OversampleH, oversample_h, src->OversampleV, oversample_v, src->PixelSnapH, src->GlyphOffset.x, src->GlyphOffset.y);
                    }
                }

                // Display all glyphs of the fonts in separate pages of 256 characters
                {
                    if (ImGui::TreeNode("Glyphs", "Glyphs (%d)", font->Glyphs.Size))
                    {
                        ImDrawList* draw_list = ImGui::GetWindowDrawList();
                        const ImU32 glyph_col = ImGui::GetColorU32(ImGuiCol_Text);
                        const float cell_size = font->FontSize * 1;
                        const float cell_spacing = ImGui::GetStyle().ItemSpacing.y;
                        for (unsigned int base = 0; base <= IM_UNICODE_CODEPOINT_MAX; base += 256)
                        {
                            // Skip ahead if a large bunch of glyphs are not present in the font (test in chunks of 4k)
                            // This is only a small optimization to reduce the number of iterations when IM_UNICODE_MAX_CODEPOINT
                            // is large // (if ImWchar==ImWchar32 we will do at least about 272 queries here)
                            if (!(base & 8191) && font->IsGlyphRangeUnused(base, base + 8191))
                            {
                                base += 8192 - 256;
                                continue;
                            }

                            int count = 0;
                            for (unsigned int n = 0; n < 256; n++)
                                if (font->FindGlyphNoFallback((ImWchar)(base + n)))
                                    count++;
                            if (count <= 0)
                                continue;
                            if (!ImGui::TreeNode((void*)(intptr_t)base, "U+%04X..U+%04X (%d %s)", base, base + 255, count, count > 1 ? "glyphs" : "glyph"))
                                continue;

                            // Draw a 16x16 grid of glyphs
                            ImVec2 base_pos = ImGui::GetCursorScreenPos();
                            for (unsigned int n = 0; n < 256; n++)
                            {
                                // We use ImFont::RenderChar as a shortcut because we don't have UTF-8 conversion functions
                                // available here and thus cannot easily generate a zero-terminated UTF-8 encoded string.
                                ImVec2 cell_p1(base_pos.x + (n % 16) * (cell_size + cell_spacing), base_pos.y + (n / 16) * (cell_size + cell_spacing));
                                ImVec2 cell_p2(cell_p1.x + cell_size, cell_p1.y + cell_size);
                                const ImFontGlyph* glyph = font->FindGlyphNoFallback((ImWchar)(base + n));
                                draw_list->AddRect(cell_p1, cell_p2, glyph ? IM_COL32(255, 255, 255, 100) : IM_COL32(255, 255, 255, 50));
                                if (!glyph)
                                    continue;
                                font->RenderChar(draw_list, cell_size, cell_p1, glyph_col, (ImWchar)(base + n));
                                if (ImGui::IsMouseHoveringRect(cell_p1, cell_p2) && ImGui::BeginTooltip())
                                {
                                    ImGui::DebugNodeFontGlyph(font, glyph);
                                    ImGui::EndTooltip();
                                }
                            }
                            ImGui::Dummy(ImVec2((cell_size + cell_spacing) * 16, (cell_size + cell_spacing) * 16));
                            ImGui::TreePop();
                        }
                        ImGui::TreePop();
                    }
                }

                ImGui::TreePop();
            }
        }
    }

    void DrawColorsTab()
    {
        ImGuiStyle& style = ImGui::GetStyle();

        m_ColorsTextFilter.Draw("Filter colors", ImGui::GetFontSize() * 16);

        ImGui::SameLine();

        if (ImGui::RadioButton("Opaque", m_ColorsAlphaFlags == ImGuiColorEditFlags_AlphaOpaque))
            m_ColorsAlphaFlags = ImGuiColorEditFlags_AlphaOpaque;
        ImGui::SameLine();

        if (ImGui::RadioButton("Alpha", m_ColorsAlphaFlags == ImGuiColorEditFlags_None))
            m_ColorsAlphaFlags = ImGuiColorEditFlags_None;
        ImGui::SameLine();

        if (ImGui::RadioButton("Both", m_ColorsAlphaFlags == ImGuiColorEditFlags_AlphaPreviewHalf))
            m_ColorsAlphaFlags = ImGuiColorEditFlags_AlphaPreviewHalf;

        ImGui::SetNextWindowSizeConstraints(ImVec2(0.0f, ImGui::GetTextLineHeightWithSpacing() * 10), ImVec2(FLT_MAX, FLT_MAX));

        ImGui::BeginChild(
            "##colors",
            ImVec2(0, 0),
            ImGuiChildFlags_Borders | ImGuiChildFlags_NavFlattened, ImGuiWindowFlags_AlwaysVerticalScrollbar | ImGuiWindowFlags_AlwaysHorizontalScrollbar);

        ImGui::PushItemWidth(ImGui::GetFontSize() * -12);

        for (int i = 0; i < ImGuiCol_COUNT; i++)
        {
            const char* name = ImGui::GetStyleColorName(i);

            if (!m_ColorsTextFilter.PassFilter(name))
                continue;
            ImGui::PushID(i);

            if (ImGui::Button("?"))
                ImGui::DebugFlashStyleColor((ImGuiCol)i);
            ImGui::SetItemTooltip("Flash given color to identify places where it is used.");
            ImGui::SameLine();

            ImGui::ColorEdit4("##color", (float*)&style.Colors[i], ImGuiColorEditFlags_AlphaBar | m_ColorsAlphaFlags);

            if (memcmp(&style.Colors[i], &m_ReferenceStyle.Colors[i], sizeof(ImVec4)) != 0)
            {
                // Tips: in a real user application, you may want to merge and use an icon font into the main font,
                // so instead of "Save"/"Revert" you'd use icons!
                // Read the FAQ and docs/FONTS.md about using icon fonts. It's really easy and super convenient!
                ImGui::SameLine(0.0f, style.ItemInnerSpacing.x);
                if (ImGui::Button("Save"))
                    m_ReferenceStyle.Colors[i] = style.Colors[i];

                ImGui::SameLine(0.0f, style.ItemInnerSpacing.x);
                if (ImGui::Button("Revert"))
                    style.Colors[i] = m_ReferenceStyle.Colors[i];
            }

            ImGui::SameLine(0.0f, style.ItemInnerSpacing.x);
            ImGui::TextUnformatted(name);
            ImGui::PopID();
        }
        ImGui::PopItemWidth();
        ImGui::EndChild();
    }

    void LoadFonts()
    {
        if(!std::filesystem::exists(m_Path))
            return;

        // retrive ImGui IO
        auto& io = ImGui::GetIO();

        // recursivelly scan path for .ttf fonts
        for(const auto& directory :
             std::filesystem::recursive_directory_iterator(m_Path))
        {
            if(directory.is_directory() ||
                directory.path().extension() != ".ttf")
                continue;

            io.Fonts->AddFontFromFileTTF(
                pugi::as_utf8(directory.path().wstring()).c_str(),
                m_DefaultFontSize * 4.0 / 3.0,
                NULL,
                io.Fonts->GetGlyphRangesCyrillic());
        }

        // build fonts
        io.Fonts->Build();
    }

    void LoadStyle()
    {
        if(!std::filesystem::exists(m_Path))
            return;

        // retrive ImGui IO
        auto& io = ImGui::GetIO();

        // recursivelly scan path for .xml styles
        for(const auto& directory :
             std::filesystem::recursive_directory_iterator(m_Path))
        {
            if(directory.path().extension() != ".xml" &&
                directory.path().filename().stem().wstring() != m_StyleSettingsFileName)
                continue;

            // parse settings
            pugi::xml_document doc;
            doc.load_file(directory.path().wstring().c_str());

            auto style    = doc.child("Style");
            auto font     = style.child("Font");
            auto fontSize = font.child("Size");
            auto fontName = font.child("Name");

            // setup font size
            m_FontSize = fontSize.text().as_int();

            // setup application font font
            ImFontAtlas* atlas = io.Fonts;

            for(auto font : atlas->Fonts)
            {
                if(fontName.text().as_string() == std::string(font->GetDebugName()))
                    io.FontDefault = font;
            }

            // setup application style...
            m_ReferenceStyle = ImGui::GetStyle();

            return;
        }
    }
};

#endif // IMGUIAPPLICATIONSTYLESETTINGSLAYER_H
