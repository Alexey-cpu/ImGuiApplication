#ifndef FUNCTIONALBLOCK_H
#define FUNCTIONALBLOCK_H

#include "FunctionalBlockExecutionContext.h"
#include "FunctionalBlockPort.h"
#include "DynamicGraphEdge.h"
#include "Complex.h"
#include <Singleton.h>

// imgui
# ifndef IMGUI_DEFINE_MATH_OPERATORS
#     define IMGUI_DEFINE_MATH_OPERATORS
# endif
#include <imgui.h>
#include <imgui_internal.h>

namespace FunctuionalBlockPropertyProviderScope
{
    // service methods
    template< typename T >
    std::string convert_to_string( T _Input );

    template<> inline std::string convert_to_string<bool>( bool _Input )
    {
        return std::to_string( _Input );
    }

    template<> inline std::string convert_to_string< float >( float _Input )
    {
        return std::to_string( _Input );
    }

    template<> inline std::string convert_to_string<double>( double _Input )
    {
        return std::to_string( _Input );
    }

    template<> inline std::string convert_to_string<long double>( long double _Input )
    {
        return std::to_string( _Input );
    }

    template<> inline std::string convert_to_string<short>( short _Input )
    {
        return std::to_string( _Input );
    }

    template<> inline std::string convert_to_string<int>( int _Input )
    {
        return std::to_string( _Input );
    }

    template<> inline std::string convert_to_string<long>( long _Input )
    {
        return std::to_string( _Input );
    }

    template<> inline std::string convert_to_string<long long>( long long _Input )
    {
        return std::to_string( _Input );
    }

    template<> inline std::string convert_to_string<unsigned short>( unsigned short _Input )
    {
        return std::to_string( _Input );
    }

    template<> inline std::string convert_to_string<unsigned int>( unsigned int _Input )
    {
        return std::to_string( _Input );
    }

    template<> inline std::string convert_to_string<unsigned long>( unsigned long _Input )
    {
        return std::to_string( _Input );
    }

    template<> inline std::string convert_to_string<unsigned long long>( unsigned long long _Input )
    {
        return std::to_string( _Input );
    }

    template<> inline std::string convert_to_string< std::string >( std::string _Input )
    {
        return _Input;
    }

    template<> inline std::string convert_to_string< Complex<double> >( Complex<double> _Input )
    {
        return _Input.to_string();
    }

    template<> inline std::string convert_to_string< Complex<float> >( Complex<float> _Input )
    {
        return _Input.to_string();
    }
};

// IFunctionalBlockProperty
class IFunctionalBlockProperty : public FactoryObject, public IPugiXMLSerializable
{
public:

    IFunctionalBlockProperty(const FactoryObjectHierarchy* _Caller) :
        _Caller(_Caller){}

    virtual ~IFunctionalBlockProperty(){}

    const FactoryObjectHierarchy* _Caller = nullptr;

    virtual bool less(const IFunctionalBlockProperty* _Other) const = 0;
    virtual bool greater(const IFunctionalBlockProperty* _Other) const = 0;
    virtual std::string to_string() const = 0;
};

// FunctionalBlockProperty
template<typename T>
class FunctionalBlockProperty : public IFunctionalBlockProperty
{
public:

    typedef T __type;

    FunctionalBlockProperty(const FactoryObjectHierarchy* _Caller) :
        IFunctionalBlockProperty(_Caller){}

    virtual ~FunctionalBlockProperty(){}

    // interface
    virtual __type get() const = 0;
    virtual void set(__type)  = 0;

    virtual bool less(const IFunctionalBlockProperty* _Other) const override
    {
        const FunctionalBlockProperty<__type>* other =
            dynamic_cast<const FunctionalBlockProperty<__type>*>(_Other);

        return other != nullptr && this->get() < other->get();
    }

    virtual bool greater(const IFunctionalBlockProperty* _Other) const override
    {
        const FunctionalBlockProperty<__type>* other =
            dynamic_cast<const FunctionalBlockProperty<__type>*>(_Other);

        return other != nullptr && this->get() > other->get();
    }

