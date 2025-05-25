#ifndef IMGUIImGuiHierarchy_H
#define IMGUIImGuiHierarchy_H

#include <ImGuiApplicationRenderer.h>

// imgui
#include <imgui.h>

// STL
#include <list>
#include <string>
#include <iostream>

// ImGuiApplicationObject
class ImGuiApplicationObject : public ImGuiApplicationRenderer
{
public:

    // constructors
    ImGuiApplicationObject(
        std::string             _Name   = std::string(),
        ImGuiApplicationObject* _Parent = nullptr);

    // virtual destrucor
    virtual ~ImGuiApplicationObject();

    // getters
    ImGuiApplicationObject* get_parent() const;
    std::string get_name() const;

    // setters
    void set_parent(ImGuiApplicationObject*);
    void set_name(std::string _Name);

protected:

    // info
    std::string             m_Name   = std::string();
    ImGuiApplicationObject* m_Parent = nullptr;

    std::list<ImGuiApplicationObject*> m_Children;

    void attach_child(ImGuiApplicationObject* _Child);
    void detach_child(ImGuiApplicationObject* _Child);
};

#endif // IMGUIImGuiHierarchy_H
