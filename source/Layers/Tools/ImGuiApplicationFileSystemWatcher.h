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
        ImGuiApplicationLayer("ImGuiApplicationFileSystemWatcher")
    {
        std::cout << "ImGuiApplicationFileSystemWatcher() \n";
    }

    virtual ~ImGuiApplicationFileSystemWatcher(){}

    virtual void Update() override
    {
        // do nothing here
        if(m_CurrentPath == std::filesystem::current_path())
            return;

        std::cout << "builds document... \n";

        // rebuild document
        m_Document.remove_children();

        auto path = m_Document.append_child("PATH");
        path.append_attribute("PATH").set_value(pugi::as_utf8(std::filesystem::current_path().parent_path().make_preferred().wstring().c_str()));

        for(const auto& directoryEntry :
             std::filesystem::directory_iterator(std::filesystem::current_path().make_preferred()))
        {
            auto folder = path.append_child((directoryEntry.is_directory() ? "FOLDER" : "FILE"));

            folder.append_attribute("PATH").set_value(pugi::as_utf8(directoryEntry.path().wstring().c_str()));

            if(directoryEntry.is_directory())
            {
                folder.append_attribute("FOLDER").set_value(pugi::as_utf8(directoryEntry.path().filename().wstring().c_str()));
            }
            else
            {
                folder.append_attribute("FILE").set_value(pugi::as_utf8(directoryEntry.path().filename().wstring().c_str()));
                folder.append_attribute("EXTENTION").set_value(pugi::as_utf8(directoryEntry.path().extension().wstring().c_str()));
            }

            auto tp = std::filesystem::last_write_time(directoryEntry.path());
            time_t secs = std::chrono::duration_cast<std::chrono::seconds>(tp.time_since_epoch()).count();

            folder.append_attribute("LAST_WRITE_TIME").set_value(pugi::as_utf8(pugi::as_wide(asctime(std::localtime(&secs)))));
        }

        m_CurrentPath = std::filesystem::current_path();

        // debug output
        m_Document.save_file("C:\\SDK\\Qt_Projects\\ImGuiRenderExplore\\logs\\current_path.xml");
    }

    pugi::xml_document& GetDocument()
    {
        return m_Document;
    }

protected:

    // info
    std::filesystem::path m_CurrentPath;
    pugi::xml_document    m_Document;
};

#endif // IMGUIAPPLICATIONFILESYSTEMWATCHER_H
