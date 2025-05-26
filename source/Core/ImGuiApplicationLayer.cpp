#include <ImGuiApplicationLayer.h>
#include <ImGuiApplication.h>

void ImGuiApplicationLayer::Render()
{
    Begin();

    Update();

    End();
}
