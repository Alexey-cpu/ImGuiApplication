#ifndef IMGUIAPPLICATIONSTYLELAYER_H
#define IMGUIAPPLICATIONSTYLELAYER_H

// Custom
#include <ImGuiApplicationLayer.h>

// STL
#include <filesystem>

// imgui
#include <imgui.h>
#include <imgui_internal.h>

// pugixml
#include <pugixml.hpp>

// ImGuiApplicationStyleLayer
class ImGuiApplicationStyleLayer : public ImGuiApplicationLayer
{
public:

    // constructors
    ImGuiApplicationStyleLayer() :
        ImGuiApplicationLayer("ImGuiApplicationStyleLayer"){}

    // virtual destructor
    virtual ~ImGuiApplicationStyleLayer(){}

    // ImGuiApplicationLayer
    virtual void Start() override{}

    virtual void BeforeUpdate() override{}

    virtual void Update() override
    {
        ImGuiStyle& style = ImGui::GetStyle();

        if (ImGui::BeginTabBar("##tabs", ImGuiTabBarFlags_None))
        {
            /*
            if (ImGui::BeginTabItem("Colors"))
            {
                static int output_dest = 0;
                static bool output_only_modified = true;
                if (ImGui::Button("Export"))
                {
                    if (output_dest == 0)
                        ImGui::LogToClipboard();
                    else
                        ImGui::LogToTTY();

                    //ImGui::LogText("ImVec4* colors = ImGui::GetStyle().Colors;" IM_NEWLINE);

                    for (int i = 0; i < ImGuiCol_COUNT; i++)
                    {
                        const ImVec4& col = style.Colors[i];
                        const char* name = ImGui::GetStyleColorName(i);
                        if (!output_only_modified || memcmp(&col, &ref->Colors[i], sizeof(ImVec4)) != 0)
                            ImGui::LogText("colors[ImGuiCol_%s]%*s= ImVec4(%.2ff, %.2ff, %.2ff, %.2ff);" IM_NEWLINE,
                                           name, 23 - (int)strlen(name), "", col.x, col.y, col.z, col.w);
                    }
                    ImGui::LogFinish();
                }
                ImGui::SameLine(); ImGui::SetNextItemWidth(120); ImGui::Combo("##output_type", &output_dest, "To Clipboard\0To TTY\0");
                ImGui::SameLine(); ImGui::Checkbox("Only Modified Colors", &output_only_modified);

                static ImGuiTextFilter filter;
                filter.Draw("Filter colors", ImGui::GetFontSize() * 16);

                static ImGuiColorEditFlags alpha_flags = 0;
                if (ImGui::RadioButton("Opaque", alpha_flags == ImGuiColorEditFlags_AlphaOpaque))       { alpha_flags = ImGuiColorEditFlags_AlphaOpaque; } ImGui::SameLine();
                if (ImGui::RadioButton("Alpha",  alpha_flags == ImGuiColorEditFlags_None))              { alpha_flags = ImGuiColorEditFlags_None; } ImGui::SameLine();
                if (ImGui::RadioButton("Both",   alpha_flags == ImGuiColorEditFlags_AlphaPreviewHalf))  { alpha_flags = ImGuiColorEditFlags_AlphaPreviewHalf; } ImGui::SameLine();

                //HelpMarker(
                //    "In the color list:\n"
                //    "Left-click on color square to open color picker,\n"
                //    "Right-click to open edit options menu.");

                ImGui::SetNextWindowSizeConstraints(ImVec2(0.0f, ImGui::GetTextLineHeightWithSpacing() * 10), ImVec2(FLT_MAX, FLT_MAX));
                ImGui::BeginChild("##colors", ImVec2(0, 0), ImGuiChildFlags_Borders | ImGuiChildFlags_NavFlattened, ImGuiWindowFlags_AlwaysVerticalScrollbar | ImGuiWindowFlags_AlwaysHorizontalScrollbar);
                ImGui::PushItemWidth(ImGui::GetFontSize() * -12);
                for (int i = 0; i < ImGuiCol_COUNT; i++)
                {
                    const char* name = ImGui::GetStyleColorName(i);
                    if (!filter.PassFilter(name))
                        continue;
                    ImGui::PushID(i);
#ifndef IMGUI_DISABLE_DEBUG_TOOLS
                    if (ImGui::Button("?"))
                        ImGui::DebugFlashStyleColor((ImGuiCol)i);
                    ImGui::SetItemTooltip("Flash given color to identify places where it is used.");
                    ImGui::SameLine();
#endif
                    ImGui::ColorEdit4("##color", (float*)&style.Colors[i], ImGuiColorEditFlags_AlphaBar | alpha_flags);
                    if (memcmp(&style.Colors[i], &ref->Colors[i], sizeof(ImVec4)) != 0)
                    {
                        // Tips: in a real user application, you may want to merge and use an icon font into the main font,
                        // so instead of "Save"/"Revert" you'd use icons!
                        // Read the FAQ and docs/FONTS.md about using icon fonts. It's really easy and super convenient!
                        ImGui::SameLine(0.0f, style.ItemInnerSpacing.x); if (ImGui::Button("Save")) { ref->Colors[i] = style.Colors[i]; }
                        ImGui::SameLine(0.0f, style.ItemInnerSpacing.x); if (ImGui::Button("Revert")) { style.Colors[i] = ref->Colors[i]; }
                    }
                    ImGui::SameLine(0.0f, style.ItemInnerSpacing.x);
                    ImGui::TextUnformatted(name);
                    ImGui::PopID();
                }
                ImGui::PopItemWidth();
                ImGui::EndChild();

                ImGui::EndTabItem();
            }
            */

            if (ImGui::BeginTabItem("Fonts"))
            {
                ImGuiIO& io = ImGui::GetIO();
                ImFontAtlas* atlas = io.Fonts;
                ShowFontAtlas(atlas);
                ImGui::EndTabItem();
            }

            ImGui::EndTabBar();
        }
    }

