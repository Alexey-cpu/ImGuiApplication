#ifndef FACTORYOBJECT_H
#define FACTORYOBJECT_H

// custom includes
#include "UUID4Generator.h"

// STL includes
#include <string>
#include <functional>

// imgui
# ifndef IMGUI_DEFINE_MATH_OPERATORS
#     define IMGUI_DEFINE_MATH_OPERATORS
# endif
#include <imgui.h>
#include <imgui_internal.h>

// absl
#include <absl/container/btree_map.h>

class FactoryObject;
class FactoryObjectComponent;

// FactoryObjectComponent
class FactoryObjectComponent
{
public:

    FactoryObjectComponent(bool _AllowMultipleInstances = true);
    virtual ~FactoryObjectComponent();

    bool allow_multiple_instannces();

    template< typename __type = FactoryObject >
    __type* get_object() const
    {
        return dynamic_cast<__type*>(m_Object);
    }

protected:

    // info
    FactoryObject* m_Object                 = nullptr;
    bool           m_AllowMultipleInstances = true;

    // service methods
    virtual void set_object(FactoryObject* _Decorator);

    friend class FactoryObject;
};

// FactoryObject
class FactoryObject
{
public:

    FactoryObject();
    virtual ~FactoryObject();

    void detach_component(FactoryObjectComponent* _Component);
    void remove_component(FactoryObjectComponent* _Component);
    void remove_all_components();

    template<typename __type>
    void remove_component()
    {
        this->remove_component(this->find_component<__type>());
    }

    template< typename __type >
    bool attach_component(__type* _Component)
    {
        FactoryObjectComponent* component = dynamic_cast<FactoryObjectComponent*>(_Component);

        // check that input is IBehaviour
        if(component == nullptr)
            return false;

        // detach component from it's previous master object
        if(component->get_object() != nullptr)
            component->get_object()->detach_component( component );

        // do not try to attach the same component twice
        auto iterator = std::find(
            m_Components.begin(),
            m_Components.end(),
            _Component
            );

        if(iterator != m_Components.end())
        {
            // just to avoid memory leaks !!!
            delete component;
            component = nullptr;

            return false;
        }

        // do not try to attach component that does not allow multiple instances at the same time
        __type* attachedComponent = this->find_component<__type>();

        if(attachedComponent != nullptr &&
            !component->allow_multiple_instannces())
        {
            // just to avoid memory leaks !!!
            delete component;
            component = nullptr;

            return false;
        }

        component->m_Object = this;
        m_Components.push_back(component);

        // callback on object set
        component->set_object(this);

        return true;
    }

    std::vector<FactoryObjectComponent*>& get_components() const
    {
        return this->m_Components;
    }

    template<typename __type>
    __type* find_component() const
    {
        for(auto component : m_Components)
        {
            if(dynamic_cast< __type* >(component) != nullptr)
            {
                return dynamic_cast< __type* >(component);
            }
        }

        return nullptr;
    }

    template<typename __type>
    __type* find_component(std::function< bool(FactoryObjectComponent*) > _Predicate) const
    {
        if( _Predicate == nullptr )
            return nullptr;

        for(auto component : m_Components)
        {
            if( dynamic_cast< __type* >(component) != nullptr && _Predicate(component) )
            {
                return dynamic_cast< __type* >(component);
            }
        }

        return nullptr;
    }

    template< typename __type >
    std::vector<__type*> find_components() const
    {
        std::vector<__type*> output;

        for(auto component : m_Components)
        {
            if( dynamic_cast< __type* >(component) != nullptr )
            {
                output.push_back(dynamic_cast< __type* >(component));
            }
        }

        return output;
    }

    template<typename __type>
    std::vector<__type*> find_components(std::function<bool(FactoryObjectComponent*)> _Predicate) const
    {
        if(_Predicate == nullptr)
            return std::vector<__type*>();

        std::vector<__type*> output;

        for(auto component : m_Components)
        {
            if(dynamic_cast< __type* >(component) != nullptr &&
                _Predicate(component))
            {
                output.push_back(dynamic_cast< __type* >(component));
            }
        }

        return output;
    }

protected:

    mutable std::vector<FactoryObjectComponent*> m_Components;
};

