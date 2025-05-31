#include <ImGuiApplicationLayer.h>
#include <ImGuiApplication.h>

using namespace ImGuiApplication;

Layer::Layer(const std::string& _Name) :
    m_Name(_Name){}

Layer::~Layer(){}

std::string Layer::getName() const
{
    return m_Name;
}

bool Layer::isClosed() const
{
    return !m_Opened;
}

void Layer::Close()
{
    m_Opened = false;
}

void Layer::Awake()
{
    OnAwake();
}

void Layer::Start()
{
    OnStart();
}

void Layer::Update()
{
    OnUpdate();
}

void Layer::Finish()
{
    OnFinish();
}

void Layer::OnClose()
{
    for(auto it = m_RenderingQueue.begin(); it != m_RenderingQueue.end(); it++)
        (*it)->OnClose();
}

void Layer::OnAwake()
{
    for(auto it = m_RenderingQueue.begin(); it != m_RenderingQueue.end(); it++)
        (*it)->Awake();
}

void Layer::OnStart()
{
    // draw stacked modals
    for(auto it = m_RenderingQueue.begin(); it != m_RenderingQueue.end(); it++)
    {
        // remove closed Layer
        if((*it)->isClosed())
        {
            auto rm = it;
            it++;
            m_RenderingQueue.erase(rm);

            // stop if there's nothing to render
            if(it == m_RenderingQueue.end())
                break;
        }

        // begin render next child Layer
        (*it)->OnStart();
    }
}

void Layer::OnUpdate()
{
    for(auto it = m_RenderingQueue.begin(); it != m_RenderingQueue.end(); it++)
        (*it)->Update();
}

void Layer::OnFinish()
{
    for(auto it = m_RenderingQueue.begin(); it != m_RenderingQueue.end(); it++)
        (*it)->Finish();
}