    virtual std::string to_string() const override
    {
        return FunctuionalBlockPropertyProviderScope::convert_to_string<__type>(this->get());
    }
};

// IFunctionalBlockProperties
class IFunctionalBlockProperties
{
public:
    IFunctionalBlockProperties(){}
    virtual ~IFunctionalBlockProperties(){}
    virtual std::vector<FactoryObject*> get_properties() const = 0;

    bool contains_property(std::string _Name) const
    {
        auto properties =
            this->get_properties();

        for(auto property : properties)
        {
            IPugiXMLSerializable* instance =
                dynamic_cast<IPugiXMLSerializable*>(property);

            if(instance == nullptr)
                continue;

            pugi::xml_document doc;
            auto main = instance->pugi_serialize(doc);

            if(std::string(main.name())==_Name)
                return true;
        }

        return false;
    }

    template<typename __type> FunctionalBlockProperty<__type>* find_property(std::string _Name) const
    {
        auto properties =
            this->get_properties();

        for(auto property : properties)
        {
            FunctionalBlockProperty<__type>* instance =
                dynamic_cast<FunctionalBlockProperty<__type>*>(property);

            if(instance == nullptr)
                continue;

            pugi::xml_document doc;
            auto main = instance->pugi_serialize(doc);

            if(std::string(main.name())==_Name)
                return instance;
        }

        return nullptr;
    }

    template<typename __type> __type* find_property() const
    {
        auto properties =
            this->get_properties();

        for(auto property : properties)
        {
            __type* instance = dynamic_cast<__type*>(property);

            if(instance != nullptr)
                return instance;
        }

        return nullptr;
    }

    // static API
    static std::string retrieve_nick(FactoryObject* _Property)
    {
        if( _Property == nullptr )
            return std::string();

        IPugiXMLSerializable* serializable =
            dynamic_cast<IPugiXMLSerializable*>(_Property);

        if( serializable == nullptr )
            return std::string();

        pugi::xml_document doc;

        auto xproperty = serializable->pugi_serialize(doc);

        return xproperty.empty() || xproperty.attribute("NICK").empty() ?
                   std::string() :
                        std::string(xproperty.attribute("NICK").value());
    }

    static std::string retrieve_real_nick(FactoryObject* _Property)
    {
        if( _Property == nullptr )
            return std::string();

        IPugiXMLSerializable* serializable =
            dynamic_cast<IPugiXMLSerializable*>(_Property);

        if( serializable == nullptr )
            return std::string();

        pugi::xml_document doc;
        auto xproperty =
            serializable->pugi_serialize(doc);

        if( xproperty.empty() )
            return std::string();

        std::string name = retrieve_nick(_Property);
        std::string nick = std::string( xproperty.child("Real").attribute("NICK").value() );

        return nick != name && !nick.empty() ? nick : "Re[" + name + "]";
    }

    static std::string retrieve_imag_nick(FactoryObject* _Property)
    {
        if( _Property == nullptr )
            return std::string();

        IPugiXMLSerializable* serializable =
            dynamic_cast<IPugiXMLSerializable*>(_Property);

        if( serializable == nullptr )
            return std::string();

        pugi::xml_document doc;
        auto xproperty =
            serializable->pugi_serialize(doc);

        if( xproperty.empty() )
            return std::string();

        std::string name = retrieve_nick(_Property);
        std::string nick = std::string( xproperty.child("Imag").attribute("NICK").value() );

        return nick != name && !nick.empty() ? nick : "Re[" + name + "]";
    }

    static std::string retrieve_units(FactoryObject* _Property)
    {
        if( _Property == nullptr )
            return std::string();

        IPugiXMLSerializable* serializable =
            dynamic_cast<IPugiXMLSerializable*>(_Property);

        if( serializable == nullptr )
            return std::string();

        pugi::xml_document doc;
        auto xproperty =
            serializable->pugi_serialize(doc);

        return std::string(xproperty.child("Multiplier").attribute("NICK").value()) + std::string(xproperty.attribute("UNITS").value());
    }

