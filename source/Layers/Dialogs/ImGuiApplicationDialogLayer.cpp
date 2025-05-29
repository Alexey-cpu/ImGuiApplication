#include <ImGuiApplicationDialogLayer.h>

// constructor
ImGuiApplicationDialogLayer::ImGuiApplicationDialogLayer(std::string _Title) :
    ImGuiApplicationLayer(_Title){}

ImGuiApplicationDialogLayer::~ImGuiApplicationDialogLayer(){}

bool ImGuiApplicationDialogLayer::isAccepted() const
{
    return m_DialogState == ImGuiApplicationDialogState::Accepted;
}

bool ImGuiApplicationDialogLayer::isCanceled() const
{
    return m_DialogState == ImGuiApplicationDialogState::Canceled;
}

bool ImGuiApplicationDialogLayer::isUndefined() const
{
    return m_DialogState == ImGuiApplicationDialogState::Undefined;
}

void ImGuiApplicationDialogLayer::OnUpdate()
{
    ImGuiStyle& style = ImGui::GetStyle();

    // press button
    ImGui::OpenPopup(m_Name.c_str());

    // start pop-up
    if(ImGui::BeginPopupModal(
            m_Name.c_str(),
            &m_Opened,
            ImGuiWindowFlags_::ImGuiWindowFlags_None | ImGuiWindowFlags_::ImGuiWindowFlags_NoScrollbar))
    {
        // draw stacked modals
        for(auto it = m_RenderingQueue.begin(); it != m_RenderingQueue.end(); it++)
            (*it)->Update();

        ImGui::BeginChild(
            "Content",
            ImVec2(ImGui::GetContentRegionAvail().x, ImGui::GetContentRegionAvail().y - (ImGui::CalcTextSize("Button").y + style.FramePadding.x * 2.0f + m_ButtonsSpacing * 2.0f)),
            ImGuiChildFlags_::ImGuiChildFlags_Borders,
            ImGuiWindowFlags_::ImGuiWindowFlags_None | ImGuiWindowFlags_::ImGuiWindowFlags_NoScrollbar);

        DrawContent();

        ImGui::EndChild();

        ImGui::BeginChild(
            "Buttons",
            ImVec2(ImGui::GetContentRegionAvail().x, ImGui::GetContentRegionAvail().y),
            ImGuiChildFlags_::ImGuiChildFlags_Borders,
            ImGuiWindowFlags_::ImGuiWindowFlags_None | ImGuiWindowFlags_::ImGuiWindowFlags_NoScrollbar);

        DrawButtons();

        ImGui::EndChild();

        ImGui::EndPopup();
    }
}
