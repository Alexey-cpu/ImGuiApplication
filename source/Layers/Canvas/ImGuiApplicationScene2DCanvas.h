#ifndef IMGUIAPPLICATIONSCENE2DCANVAS_H
#define IMGUIAPPLICATIONSCENE2DCANVAS_H

// Custom
#include <ImGuiApplicationLayer.h>
#include <ImGuiApplicationSerializable.h>
#include <ImGuiApplicationScene2DCanvasItemHierarchy.h>

// imgui
# ifndef IMGUI_DEFINE_MATH_OPERATORS
#     define IMGUI_DEFINE_MATH_OPERATORS
# endif
#include <imgui.h>
#include <imgui_internal.h>

// STL
#include <filesystem>
#include <iostream>

namespace ImGuiApplication
{
    namespace Scene2D
    {
        class Scene : public Layer
        {
        public:

            Scene() : Layer("Scene2D"){}

            virtual ~Scene(){}

            virtual void OnUpdate() override;

            ImVec2 LocalItemPosition(ImVec2 _Position)
            {
                return _Position * m_Scale + m_Origin + m_Offset;
            };

            ImVec2 LocalMousePostion(ImVec2 _Position)
            {
                return (_Position - m_Origin - m_Offset) / m_Scale;
            };

            ImVec2      m_Origin   = ImVec2();
            ImVec2      m_Size     = ImVec2();
            ImRect      m_Rect     = ImRect();
            ImVec2      m_Offset   = ImVec2(0.f, 0.f);
            ImDrawList* m_DrawList = nullptr;
            float       m_GridSize = 32.f;
            float       m_Scale    = 1.f;

            std::list<Hierarchy*> m_ItemsToDraw =
                std::list<Hierarchy*>();

            void DrawBackground()
            {
                if(m_DrawList == nullptr)
                    return;

                // draw background color
                m_DrawList->AddRectFilled(
                    m_Rect.GetTL(),
                    m_Rect.GetBR(),
                    IM_COL32(64, 64, 64, 255));

                // draw border
                m_DrawList->AddRect(
                    m_Rect.GetTL(),
                    m_Rect.GetBR(),
                    IM_COL32(0, 255, 0, 255));

                auto offset = m_Offset;

                // draw vertical grid lines
                auto gridSize = m_GridSize * m_Scale;

                for (float x = fmodf(offset.x, gridSize); x < m_Size.x; x += gridSize)
                {
                    m_DrawList->AddLine(
                        ImVec2(
                            m_Rect.GetTL().x + x, m_Rect.GetTL().y),
                            ImVec2(m_Rect.GetTL().x + x, m_Rect.GetBR().y),
                            IM_COL32(200, 200, 200, 40));
                }

                // draw horizontal grid lines
                for (float y = fmodf(offset.y, gridSize); y < m_Size.y; y += gridSize)
                {
                    m_DrawList->AddLine(
                        ImVec2(
                            m_Rect.GetTL().x, m_Rect.GetTL().y + y),
                            ImVec2(m_Rect.GetBR().x, m_Rect.GetTL().y + y),
                            IM_COL32(200, 200, 200, 40));
                }
            }

            void CatchMouseButtons()
            {
                ImGuiIO& io = ImGui::GetIO();

                ImGui::InvisibleButton(
                    "Scene2D",
                    m_Size,
                    ImGuiButtonFlags_MouseButtonMask_);

                // drag
                if (ImGui::IsMouseDragging(ImGuiMouseButton_Middle))
                {
                    m_Offset.x += io.MouseDelta.x;
                    m_Offset.y += io.MouseDelta.y;
                }

                // zoom
                if (io.MouseWheel != 0)
                    m_Scale = ImClamp(m_Scale + io.MouseWheel * m_Scale / 16.f, 0.0001f, 100.f);
            }

            void DrawDebugLine()
            {
                if(m_DrawList == nullptr)
                    return;

                m_DrawList->AddRect(
                    LocalItemPosition(m_Rect.GetCenter() - ImVec2(128, 128) * 0.5),
                    LocalItemPosition(m_Rect.GetCenter() + ImVec2(128, 128)),
                    IM_COL32(0, 255, 0, 255));

                m_DrawList->AddRect(
                    LocalItemPosition(ImVec2(128, 128)),
                    LocalItemPosition(ImVec2(128, 128) + ImVec2(128, 128)),
                    IM_COL32(255, 0, 0, 255));

                m_DrawList->AddRect(
                    LocalItemPosition(ImVec2(-512, -512)),
                    LocalItemPosition(ImVec2(-512, -512) + ImVec2(128, 128)),
                    IM_COL32(255, 0, 0, 255));
            }

            void DrawText()
            {
                if(m_DrawList == nullptr)
                    return;

                auto mousePosition = LocalMousePostion(ImGui::GetIO().MousePos);

                m_DrawList->AddText(
                    ImGui::GetIO().MousePos,
                    IM_COL32(0, 255, 0, 255),
                    (std::to_string(mousePosition.x) + " " + std::to_string(mousePosition.y)).c_str());
            }

        };
    }
}

#endif // IMGUIAPPLICATIONSCENE2DCANVAS_H