    static std::string retrieve_real_units(FactoryObject* _Property)
    {
        if( _Property == nullptr )
            return std::string();

        IPugiXMLSerializable* serializable =
            dynamic_cast<IPugiXMLSerializable*>(_Property);

        if( serializable == nullptr )
            return std::string();

        pugi::xml_document doc;
        auto xproperty =
            serializable->pugi_serialize(doc);

        if( xproperty == nullptr )
            return std::string();

        auto multiplier = xproperty.child("Multiplier");

        auto xreal = xproperty.child("Real");

        return !xreal.empty() ?
                   std::string(multiplier.attribute("NICK").value()) + std::string(xreal.attribute("UNITS").value()) :
                        retrieve_units( _Property );
    }

    static std::string retrieve_imag_units(FactoryObject* _Property)
    {
        if( _Property == nullptr )
            return std::string();

        IPugiXMLSerializable* serializable =
            dynamic_cast<IPugiXMLSerializable*>(_Property);

        if( serializable == nullptr )
            return std::string();

        pugi::xml_document doc;
        auto xproperty =
            serializable->pugi_serialize(doc);

        if( xproperty == nullptr )
            return std::string();

        auto multiplier = xproperty.child("Multiplier");

        auto xreal = xproperty.child("Imag");

        return !xreal.empty() ?
                   std::string(multiplier.attribute("NICK").value()) + std::string(xreal.attribute("UNITS").value()) :
                        retrieve_units( _Property );
    }

    static std::string retrieve_value(FactoryObject* _Property)
    {
        IPugiXMLSerializable* serializable =
            dynamic_cast<IPugiXMLSerializable*>(_Property);

        pugi::xml_document doc;
        return serializable != nullptr ?
                   std::string(serializable->pugi_serialize(doc).text().get()) :
                        std::string();
    }
};

// FunctionalBlockEditor
class FunctionalBlockEditor : public IPugiXMLSerializable
{
public:

    // constructors
    FunctionalBlockEditor(FactoryObjectHierarchy* _Caller) :
        m_Caller(_Caller){}

    // virtual destructor
    virtual ~FunctionalBlockEditor(){}

    // virtual functions
    virtual bool is_valid() const = 0;

    // virtual functions override
    void set_name(std::string _Name)
    {
        if(m_Caller != nullptr)
            m_Caller->set_name( _Name );
    }

    std::string get_name() const
    {
        return m_Caller != nullptr ? m_Caller->get_name() : std::string();
    }

    // IPugiXMLSerializable
    virtual pugi::xml_node pugi_serialize(pugi::xml_node& _Parent) override
    {
        (void)_Parent;
        return pugi::xml_node();
    }
    virtual bool pugi_deserialize(pugi::xml_node& _Node) override
    {
        (void)_Node;
        return true;
    }

    FactoryObjectHierarchy* m_Caller = nullptr;
};

// IFunctionalBlockEditor
class IFunctionalBlockEditor
{
public:

    // constructors
    IFunctionalBlockEditor(){}

    // virtual destructor
    virtual ~IFunctionalBlockEditor(){}

    // interface
    virtual FunctionalBlockEditor* request_editor() const = 0;
};

// FunctionalBlock
class FunctionalBlock : public DynamicGraphNode, public IPugiXMLSerializable, public FactoryObjectRenderer
{
public:

    // constructors
    FunctionalBlock(
        FactoryObjectHierarchy* _Parent,
        std::string             _Type,
        pugi::xml_node          _Object);

    FunctionalBlock(
        FactoryObjectHierarchy* _Parent,
        std::string             _Type,
        std::string             _ID = std::string());

    // virtual destructor
    virtual ~FunctionalBlock();

    // getters
    std::string get_type() const;
    std::string get_path() const;
    FactoryObjectHierarchy* get_inputs_root() const;
    FactoryObjectHierarchy* get_outputs_root() const;

    template< typename __type = FunctionalBlockEditor >
    __type* get_editor() const
    {
        const IFunctionalBlockEditor* editor =
            dynamic_cast<const IFunctionalBlockEditor*>(this);

        return editor != nullptr ?
                   dynamic_cast<__type*>(editor->request_editor()) :
                        nullptr;
    }

