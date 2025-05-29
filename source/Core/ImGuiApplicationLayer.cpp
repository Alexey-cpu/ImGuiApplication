#include <ImGuiApplicationLayer.h>
#include <ImGuiApplication.h>

ImGuiApplicationLayer::ImGuiApplicationLayer(std::string _Name) :
    m_Name(_Name){}

ImGuiApplicationLayer::~ImGuiApplicationLayer(){}

bool ImGuiApplicationLayer::isClosed() const
{
    return !m_Opened;
}

void ImGuiApplicationLayer::Close()
{
    m_Opened = false;
}

void ImGuiApplicationLayer::Awake()
{
    OnAwake();
}

void ImGuiApplicationLayer::Start()
{
    OnStart();
}

void ImGuiApplicationLayer::Update()
{
    OnUpdate();
}

void ImGuiApplicationLayer::Finish()
{
    OnFinish();
}

void ImGuiApplicationLayer::OnClose()
{
    for(auto it = m_RenderingQueue.begin(); it != m_RenderingQueue.end(); it++)
        (*it)->OnClose();
}

void ImGuiApplicationLayer::OnAwake(){}

void ImGuiApplicationLayer::OnStart()
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
        (*it)->OnStart();
    }
}

void ImGuiApplicationLayer::OnUpdate()
{
    for(auto it = m_RenderingQueue.begin(); it != m_RenderingQueue.end(); it++)
        (*it)->Update();
}

void ImGuiApplicationLayer::OnFinish()
{
    for(auto it = m_RenderingQueue.begin(); it != m_RenderingQueue.end(); it++)
        (*it)->OnFinish();
}
