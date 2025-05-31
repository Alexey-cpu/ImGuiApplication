#ifndef IMGUIAPPLICATIONSETTINGS_H
#define IMGUIAPPLICATIONSETTINGS_H

// Custom
#include <ImGuiApplicationLayer.h>
#include <ImGuiApplicationSerializable.h>

// imgui
#include <imgui.h>

// STL
#include <filesystem>

namespace ImGuiApplication
{
    namespace Settings
    {
        class Settings : public Layer, public IXSerializable
        {
        public:

            Settings(
                const std::string&                       _Title    = "Settings",
                const std::filesystem::path&             _Path     = std::filesystem::current_path(),
                const std::list<std::shared_ptr<Layer>>& _Settings = std::list<std::shared_ptr<Layer>>());

            virtual ~Settings();

            virtual void OnClose() override;
            virtual void OnAwake() override;
            virtual void OnUpdate() override;
            virtual void OnFinish() override;

            virtual pugi::xml_node Serialize(pugi::xml_node& _Parent) override;
            virtual bool Deserialize(pugi::xml_node& _Node) override;

        protected:

            std::filesystem::path  m_Path;
            std::shared_ptr<Layer> m_CurrentLayer = nullptr;

            // service methods
            void Save();
        };
    }
};

#endif // IMGUIAPPLICATIONSETTINGS_H
