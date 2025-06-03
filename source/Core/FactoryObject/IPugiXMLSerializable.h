#ifndef IPUGIXMLSERIALIZABLE_H
#define IPUGIXMLSERIALIZABLE_H

//------------------------------------------------------------------------------------------------------------------------------------
// pugixml serializer interface
//------------------------------------------------------------------------------------------------------------------------------------
#include <pugixml.hpp>

class IPugiXMLSerializable
{
public:

    // constructors
    IPugiXMLSerializable(){}

    // virtual destructor
    virtual ~IPugiXMLSerializable(){}

    // IPugiXMLSerializable
    virtual pugi::xml_node pugi_serialize(pugi::xml_node& _Parent) = 0;
    virtual bool pugi_deserialize(pugi::xml_node& _Node) = 0;
};
//------------------------------------------------------------------------------------------------------------------------------------


#endif // IPUGIXMLSERIALIZABLE_H
