#include <ImGuiApplicationSettingsColors.h>

using namespace ImGuiApplication::Settings;

Colors::Colors() : Layer("Colors"){}

Colors::~Colors(){}

void Colors::OnClose(){}

void Colors::OnAwake(){}

void Colors::OnStart(){}

void Colors::OnUpdate()
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

void Colors::OnFinish(){}

pugi::xml_node Colors::Serialize(pugi::xml_node& _Parent)
{
    (void)_Parent;
    return pugi::xml_node();
}

bool Colors::Deserialize(pugi::xml_node& _Node)
{
    (void)_Node;
    return true;
}