// FactoryObjectHierarchy
class FactoryObjectHierarchy : public FactoryObject
{
public:

    class Geometry
    {
    public:
        Geometry(
            const ImVec2& _Origin = ImVec2(0.f, 0.f),
            const ImVec2& _Size   = ImVec2(128.f, 128.f),
            const ImVec2& _Offset = ImVec2(0.f, 0.f),
            const float&  _Scale  = 1.f) :
            m_Origin(_Origin),
            m_Size(_Size),
            m_Offset(_Offset),
            m_Scale(_Scale)
        {
            set_rect(ImRect(m_Origin, m_Origin + m_Size));
        }

        ~Geometry(){}

        // getters
        ImVec2 get_origin() const
        {
            return m_Origin;
        }

        ImVec2 get_size() const
        {
            return m_Size;
        }

        ImVec2 get_offset() const
        {
            return m_Offset;
        }

        ImRect get_rect() const
        {
            return m_Rect;
        }

        float get_scale() const
        {
            return m_Scale;
        }

        // setters
        void set_origin(const ImVec2& _Origin)
        {
            set_rect(ImRect(_Origin, _Origin + get_size()));
        }

        void set_size(const ImVec2& _Size)
        {
            set_rect(ImRect(get_origin(), get_origin() + _Size));
        }

        void set_offset(const ImVec2& _Offset)
        {
            m_Offset = _Offset;
        }

        void set_rect(const ImRect& _Rect)
        {
            m_Rect   = _Rect;
            m_Origin = m_Rect.GetTL();
            m_Size   = m_Rect.GetSize();
        }

        void set_scale(float _Scale)
        {
            m_Scale = _Scale;
        }

    protected:

        ImVec2 m_Origin = ImVec2(0.f, 0.f);
        ImVec2 m_Size   = ImVec2(128.f, 128.f);
        ImVec2 m_Offset = ImVec2(0.f, 0.f);
        ImRect m_Rect   = ImRect(m_Origin, m_Origin + m_Size);
        float  m_Scale  = 1.f;
    };

    FactoryObjectHierarchy(
        const std::string&      _Name   = std::string(),
        FactoryObjectHierarchy* _Parent = nullptr,
        const std::string&      _UUID   = std::string()
        );

    virtual ~FactoryObjectHierarchy();

    // predicates
    bool has_attribute(uint64_t _Attribute);
    bool has_parent() const;
    bool has_children() const;
    bool recursive_children_access_enabled();

    // getters
    UUID4Generator::UUID4 get_id() const;
    virtual std::string get_name() const;
    uint64_t get_attributes();
    FactoryObjectHierarchy* get_root_item() const;
    Geometry get_geometry() const;

    template<typename __type = FactoryObjectHierarchy>
    __type* get_parent() const
    {
        return dynamic_cast<__type*>(this->m_Parent);
    }

    template<typename __type>
    __type* get_parent_recursive() const
    {
        const FactoryObjectHierarchy* root = this;

        while(root->get_parent() != nullptr)
        {
            __type* parent = dynamic_cast<__type*>(root->get_parent());

            if(parent != nullptr)
            {
                return parent;
            }

            root = root->get_parent();
        }

        return nullptr;
    }

    // setters
    virtual void set_name(const std::string& _Name);
    void set_attributes(uint64_t _Attributes);
    void set_parent(FactoryObjectHierarchy* _Parent);

    void enable_recursive_children_access();
    void disable_recursive_children_access();

    template< typename __type >
    __type* find_child(std::function<bool(FactoryObjectHierarchy*)> _Predicate) const
    {
        if(_Predicate == nullptr)
            return nullptr;

        for(auto child : m_Children)
        {
            if(_Predicate(child))
            {
                return dynamic_cast<__type*>(child);
            }
        }

        return nullptr;
    }

    template<typename __type>
    __type* find_child() const
    {
        return find_child<__type>(
            [](FactoryObjectHierarchy* _Object)->bool
            {
                return dynamic_cast<__type*>(_Object) != nullptr;
            }
        );
    }

