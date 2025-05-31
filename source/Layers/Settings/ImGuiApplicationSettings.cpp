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
    //pugi::xml_document document;
    //Serialize(document);
    //document.save_file(m_Path.wstring().c_str());
}

void Settings::OnAwake()
{
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
        ImGui::TableHeadersRow();

        if(std::find(m_RenderingQueue.begin(), m_RenderingQueue.end(), m_CurrentLayer) == m_RenderingQueue.end())
        {
            if(!m_RenderingQueue.empty())
                m_CurrentLayer = m_RenderingQueue.front();
        }

        // draw name column
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

        // draw content column
        ImGui::TableSetColumnIndex(1);

        if(m_CurrentLayer != nullptr)
            m_CurrentLayer->Update();

        ImGui::EndTable();
    }

    ImGui::End();
}

void Settings::OnFinish()
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

    std::cout << "Settings::OnFinish() " << filepath << "\n";
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
            serializable->Deserialize(_Node);
    }

    return true;
}
