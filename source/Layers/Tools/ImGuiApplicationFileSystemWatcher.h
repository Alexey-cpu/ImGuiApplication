#ifndef IMGUIAPPLICATIONFILESYSTEMWATCHER_H
#define IMGUIAPPLICATIONFILESYSTEMWATCHER_H

// Custom
#include <ImGuiApplicationLayer.h>

// STL
#include <filesystem>

// pugixml
#include <pugixml.hpp>
#include <iostream>

// ImGuiApplicationFileSystemWatcher
class ImGuiApplicationFileSystemWatcher : public ImGuiApplicationLayer
{
public:

    //
    ImGuiApplicationFileSystemWatcher() :
        ImGuiApplicationLayer("ImGuiApplicationFileSystemWatcher"){}

    virtual ~ImGuiApplicationFileSystemWatcher(){}

    virtual void OnStart() override
    {
        // do nothing here
        if(m_CurrentPath == std::filesystem::current_path())
            return;
    }

protected:

    // info
    std::filesystem::path m_CurrentPath;
};

#endif // IMGUIAPPLICATIONFILESYSTEMWATCHER_H