    template<typename __type = FactoryObjectHierarchy>
    __type* find_child(const std::string& _Name) const
    {
        return find_child<__type>(
            [&_Name]( FactoryObjectHierarchy* _Object )->bool
            {
                return _Object != nullptr &&
                       _Object->get_name() == _Name;
            }
            );
    }

    template< typename __type = FactoryObjectHierarchy >
    __type* find_child(const UUID4Generator::UUID4& _ID) const
    {
        auto iterator = m_Children.find(_ID);
        return iterator != m_Children.end() ? dynamic_cast<__type*>(*iterator) : nullptr;
    }

    std::list<FactoryObjectHierarchy*>
    find_children(std::function<bool(FactoryObjectHierarchy*) > _Predicate) const
    {
        std::list<FactoryObjectHierarchy*> output;

        if(_Predicate == nullptr)
            return output;

        for(auto child : this->m_Children)
        {
            if(_Predicate( child ))
            {
                output.push_back( child );
            }
        }

        return output;
    }

    template<typename __type = FactoryObjectHierarchy>
    __type* find_child_recursuve(
        std::function<bool(FactoryObjectHierarchy*)> _Predicate = [](FactoryObjectHierarchy* _Object)->bool
        {
            return dynamic_cast<__type*>(_Object) != nullptr;
        },
        std::function<bool(FactoryObjectHierarchy*)> _RecursiveChildrenAccessPredicate = nullptr
    ) const
    {
        return dynamic_cast<__type*>(
            FactoryObjectHierarchy::find_child_recursuve(
                this,
                _Predicate,
                _RecursiveChildrenAccessPredicate
                )
            );
    }

    template<typename __type = FactoryObjectHierarchy>
    __type* find_child_recursuve(
        const std::string&                           _Name,
        std::function<bool(FactoryObjectHierarchy*)> _RecursiveChildrenAccessPredicate = nullptr
    ) const
    {
        return FactoryObjectHierarchy::find_child_recursuve<__type>(
            [&_Name]( FactoryObjectHierarchy* _Object)->bool
            {
                return _Object != nullptr &&
                       _Object->get_name() == _Name;
            },
            _RecursiveChildrenAccessPredicate
            );
    }

    template<typename __type = FactoryObjectHierarchy>
    __type* find_child_recursuve(
        const UUID4Generator::UUID4&                 _ID,
        std::function<bool(FactoryObjectHierarchy*)> _RecursiveChildrenAccessPredicate = nullptr) const
    {
        FactoryObjectHierarchy* object =
            FactoryObjectHierarchy::find_child_recursuve(
            this,
            _ID,
            _RecursiveChildrenAccessPredicate
            );

        return dynamic_cast<__type*>(object);
    }

    std::list<FactoryObjectHierarchy*>
    find_children_recursuve(
        const std::function<bool(FactoryObjectHierarchy*)>& _Predicate,
        const std::function<bool(FactoryObjectHierarchy*)>& _RecursiveChildrenAccessPredicate = nullptr) const;

    void find_children_recursuve(
        const std::function<bool(FactoryObjectHierarchy*)>& _Predicate,
        std::list<FactoryObjectHierarchy*>&                 _Output,
        const std::function<bool(FactoryObjectHierarchy*)>& _RecursiveChildrenAccessPredicate = nullptr) const;

    void apply_function_to_children_recursuve(
        const std::function<void(FactoryObjectHierarchy*)>& _Function,
        const std::function<bool(FactoryObjectHierarchy*)>& _RecursiveChildrenAccessPredicate = nullptr) const;

    size_t count_children_recursive(
        const std::function<bool(FactoryObjectHierarchy*)>& _Predicate,
        const std::function<bool(FactoryObjectHierarchy*)>& _RecursiveChildrenAccessPredicate = nullptr) const;

    std::list<FactoryObjectHierarchy*> get_children() const;

    void remove_all_children();

    template<typename __type>
    void remove_children()
    {
        auto children = this->m_Children;

        for(auto child : children)
        {
            if(dynamic_cast<__type*>(child) == nullptr)
                continue;

            delete child;
            child = nullptr;
        }
    }

    void remove_child(FactoryObjectHierarchy* _Object);

    // UUID regeneration function creates new UUID
    // for this item, as the function reattaches the
    // object to it's parent DO NOT call when iterating
    // trough the obuject's children list
    void regenerate_uuid();