    template<typename __type>
    __type* get_property() const
    {
        const IFunctionalBlockProperties* provider =
            dynamic_cast<const IFunctionalBlockProperties*>(this);

        return provider != nullptr ?
                   provider->find_property<__type>() :
                        nullptr;
    }

    template<typename __type>
    __type get_property(std::string _Name) const
    {
        const IFunctionalBlockProperties* provider =
            dynamic_cast<const IFunctionalBlockProperties*>(this);

        FunctionalBlockProperty<__type>* property =
            provider != nullptr ?
                provider->find_property<__type>(_Name) :
                    nullptr;

        return property != nullptr ? property->get() : __type();
    }

    // setters
    template< typename __type >
    void set_property(std::string _Name, __type _Value)
    {
        IFunctionalBlockProperties* provider =
            dynamic_cast<IFunctionalBlockProperties*>(this);

        auto property =
            provider != nullptr ?
                provider->find_property<__type>(_Name) :
                    nullptr;

        if(property != nullptr)
            property->set(_Value);
    }

    // public methods
    template< typename __type >
    FunctionalBlockDataPort<__type>* add_input( std::string _Name )
    {
        return new FunctionalBlockDataPort< __type >(
            FunctionalBlockPort::Type::INPUT,
            _Name,
            __type(),
            this->get_inputs_root()
        );
    }

    template< typename __type >
    FunctionalBlockDataPort<__type>* add_output( std::string _Name )
    {
        return new FunctionalBlockDataPort<__type>(
            FunctionalBlockPort::Type::OUTPUT,
            _Name,
            __type(),
            this->get_outputs_root()
        );
    }

    FunctionalBlockPort* find_input(std::string _Name) const;
    FunctionalBlockPort* find_output(std::string _Name) const;

    // virtual functions
    template< typename __type >
    bool preprocess()
    {
        FunctionalBlockExecutionContext<__type>* context =
            dynamic_cast<FunctionalBlockExecutionContext<__type>*>(this);

        return context != nullptr && context->preprocess();
    }

    template< typename __type >
    bool postprocess()
    {
        FunctionalBlockExecutionContext<__type>* context =
            dynamic_cast<FunctionalBlockExecutionContext<__type>*>(this);

        return context != nullptr && context->postprocess();
    }

    template< typename __type >
    bool process()
    {
        // functional block preprocessing
        if(!preprocess<__type>())
            return false;

        // transmit info from outputs to their adjacent inputs
        get_outputs_root()->apply_function_to_children_recursuve(
            [](FactoryObjectHierarchy* _Object)
            {
                FunctionalBlockPort* port =
                    dynamic_cast<FunctionalBlockPort*>(_Object);

                if(port == nullptr)
                    return;

                for(auto adjacentEdge : port->retrieve_adjacent_edges())
                {
                    DynamicGraphEdge* edge =
                        dynamic_cast<DynamicGraphEdge*>(adjacentEdge);

                    if(edge == nullptr)
                        continue;

                    FunctionalBlockPort* adjacentPort =
                        edge->get_adjacent_node<FunctionalBlockPort>(port);

                    if(adjacentPort != nullptr)
                        adjacentPort->retrieve_value_from(port);
                }
            }
        );

        // functional block postprocessing
        return postprocess<__type>();
    }

    std::list<FunctionalBlock*> retrive_adjacent_functional_blocks(FunctionalBlockPort* _Port) const;

    // virtual methods override
    virtual std::list<DynamicGraphNode*> retrieve_adjacent_nodes() override;

    // IPugiXMLSerializable
    virtual pugi::xml_node pugi_serialize(pugi::xml_node& _Parent) override;
    virtual bool pugi_deserialize(pugi::xml_node& _Node) override;

    //----------------------------------------------------------------------------------
    // IMGUI
    //----------------------------------------------------------------------------------
    //virtual void set_geometry(const Geometry& _Geometry) override;
    virtual void draw_start(const glm::mat4& _Transform) override;
    virtual void draw_process(const glm::mat4& _Transform) override;
    virtual void draw_finish(const glm::mat4& _Transform) override;

    ImU32 m_Color = IM_COL32(
        (*Singleton<PseudoRandomNumberGenerator<int>>::Instance())(0, 255),
        (*Singleton<PseudoRandomNumberGenerator<int>>::Instance())(0, 255),
        (*Singleton<PseudoRandomNumberGenerator<int>>::Instance())(0, 255),
        255);

