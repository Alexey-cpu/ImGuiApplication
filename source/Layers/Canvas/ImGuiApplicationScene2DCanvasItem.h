#ifndef IMGUIAPPLICATIONSCENE2DCANVASITEM_H
#define IMGUIAPPLICATIONSCENE2DCANVASITEM_H

// Custom
#include <ImGuiApplicationLayer.h>
#include <ImGuiApplicationSerializable.h>
#include <ImGuiApplicationScene2DCanvas.h>

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
        class Graphics : public Hierarchy
        {
        public:

            Graphics(ImVec2 _Origin, ImVec2 _Size, Hierarchy* _Parent = nullptr)
            {
                setParent(_Parent);
                setGeometry(_Origin, _Size);
            }

            virtual ~Graphics()
            {
                // remove self from the scene
            }

            virtual void Draw()
            {
                if(m_Scene == nullptr)
                    return;

                // TODO: add some stuff here...
                auto drawList = ImGui::GetWindowDrawList();

                // draw self
                m_Scene->m_DrawList->AddRect(
                    m_Scene->LocalItemPosition(m_Rect.GetTL()),
                    m_Scene->LocalItemPosition(m_Rect.GetBR()),
                    IM_COL32(255, 0, 0, 255)
                    );

                // draw children
                for(auto child : m_Children)
                {
                    Graphics* graphics =
                        dynamic_cast<Graphics*>(child);

                    if(graphics != nullptr)
                        graphics->Draw();
                }
            }

            void setGeometry(ImVec2 _Origin, ImVec2 _Size)
            {
                // setup parent-relative geometry
                m_Origin = _Origin;
                m_Size   = _Size;
                m_Rect   = ImRect(m_Origin, m_Origin + m_Size);
            }

        protected:

            Scene* m_Scene = nullptr;

            // geometry
            ImVec2 m_Origin = ImVec2();
            ImVec2 m_Size   = ImVec2();
            ImRect m_Rect   = ImRect();
        };
    };
};

#endif // IMGUIAPPLICATIONSCENE2DCANVASITEM_H