    // virtual functions override
    virtual std::string to_string();
    virtual void from_string(const std::string& _Value);
    virtual void set_geometry(const Geometry& _Geometry);
    virtual void draw_start();
    virtual void draw_process();
    virtual void draw_finish();
    void draw();

protected:

    // info
    std::string             m_Name                           = std::string();
    UUID4Generator::UUID4   m_UUID                           = UUID4Generator::UUID4(std::string());
    uint64_t                m_Attributes                     = 0;
    FactoryObjectHierarchy* m_Parent                         = nullptr;
    bool                    m_RecursiveChildrenAccessEnabled = true;
    Geometry                m_Geometry;

private:

    // nested types
    class HashList final
    {
    protected:

        typedef UUID4Generator::UUID4 T1;
        typedef FactoryObjectHierarchy*    T2;
        typedef UUID4Generator::UUID4::TransparentComparator Compare;

        typedef typename std::list<T2>::iterator iterator;
        typedef typename std::list<T2>::const_iterator const_iterator;
        std::list<T2> m_List;
        absl::btree_map<T1, const_iterator, Compare> m_Map;

    public:

        // constructors
        HashList(){}

        // virtual destructor
        ~HashList(){}

        std::pair< const_iterator, bool > insert(std::pair<T1, T2> _Input)
        {
            m_List.push_back(_Input.second);

            if(!m_Map.insert({_Input.first, std::prev(m_List.end())}).second)
            {
                m_List.pop_back();
                return {m_List.end(), false};
            }

            return {std::prev(m_List.end()), true};
        }

        const_iterator find(T1 _ID) const
        {
            auto iterator = m_Map.find(_ID);
            return iterator != m_Map.end() ? iterator->second : m_List.end();
        }

        void erase(T1 _ID)
        {
            auto iterator = m_Map.find(_ID);

            if(iterator == m_Map.end())
                return;

            m_List.erase(iterator->second);
            m_Map.erase(iterator);
        }

        void clear()
        {
            m_Map.clear();
            m_List.clear();
        }

        bool empty() const
        {
            return m_List.empty();
        }

        const_iterator begin() const
        {
            return m_List.begin();
        }

        const_iterator  end() const
        {
            return m_List.end();
        }

        size_t size() const
        {
            return m_List.size();
        }

    } m_Children;

    bool m_TheHierarchyIsRemoved = false;

    // service methods
    static FactoryObjectHierarchy* find_child_recursuve(
        const FactoryObjectHierarchy*                _Object,
        std::function<bool(FactoryObjectHierarchy*)> _Predicate,
        std::function<bool(FactoryObjectHierarchy*)> _RecursiveChildrenAccessPredicate = nullptr);

    static FactoryObjectHierarchy* find_child_recursuve(
        const FactoryObjectHierarchy* _Object,
        const UUID4Generator::UUID4&  _ID,
        std::function<bool(FactoryObjectHierarchy*) > _RecursiveChildrenAccessPredicate = nullptr);

    static void find_children_recursuve(
        const FactoryObjectHierarchy* _Object,
        std::function<bool(FactoryObjectHierarchy*)>  _Predicate,
        std::list<FactoryObjectHierarchy*>&            _Output,
        std::function< bool( FactoryObjectHierarchy*)> _RecursiveChildrenAccessPredicate = nullptr);

    static void apply_function_to_children_recursuve(
        const FactoryObjectHierarchy*                       _Object,
        const std::function<void(FactoryObjectHierarchy*)>& _Function,
        const std::function<bool(FactoryObjectHierarchy*)>& _RecursiveChildrenAccessPredicate = nullptr);

    void attach_child(FactoryObjectHierarchy* _Object);

    void detach_child(FactoryObjectHierarchy* _Object);
};

// FactoryObjectUUIDChecker
class FactoryObjectUUIDChecker : public FactoryObjectComponent
{
public:

    // constructors
    FactoryObjectUUIDChecker(){}

    // virtual destructor
    virtual ~FactoryObjectUUIDChecker(){}

    // virtual methods
    virtual bool check(const UUID4Generator::UUID4&)
    {
        return true;
    }
};

#endif // FACTORYOBJECT_H
