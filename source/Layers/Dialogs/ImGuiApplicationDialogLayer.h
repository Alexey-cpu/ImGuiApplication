#ifndef IMGUIAPPLICATIONDIALOGLAYER_H
#define IMGUIAPPLICATIONDIALOGLAYER_H

// Custom
#include <ImGuiApplicationLayer.h>

// STL
#include <string>

// imgui
#include <imgui.h>

// ImGuiApplicationDialogState
enum ImGuiApplicationDialogState
{
    Undefined,
    Accepted,
    Canceled
};

// ImGuiApplicationDialogLayer
class ImGuiApplicationDialogLayer : public ImGuiApplicationLayer
{
public:

    // constructor
    ImGuiApplicationDialogLayer(std::string _Title);

    // virtual destructor
    virtual ~ImGuiApplicationDialogLayer();

    bool isAccepted() const;
    bool isCanceled() const;
    bool isUndefined() const;

    // ImGuiApplicationLayer
    virtual void OnUpdate() override;
    virtual void DrawContent() = 0;
    virtual void DrawButtons() = 0;

protected:

    // info
    float                       m_ButtonsSpacing = 8;
    ImGuiApplicationDialogState m_DialogState    = ImGuiApplicationDialogState::Undefined;
};

#endif // IMGUIAPPLICATIONDIALOGLAYER_H
