#include <ImGuiApplicationObject.h>
#include <ImGuiApplication.h>

// constructors
ImGuiApplicationObject::ImGuiApplicationObject(std::string _ID, ImGuiApplicationObject* _Parent) : m_Name(_ID)
{
    set_parent(_Parent);
}

ImGuiApplicationObject::~ImGuiApplicationObject()
{
    // detach self from parent
    if(m_Parent != nullptr)
        m_Parent->detach_child(this);
    else
        ImGuiApplication::Instance()->pop_from_rendering_queue(this);

    // remove all children
    if(m_Children.empty())
        return;

    auto children = m_Children;

    for(auto child : children)
    {
        if(child != nullptr)
            delete child;
    }

    m_Children.clear();
}

ImGuiApplicationObject* ImGuiApplicationObject::get_parent() const
{
    return m_Parent;
}

std::string ImGuiApplicationObject::get_name() const
{
    return m_Name;
}

void ImGuiApplicationObject::set_parent(ImGuiApplicationObject* _Parent)
{
    // detach self from parent
    if(m_Parent != nullptr)
        m_Parent->detach_child(this);

    // attach self to a new parent
    m_Parent = _Parent;

    if(m_Parent != nullptr)
        m_Parent->attach_child(this);
    else
        ImGuiApplication::Instance()->push_to_rendering_queue(this);
}

void ImGuiApplicationObject::set_name(std::string _Name)
{
    m_Name = _Name;
}

void ImGuiApplicationObject::attach_child(ImGuiApplicationObject* _Child)
{
    if(m_Children.empty() ||
        std::find(m_Children.begin(), m_Children.end(), _Child) == m_Children.end())
        m_Children.push_back(_Child);
}

void ImGuiApplicationObject::detach_child(ImGuiApplicationObject* _Child)
{
    if(m_Children.empty())
        return;

    auto iterator =
        std::find(m_Children.begin(), m_Children.end(), _Child);

    if(iterator != m_Children.end())
        m_Children.erase(iterator);
}