    //ImRect m_MouseCatcher;

    struct MouseEvent
    {
        enum Type
        {
            None,
            Click,
            Release,
            Down,
            Hover
        } type = Type::None;

        ImGuiMouseButton_ button = ImGuiMouseButton_::ImGuiMouseButton_Left;

    } m_MouseEvent;

    struct KeyEvent
    {
        enum Type
        {
            None,
            Click,
            Release,
            Down
        } type;

        ImGuiKey key = ImGuiKey::ImGuiKey_None;

    } m_KeyEvent;
    //----------------------------------------------------------------------------------

protected:

    // info
    FactoryObjectHierarchy* m_InputsRoot  = nullptr;
    FactoryObjectHierarchy* m_OutputsRoot = nullptr;
    std::string             m_Type        = STRINGIFY(FunctionalBlock);

    // service methods
    void instantiate(FactoryObjectHierarchy* _Parent);
};

// IFunctionalBlockParametersComputerComponent
template<typename __type>
class IFunctionalBlockParametersComputerComponent
{
public:

    // constructors
    IFunctionalBlockParametersComputerComponent(){}

    // virtual destructor
    virtual ~IFunctionalBlockParametersComputerComponent(){}

    // pure virtual methods
    virtual void reset_parameters(__type* = nullptr) = 0;
    virtual void compute_parameters(__type* = nullptr) = 0;
};

// FunctionalBlockFactory
class FunctionalBlockFactory : public FactoryObject
{
public:

    // nested types
    class IFunctionalBlockCreator : public FactoryObjectComponent
    {
    protected:

        const std::string m_Name;

        virtual void set_object( FactoryObject* _Decorator ) override
        {
            if(_Decorator == nullptr)
                return;

            // remove previously attached creators
            IFunctionalBlockCreator* creator =
                _Decorator->find_component<IFunctionalBlockCreator>(
                    [this](FactoryObjectComponent* _Behaviour)->bool
                    {
                        IFunctionalBlockCreator* request =
                            dynamic_cast<IFunctionalBlockCreator*>(_Behaviour);

                        return request != nullptr &&
                               request != this &&
                               request->get_name() == this->get_name();
                    }
                    );

            if(creator != nullptr)
                delete creator;
        }

    public:

        // constructors
        IFunctionalBlockCreator(std::string _Name) : m_Name(_Name){}

        // getters
        std::string get_name() const
        {
            return m_Name;
        }

        // virtual destructors
        virtual ~IFunctionalBlockCreator(){}

        // virtual functions override
        virtual FactoryObjectHierarchy* Create(FactoryObjectHierarchy* _Environment, pugi::xml_node _Node = pugi::xml_node()) = 0;
    };

    // constructors
    FunctionalBlockFactory(){}

    // virtual destructor
    virtual ~FunctionalBlockFactory(){}

    // public methods
    template<typename __type = IFunctionalBlockCreator>
    __type* get_creator()
    {
        __type* creator = this->find_component<__type>();

        return dynamic_cast<IFunctionalBlockCreator*>(creator) != nullptr ? creator : nullptr;
    }

    IFunctionalBlockCreator* get_creator( std::string _Name )
    {
        auto components = this->get_components();

        for(auto component : components)
        {
            IFunctionalBlockCreator* creator =
                dynamic_cast<IFunctionalBlockCreator*>(component);

            if(creator != nullptr && creator->get_name() == _Name)
                return creator;
        }

        return nullptr;
    }

    std::map<std::string, IFunctionalBlockCreator*> get_creators()
    {
        std::map<std::string, IFunctionalBlockCreator*> map;

        auto components = this->get_components();

        for(auto component : components)
        {
            IFunctionalBlockCreator* creator =
                dynamic_cast<IFunctionalBlockCreator*>(component);

            if(creator != nullptr &&
                map.find(creator->get_name()) == map.end())
                map.insert({creator->get_name(), creator});
        }

        return map;
    }
};

#endif // FUNCTIONALBLOCK_H
