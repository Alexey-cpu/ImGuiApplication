#ifndef IMGUIAPPLICATIONSETTINGSCOLORS_H
#define IMGUIAPPLICATIONSETTINGSCOLORS_H

// Custom
#include <ImGuiApplication.h>
#include <ImGuiApplicationSerializable.h>

// imgui
#include <imgui.h>
#include <imgui_internal.h>

namespace ImGuiApplication::Settings
{
    class Colors : public Layer, public IXSerializable
    {
    public:

        Colors();
        virtual ~Colors();
        virtual void OnClose() override;
        virtual void OnAwake() override;
        virtual void OnStart() override;
        virtual void OnUpdate() override;
        virtual void OnFinish() override;
        virtual pugi::xml_node Serialize(pugi::xml_node& _Parent) override;
        virtual bool Deserialize(pugi::xml_node& _Node) override;

    protected:
        ImGuiStyle            m_ReferenceStyle;
        ImGuiTextFilter       m_ColorsTextFilter;
        ImGuiColorEditFlags   m_ColorsAlphaFlags = 0;
    };
}

#endif // IMGUIAPPLICATIONSETTINGSCOLORS_H
