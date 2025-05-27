#ifndef IMGUIAPPLICATIONLAYER_H
#define IMGUIAPPLICATIONLAYER_H

#include <string>
#include <memory>
#include <list>

// ImGuiApplicationLayer
class ImGuiApplicationLayer
{
public:

    // constructors
    ImGuiApplicationLayer(std::string _Name) :
        m_Name(_Name){}

    // virtual destructor
    virtual ~ImGuiApplicationLayer(){}

    // getters
    bool isClosed() const
    {
        return !m_Opened;
    }

    bool isHidden() const
    {
        return !m_Shown;
    }

    void Close()
    {
        m_Opened = false;
    }

    void Show()
    {
        m_Shown = true;
    }

    void Hide()
    {
        m_Shown = false;
    }

    // interface
    void Render()
    {
        if(!isHidden())
            Update();
    }

    virtual void Begin()
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

    virtual void Update()
    {
        for(auto it = m_RenderingQueue.begin(); it != m_RenderingQueue.end(); it++)
            (*it)->Render();
    }

    virtual void End()
    {
        for(auto it = m_RenderingQueue.begin(); it != m_RenderingQueue.end(); it++)
            (*it)->End();
    }

    template<typename __type, typename ... __parameters>
    std::shared_ptr<__type> Push(__parameters... _Parameters)
    {
        // create layer
        auto layer = std::make_shared<__type>(_Parameters ...);

        // push layer into rendering queue
        m_RenderingQueue.push_back(layer);

        // return previously create layer
        return layer;
    }

protected:

    // info
    std::string                                       m_Name           = std::string();
    bool                                              m_Opened         = true;
    bool                                              m_Shown          = true;
    std::list<std::shared_ptr<ImGuiApplicationLayer>> m_RenderingQueue = std::list<std::shared_ptr<ImGuiApplicationLayer>>();
};

#endif // IMGUIAPPLICATIONLAYER_H
