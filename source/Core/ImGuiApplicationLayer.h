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
    bool isClosed() const;
    bool isHidden() const;
    void Close();
    void Show();
    void Hide();
    void Render();

    // virtual functions to override
    virtual void Begin();
    virtual void Update();
    virtual void End();

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
