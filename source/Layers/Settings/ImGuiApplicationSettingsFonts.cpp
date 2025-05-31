#include <ImGuiApplicationSettingsFonts.h>

using namespace ImGuiApplication::Settings;

Fonts::Fonts(const std::filesystem::path& _Path) :
    Layer("Fonts"), m_Path(_Path){}

Fonts::~Fonts(){}

void Fonts::OnClose(){}

void Fonts::OnAwake()
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

void Fonts::OnStart()
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

void Fonts::OnUpdate()
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

void Fonts::OnFinish(){}

pugi::xml_node Fonts::Serialize(pugi::xml_node& _Parent)
{
    auto fonts = _Parent.append_child("Fonts");
    fonts.append_child("Size").text().set(std::to_string(m_FontSize).c_str());
    fonts.append_child("Name").text().set(m_FontName.c_str());
    fonts.append_child("Path").text().set(pugi::as_utf8(m_Path.wstring()).c_str());
    return fonts;
}

bool Fonts::Deserialize(pugi::xml_node& _Node)
{
    pugi::xml_node fonts = std::string(_Node.name()) == "Fonts" ? _Node : _Node.child("Fonts");

    if(fonts.empty())
        return false;

    // deserialize fonts
    m_FontSize = fonts.child("Size").empty() ? m_DefaultFontSize : fonts.child("Size").text().as_int();
    m_FontName = fonts.child("Name").text().as_string();
    m_Path     = std::filesystem::path(pugi::as_wide(fonts.child("Path").text().as_string()));

    // awake
    OnAwake();

    return true;
}
