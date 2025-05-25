#ifndef IMGUIAPPLICATIONRENDERER_H
#define IMGUIAPPLICATIONRENDERER_H

// ImGuiApplicationRenderer
class ImGuiApplicationRenderer
{
public:

    // constructors
    ImGuiApplicationRenderer(){}

    // virtual destructor
    virtual ~ImGuiApplicationRenderer(){}

    // interface
    virtual bool render() = 0;
};

#endif // IMGUIAPPLICATIONRENDERER_H
