#ifndef FUNCTIONALBLOCKPORT_H
#define FUNCTIONALBLOCKPORT_H

// external includes
#include "DynamicGraphNode.h"
#include "IPugiXMLSerializable.h"

// imgui
# ifndef IMGUI_DEFINE_MATH_OPERATORS
#     define IMGUI_DEFINE_MATH_OPERATORS
# endif
#include <imgui.h>
#include <imgui_internal.h>

#include <Singleton.h>

// FunctionalBlockPort
class FunctionalBlockPort :
    public DynamicGraphNode,
    public FactoryObjectComponent,
    public IPugiXMLSerializable
{
public:

    // nested types
    enum Type
    {
        INPUT,
        OUTPUT,
        NONE
    };

    // constructors
    FunctionalBlockPort(
            Type               _PortType,
            std::string        _Name   = std::string(),
            FactoryObjectHierarchy* _Parent = nullptr,
            std::string        _Object = std::string() );

    // virtual destructor
    virtual ~FunctionalBlockPort();

    // virtual functions
    virtual const std::type_info& get_type_info() const = 0;
    virtual void retrieve_value_from(DynamicGraphNode* _Port) = 0;
    virtual bool is_compatible_to(FunctionalBlockPort* _Port) const;
    virtual std::string get_name() const override;
    bool is_conntected();
    Type get_port_type() const;

    // IPugiXMLSerializable
    virtual pugi::xml_node pugi_serialize(pugi::xml_node& _Parent) override;
    virtual bool pugi_deserialize(pugi::xml_node& _Node) override;

    //----------------------------------------------------------------------------------
    // GEOMETRY
    //----------------------------------------------------------------------------------
    void draw();

    void setGeometry(ImVec2 _Origin, ImVec2 _Size);

    // geometry
    ImVec2 m_Origin = ImVec2();
    ImVec2 m_Size   = ImVec2();
    ImRect m_Rect   = ImRect();

    ImU32 m_Color = IM_COL32(
        (*Singleton<PseudoRandomNumberGenerator<int>>::Instance())(0, 255),
        (*Singleton<PseudoRandomNumberGenerator<int>>::Instance())(0, 255),
        (*Singleton<PseudoRandomNumberGenerator<int>>::Instance())(0, 255), 255);
    //----------------------------------------------------------------------------------

protected:

    // info
    Type m_Type;
};

// FunctionalBlockDataPort
template< typename __type >
class FunctionalBlockDataPort : public FunctionalBlockPort
{
    __type m_Data;
public:

    // constructors
    FunctionalBlockDataPort(
        Type               _PortType,
        std::string        _Name   = std::string(),
        __type             _Data   = __type(),
        FactoryObjectHierarchy* _Parent = nullptr,
        std::string        _Object = std::string() ) :
        FunctionalBlockPort(_PortType, _Name, _Parent, _Object),
        m_Data( _Data ){}

    // virtual destructors
    virtual ~FunctionalBlockDataPort(){}

    // getters
    __type get()
    {
        return m_Data;
    }

    void set(__type _Data)
    {
        m_Data = _Data;
    }

    // virtual functions override
    virtual void retrieve_value_from(DynamicGraphNode* _Port) override
    {
        FunctionalBlockDataPort<__type>* port = dynamic_cast<FunctionalBlockDataPort<__type>*>(_Port);

        if(port != nullptr &&
            this->is_compatible_to(port))
            this->set(port->get());
    }

    const std::type_info& get_type_info() const override
    {
        return typeid (__type);
    }
};

#endif // FUNCTIONALBLOCKPORT_H
