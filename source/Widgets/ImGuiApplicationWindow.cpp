#include <ImGuiApplicationWindow.h>

ImGuiApplicationWindow::ImGuiApplicationWindow(
    std::string             _Name,
    ImGuiWindowFlags        _WindowFlags,
    ImGuiChildFlags         _ChildFlags,
    ImGuiApplicationObject* _Parent,
    ImVec2                  _Position,
    ImVec2                  _Size) :
    ImGuiApplicationObject(_Name, _Parent),
    m_WindowFlags(_WindowFlags),
    m_ChildFlags(_ChildFlags),
    m_Position(_Position),
    m_Size(_Size){}

ImGuiApplicationWindow::~ImGuiApplicationWindow(){}

ImVec2 ImGuiApplicationWindow::get_size() const
{
    return m_Size;
}

ImVec2 ImGuiApplicationWindow::get_position() const
{
    return m_Position;
}

ImGuiWindowFlags ImGuiApplicationWindow::get_window_flags() const
{
    return m_WindowFlags;
}

ImGuiChildFlags ImGuiApplicationWindow::get_child_flags() const
{
    return m_ChildFlags;
}

void ImGuiApplicationWindow::set_size(ImVec2 _Size)
{
    m_Size = _Size;
}

void ImGuiApplicationWindow::set_position(ImVec2 _Position)
{
    m_Position = _Position;
}

void ImGuiApplicationWindow::set_window_flags(ImGuiWindowFlags _WindowFlags)
{
    m_WindowFlags = _WindowFlags;
}

void ImGuiApplicationWindow::set_child_flags(ImGuiChildFlags _ChildFlags)
{
    m_ChildFlags = _ChildFlags;
}

bool ImGuiApplicationWindow::render()
{
    if(get_parent() != nullptr)
        return draw_children();

    if(ImGui::Begin(m_Name.c_str(), &m_Open, m_WindowFlags))
        draw_children();
    ImGui::End();

    return true;
}

bool ImGuiApplicationWindow::draw_children()
{
    for(auto child : m_Children)
    {
        ImGuiApplicationWindow* window =
            dynamic_cast<ImGuiApplicationWindow*>(child);

        if(window == nullptr)
            continue;

        if(ImGui::BeginChild(
                (child->get_name() + std::to_string((size_t)(child))).c_str(),
                window->get_size(),
                m_ChildFlags,
                m_WindowFlags))
        {
            child->render();
        }

        ImGui::EndChild();
    }

    return true;
}