    virtual void AfterUpdate() override{}

    void ShowFontAtlas(ImFontAtlas* atlas)
    {
        ImGuiStyle& style = ImGui::GetStyle();

        //ImGui::Text("Read ");
        //ImGui::SameLine(0, 0);
        //ImGui::TextLinkOpenURL("https://www.dearimgui.com/faq/");
        //ImGui::SameLine(0, 0);
        //ImGui::Text(" for details on font loading.");
        //ImGuiMetricsConfig* cfg = &g.DebugMetricsConfig;
        //ImGui::Checkbox("Show font preview", &cfg->ShowFontPreview);

        // Font list
        for (ImFont* font : atlas->Fonts)
        {
            ImGui::PushID(font);
            DebugNodeFont(font);
            ImGui::PopID();
        }

        //if (ImGui::TreeNode("Font Atlas", "Font Atlas (%dx%d pixels)", atlas->TexWidth, atlas->TexHeight))
        //{
        //    ImGui::PushStyleVar(ImGuiStyleVar_ImageBorderSize, std::max(1.0f, style.ImageBorderSize));
        //    ImGui::ImageWithBg(atlas->TexID, ImVec2((float)atlas->TexWidth, (float)atlas->TexHeight), ImVec2(0.0f, 0.0f), ImVec2(1.0f, 1.0f), ImVec4(0.0f, 0.0f, 0.0f, 1.0f));
        //    ImGui::PopStyleVar();
        //    ImGui::TreePop();
        //}
    }

    void DebugNodeFont(ImFont* font)
    {
        //ImGuiContext& g = *GImGui;
        //ImGuiMetricsConfig* cfg = &g.DebugMetricsConfig;
        if(ImGui::TreeNode(font, "Font: \"%s\": %.2f px, %d glyphs, %d sources(s)", font->Sources ? font->Sources[0].Name : "", font->FontSize, font->Glyphs.Size, font->SourcesCount))
        {
            ImGui::PushFont(font);
            ImGui::SeparatorText("The quick brown fox jumps over the lazy dog");
            ImGui::PopFont();

            if (ImGui::Button("Select this font"))
            {
                ImGui::GetIO().FontDefault = font;

                // TODO: push a style into fonts layer...
            }

            // Display details
            ImGui::SameLine();
            ImGui::SetNextItemWidth(ImGui::GetFontSize() * 8);
            ImGui::DragFloat("Font scale", &font->Scale, 0.005f, 0.3f, 2.0f, "%.1f");
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

        //ImGui::TreePop();
        //ImGui::Unindent();
    }

};

#endif // IMGUIAPPLICATIONSTYLELAYER_H
