#ifndef IMGUIAPPLICATIONSERIALIZABLE_H
#define IMGUIAPPLICATIONSERIALIZABLE_H

// pugixml
#include <pugixml.hpp>

namespace ImGuiApplication
{
    class IXSerializable
    {
    public:
        IXSerializable(){}
        virtual ~IXSerializable(){}
        virtual pugi::xml_node Serialize(pugi::xml_node& _Parent) = 0;
        virtual bool Deserialize(pugi::xml_node& _Node) = 0;
    };
};

#endif // IMGUIAPPLICATIONSERIALIZABLE_H
