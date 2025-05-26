#ifndef IMGUIAPPLICATIONLAYER_H
#define IMGUIAPPLICATIONLAYER_H

// ImGuiApplicationLayer
class ImGuiApplicationLayer
{
public:

    // constructors
    ImGuiApplicationLayer(){}

    // virtual destructor
    virtual ~ImGuiApplicationLayer(){}

    // getters
    bool isClosed() const
    {
        return !m_Opened;
    }

    // interface
    virtual void Begin() {}
    virtual void Update(){}
    virtual void End()   {}

    void Render();

protected:

    bool m_Opened = true;
};

#endif // IMGUIAPPLICATIONLAYER_H
