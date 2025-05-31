#include <ImGuiApplicationDialog.h>

using namespace ImGuiApplication::Dialogs;

// constructor
Dialog::Dialog(std::string _Title) :
    Layer(_Title){}

Dialog::~Dialog(){}

bool Dialog::isAccepted() const
{
    return m_DialogState == State::Accepted;
}

bool Dialog::isCanceled() const
{
    return m_DialogState == State::Canceled;
}

bool Dialog::isUndefined() const
{
    return m_DialogState == State::Undefined;
}

void Dialog::OnUpdate()
{
    ImGuiStyle& style = ImGui::GetStyle();

    // press button
    ImGui::OpenPopup(m_Name.c_str());

    auto wiondowFlags =
        ImGuiWindowFlags_::ImGuiWindowFlags_None | ImGuiWindowFlags_::ImGuiWindowFlags_NoScrollbar;

    // start pop-up
    if(ImGui::BeginPopupModal(
            m_Name.c_str(),
            &m_Opened,
            wiondowFlags))
    {
        // draw stacked modals
        for(auto it = m_RenderingQueue.begin(); it != m_RenderingQueue.end(); it++)
            (*it)->Update();

        ImGui::BeginChild(
            "Content",
            ImVec2(ImGui::GetContentRegionAvail().x, ImGui::GetContentRegionAvail().y - (ImGui::CalcTextSize("Button").y + style.FramePadding.x * 2.0f + m_ButtonsSpacing * 2.0f)),
            ImGuiChildFlags_::ImGuiChildFlags_Borders,
            wiondowFlags);

        DrawContent();

        ImGui::EndChild();

        ImGui::BeginChild(
            "Buttons",
            ImVec2(ImGui::GetContentRegionAvail().x, ImGui::GetContentRegionAvail().y),
            ImGuiChildFlags_::ImGuiChildFlags_Borders,
            wiondowFlags);

        DrawButtons();

        ImGui::EndChild();

        ImGui::EndPopup();
    }
}
