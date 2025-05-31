#include <ImGuiApplicationSettings.h>

#include <iostream>

using namespace ImGuiApplication::Settings;

Settings::Settings(const std::string&                       _Title,
                   const std::filesystem::path&             _Path,
                   const std::list<std::shared_ptr<Layer>>& _Settings) : Layer(_Title)
{
    m_Path           = _Path;
    m_RenderingQueue = _Settings;

    if(!m_RenderingQueue.empty())
        m_CurrentLayer = m_RenderingQueue.front();
}

Settings::~Settings(){}

void Settings::OnClose()
{
    Save();
}

void Settings::OnAwake()
{
    // look for .xml settings file
    for(const auto& directory :
         std::filesystem::recursive_directory_iterator(m_Path, std::filesystem::directory_options::skip_permission_denied))
    {
        if(directory.is_directory())
            continue;

        if(directory.path().extension() == ".xml" &&
            pugi::as_utf8(directory.path().filename().stem().wstring()) == "Settings")
        {
            std::cout << "Settings::OnAwake() " << directory.path().string() << "\n";

            pugi::xml_document document;
            document.load_file(pugi::as_utf8(directory.path().wstring()).c_str());
            Deserialize(document);
        }
    }
}

void Settings::OnUpdate()
{
    ImGui::Begin(m_Name.c_str());

    if(ImGui::BeginTable(
            "FileSystemDialogSplitter",
            2,
            ImGuiTableFlags_ScrollY          |
                ImGuiTableFlags_RowBg        |
                ImGuiTableFlags_BordersOuter |
                ImGuiTableFlags_BordersV     |
                ImGuiTableFlags_Resizable    |
                ImGuiTableFlags_Reorderable  |
                ImGuiTableFlags_Hideable))
    {

        ImGui::TableSetupColumn("Name", ImGuiTableColumnFlags_::ImGuiTableColumnFlags_WidthStretch | ImGuiTableColumnFlags_::ImGuiTableColumnFlags_PreferSortAscending);
        ImGui::TableSetupColumn("Content", ImGuiTableColumnFlags_::ImGuiTableColumnFlags_WidthFixed | ImGuiTableColumnFlags_::ImGuiTableColumnFlags_PreferSortAscending);

        if(std::find(m_RenderingQueue.begin(), m_RenderingQueue.end(), m_CurrentLayer) == m_RenderingQueue.end())
        {
            if(!m_RenderingQueue.empty())
                m_CurrentLayer = m_RenderingQueue.front();
        }

        ImGui::TableNextRow();
        ImGui::TableSetColumnIndex(0);

        if(ImGui::BeginTable(
                "FileSystemDialogSplitter",
                1,
                ImGuiTableFlags_ScrollY          |
                    ImGuiTableFlags_RowBg        |
                    ImGuiTableFlags_Resizable    |
                    ImGuiTableFlags_Reorderable  |
                    ImGuiTableFlags_Hideable))
        {
            for(auto& item : m_RenderingQueue)
            {
                ImGui::TableNextRow();
                ImGui::TableSetColumnIndex(0);

                if(ImGui::Selectable(
                        item->getName().c_str(),
                        m_CurrentLayer == item,
                        ImGuiSelectableFlags_::ImGuiSelectableFlags_AllowOverlap))
                    m_CurrentLayer = item;
            }

            ImGui::EndTable();
        }

        // draw content column
        ImGui::TableSetColumnIndex(1);

        if(m_CurrentLayer != nullptr)
        {
            ImGui::BeginChild(m_CurrentLayer->getName().c_str(), ImVec2(0.0, 0.0));

            m_CurrentLayer->Update();

            ImGui::EndChild();
        }

        ImGui::EndTable();
    }

    ImGui::End();
}

void Settings::OnFinish()
{
    Save();
}

pugi::xml_node Settings::Serialize(pugi::xml_node& _Parent)
{
    auto settings = _Parent.append_child("Settings");

    for(auto& item : m_RenderingQueue)
    {
        auto serializable = std::dynamic_pointer_cast<IXSerializable>(item);

        if(serializable != nullptr)
            serializable->Serialize(settings);
    }

    return settings;
}

bool Settings::Deserialize(pugi::xml_node& _Node)
{
    pugi::xml_node settings = std::string(_Node.name()) == "Settings" ? _Node : _Node.child("Settings");

    if(settings.empty())
        return false;

    for(auto& item : m_RenderingQueue)
    {
        auto serializable = std::dynamic_pointer_cast<IXSerializable>(item);

        if(serializable != nullptr)
            serializable->Deserialize(settings);
    }

    return true;
}

void Settings::Save()
{
    if(!std::filesystem::exists(m_Path))
        return;

    // create filepath
    std::filesystem::path filepath =
        std::filesystem::path(m_Path.wstring().append(L"/").append(pugi::as_wide(getName())).append(L".xml")).make_preferred();

    // save settigns
    pugi::xml_document document;
    Serialize(document);
    document.save_file(filepath.c_str());
}
