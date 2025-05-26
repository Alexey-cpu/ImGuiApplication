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

    // interface
    virtual void Begin() {}
    virtual void Update(){}
    virtual void End()   {}
};

#endif // IMGUIAPPLICATIONLAYER_H
