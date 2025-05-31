#ifndef IMGUIAPPLICATIONLAYER_H
#define IMGUIAPPLICATIONLAYER_H

#include <string>
#include <memory>
#include <list>

// ImGuiApplicationLayer
namespace ImGuiApplication
{
    class Layer
    {
    public:
        Layer(const std::string& _Name);
        virtual ~Layer();

        std::string getName() const;

        bool isClosed() const;
        void Close();
        void Awake();
        void Start();
        void Update();
        void Finish();
        virtual void OnClose();
        virtual void OnAwake();
        virtual void OnStart();
        virtual void OnUpdate();
        virtual void OnFinish();

        template<typename __type, typename ... __parameters>
        std::shared_ptr<__type> Push(__parameters... _Parameters)
        {
            auto layer = std::make_shared<__type>(_Parameters ...);
            m_RenderingQueue.push_back(layer);
            return layer;
        }

        template<typename __type>
        bool Contains()
        {
            return std::find_if(
                       m_RenderingQueue.begin(),
                       m_RenderingQueue.end(),
                       [](std::shared_ptr<Layer> _Layer)->bool
                       {
                           return std::dynamic_pointer_cast<__type>(_Layer) != nullptr;
                       }) != m_RenderingQueue.end();
        }

    protected:

        std::string                       m_Name           = std::string();
        bool                              m_Opened         = true;
        std::list<std::shared_ptr<Layer>> m_RenderingQueue = std::list<std::shared_ptr<Layer>>();
    };
};

#endif // IMGUIAPPLICATIONLAYER_H
