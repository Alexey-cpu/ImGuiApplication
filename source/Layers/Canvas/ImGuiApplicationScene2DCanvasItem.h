#ifndef IMGUIAPPLICATIONSCENE2DCANVASITEM_H
#define IMGUIAPPLICATIONSCENE2DCANVASITEM_H

// Custom
#include <ImGuiApplicationLayer.h>
#include <ImGuiApplicationSerializable.h>

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
        class Item
        {
        public:

            Item(ImVec2 _Origin, ImVec2 _Size, Item* _Parent = nullptr)
            {
                setParent(_Parent);
                setGeometry(_Origin, _Size);
            }

            virtual ~Item()
            {
                // remove all children
                auto children = m_Children;

                for(auto child : children)
                {
                    if(child != nullptr)
                        delete child;
                }

                // clear children list
                m_Children.clear();
            }

            virtual void Draw()
            {
                // TODO: add some stuff here...
                auto drawList = ImGui::GetWindowDrawList();

                // draw self

                // draw children
                for(auto child : m_Children)
                {
                    if(child != nullptr)
                        child->Draw();
                }
            }

            void setParent(Item* _Parent)
            {
                // detach self from previous parent
                if(m_Parent != nullptr)
                    m_Parent->detachChild(this);

                // attach self to a new parent
                m_Parent = _Parent;
                if(m_Parent != nullptr)
                    m_Parent->attachChild(this);
            }

            void setGeometry(ImVec2 _Origin, ImVec2 _Size)
            {
                m_Origin = _Origin + (m_Parent != nullptr ? m_Parent->m_Origin : ImVec2(0.f, 0.f));
                m_Size   = _Size;
                m_Rect   = ImRect(m_Origin, m_Origin + m_Size);
            }

        protected:

            // geometry
            ImVec2      m_Origin = ImVec2();
            ImVec2      m_Size   = ImVec2();
            ImRect      m_Rect   = ImRect();
            ImVec2      m_Offset = ImVec2(0.f, 0.f);
            float       m_Scale  = 1.f;

            // hierarchy
            Item* m_Parent   = nullptr;

            std::list<Item*> m_Children =
                std::list<Item*>();

            // service methods
            void detachChild(Item* _Child)
            {
                auto iterator = std::find(m_Children.begin(), m_Children.end(), _Child);

                if(iterator != m_Children.end())
                    m_Children.erase(iterator);
            }

            void attachChild(Item* _Child)
            {
                if(_Child != nullptr)
                    m_Children.push_back(_Child);
            }
        };
    };
};

#endif // IMGUIAPPLICATIONSCENE2DCANVASITEM_H
