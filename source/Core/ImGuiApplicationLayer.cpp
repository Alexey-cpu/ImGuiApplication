#include <ImGuiApplicationLayer.h>
#include <ImGuiApplication.h>

ImGuiApplicationLayer::ImGuiApplicationLayer(std::string _Name) :
    m_Name(_Name){}

ImGuiApplicationLayer::~ImGuiApplicationLayer(){}

bool ImGuiApplicationLayer::isClosed() const
{
    return !m_Opened;
}

bool ImGuiApplicationLayer::isHidden() const
{
    return !m_Shown;
}

void ImGuiApplicationLayer::Close()
{
    m_Opened = false;
}

void ImGuiApplicationLayer::Show()
{
    m_Shown = true;
}

void ImGuiApplicationLayer::Hide()
{
    m_Shown = false;
}

void ImGuiApplicationLayer::Render()
{
    if(!isHidden())
        Update();
}

void ImGuiApplicationLayer::Begin()
{
    // draw stacked modals
    for(auto it = m_RenderingQueue.begin(); it != m_RenderingQueue.end(); it++)
    {
        // remove closed layer
        if((*it)->isClosed())
        {
            auto rm = it;
            it++;
            m_RenderingQueue.erase(rm);

            // stop if there's nothing to render
            if(it == m_RenderingQueue.end())
                break;
        }

        // begin render next child layer
        (*it)->Begin();
    }
}

void ImGuiApplicationLayer::Update()
{
    for(auto it = m_RenderingQueue.begin(); it != m_RenderingQueue.end(); it++)
        (*it)->Render();
}

void ImGuiApplicationLayer::End()
{
    for(auto it = m_RenderingQueue.begin(); it != m_RenderingQueue.end(); it++)
        (*it)->End();
}
