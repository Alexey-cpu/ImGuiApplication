#ifndef IMGUIAPPLICATIONDIALOG_H
#define IMGUIAPPLICATIONDIALOG_H

// Custom
#include <ImGuiApplicationLayer.h>

// STL
#include <string>

// imgui
#include <imgui.h>

// ImGuiApplication
namespace ImGuiApplication
{
    namespace Dialogs
    {
        enum State
        {
            Undefined,
            Accepted,
            Canceled
        };

        class Dialog : public ImGuiApplication::Layer
        {
        public:

            Dialog(std::string _Title);
            virtual ~Dialog();

            bool isAccepted() const;
            bool isCanceled() const;
            bool isUndefined() const;
            virtual void OnUpdate() override;
            virtual void DrawContent() = 0;
            virtual void DrawButtons() = 0;

        protected:
            float m_ButtonsSpacing = 8;
            State m_DialogState    = State::Undefined;
        };
    };
};

#endif // IMGUIAPPLICATIONDIALOG_H
