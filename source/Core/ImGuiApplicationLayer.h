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
    ImGuiApplicationLayer(std::string _Name);

    // virtual destructor
    virtual ~ImGuiApplicationLayer();

    // getters
    bool isClosed() const;
    void Close();

    void Awake();
    void Start();
    void Update();
    void Finish();

    // virtual functions to override
    virtual void OnClose();
    virtual void OnAwake();
    virtual void OnStart();
    virtual void OnUpdate();
    virtual void OnFinish();

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

    template<typename __type>
    bool Contains()
    {
        return std::find_if(
                   m_RenderingQueue.begin(),
                   m_RenderingQueue.end(),
                   [](std::shared_ptr<ImGuiApplicationLayer> _Layer)->bool
                   {
                       return std::dynamic_pointer_cast<__type>(_Layer) != nullptr;
                   }) != m_RenderingQueue.end();
    }

protected:

    // info
    std::string                                       m_Name           = std::string();
    bool                                              m_Opened         = true;
    std::list<std::shared_ptr<ImGuiApplicationLayer>> m_RenderingQueue = std::list<std::shared_ptr<ImGuiApplicationLayer>>();
};

#endif // IMGUIAPPLICATIONLAYER_H
