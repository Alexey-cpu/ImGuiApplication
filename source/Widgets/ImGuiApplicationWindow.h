#ifndef IMGUIAPPLICATIONWINDOW_H
#define IMGUIAPPLICATIONWINDOW_H

#include <ImGuiApplicationObject.h>

class ImGuiApplicationWindow : public ImGuiApplicationObject
{
public:

    // constructors
    ImGuiApplicationWindow(
        std::string             _Name        = std::string(),
        ImGuiWindowFlags        _WindowFlags = ImGuiWindowFlags_::ImGuiWindowFlags_MenuBar,
        ImGuiChildFlags         _ChildFlags  = ImGuiChildFlags_::ImGuiChildFlags_Borders,
        ImGuiApplicationObject* _Parent      = nullptr,
        ImVec2                  _Position    = ImVec2(0.0, 0.0),
        ImVec2                  _Size        = ImVec2(800, 600));

    // virtual destructor
    virtual ~ImGuiApplicationWindow();

    // getters
    ImVec2 get_size() const;
    ImVec2 get_position() const;
    ImGuiWindowFlags get_window_flags() const;
    ImGuiChildFlags get_child_flags() const;

    // setters
    void set_size(ImVec2 _Size);
    void set_position(ImVec2 _Position);
    void set_window_flags(ImGuiWindowFlags _WindowFlags);
    void set_child_flags(ImGuiChildFlags _ChildFlags);

    // ImGuiApplicationRenderer
    virtual bool render() override;

protected:

    bool             m_Open        = true;
    ImGuiWindowFlags m_WindowFlags = ImGuiWindowFlags_::ImGuiWindowFlags_MenuBar;
    ImGuiChildFlags  m_ChildFlags  = ImGuiChildFlags_::ImGuiChildFlags_Borders;
    ImVec2           m_Position    = ImVec2(0.0, 0.0);
    ImVec2           m_Size        = ImVec2(800, 600);

    // service methods
    bool draw_children();
};

#endif // IMGUIAPPLICATIONWINDOW_H
