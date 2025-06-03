#include <ImGuiApplicationScene2DCanvas.h>
#include <ImGuiApplicationScene2DCanvasItem.h>

using namespace ImGuiApplication::Scene2D;

void Scene::OnUpdate()
{
    ImGui::Begin("Canvas2D");

    m_Origin = ImGui::GetCursorScreenPos();

    m_Size = ImVec2(
        std::max(ImGui::GetContentRegionAvail().x, m_GridSize),
        std::max(ImGui::GetContentRegionAvail().y, m_GridSize));

    m_Rect = ImRect(m_Origin, m_Origin + m_Size);

    // get draw list
    m_DrawList = ImGui::GetWindowDrawList();

    // push clipping rect
    m_DrawList->PushClipRect(m_Rect.GetTL(), m_Rect.GetBR(), true);

    // draw children
    CatchMouseButtons();
    DrawBackground();

    for(auto& itemToDraw : m_ItemsToDraw)
    {
        Graphics* graphics =
            dynamic_cast<Graphics*>(itemToDraw);

        if(graphics != nullptr)
            graphics->Draw();
    }

    // pop clipping rect
    m_DrawList->PopClipRect();

    ImGui::End();
}
