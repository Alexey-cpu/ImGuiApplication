#include "FactoryObject.h"
#include "Singleton.h"

// FactoryObjectManager
class FactoryObjectManager : public FactoryObject
{
public:

    // constructors
    FactoryObjectManager(){}

    // destructors
    virtual ~FactoryObjectManager(){}

    // public methods
    UUID4Generator::UUID4 uuid(const std::string& _UUID = std::string())
    {
        const std::lock_guard<std::mutex> lock(m_Mutex);

        UUID4Generator::UUID4 uuid = _UUID == std::string() ? m_UUIDGenerator.guid() : UUID4Generator::UUID4(_UUID);
        FactoryObjectUUIDChecker*veryfier = this->find_component<FactoryObjectUUIDChecker>();

        for(size_t i = 0 ; i < 10 && veryfier != nullptr && !veryfier->check(uuid) ; i++)
            uuid = m_UUIDGenerator.guid();

        return uuid;
    }

protected:

    // info
    UUID4Generator::UUID4Generator m_UUIDGenerator;
    std::mutex m_Mutex;
};

// FactoryObjectComponent
FactoryObjectComponent::FactoryObjectComponent(bool _AllowMultipleInstances) :
    m_AllowMultipleInstances(_AllowMultipleInstances){}

FactoryObjectComponent::~FactoryObjectComponent()
{
    if(this->get_object() != nullptr)
        this->get_object()->detach_component(this);
}

bool FactoryObjectComponent::allow_multiple_instannces()
{
    return m_AllowMultipleInstances;
}

void FactoryObjectComponent::set_object(FactoryObject* _Decorator)
{
    (void)_Decorator;
}

// FactoryObject
FactoryObject::FactoryObject(){}

FactoryObject::~FactoryObject()
{
    // remove all components
    remove_all_components();
}

void FactoryObject::detach_component(FactoryObjectComponent* _Component)
{
    auto iterator = std::find(
        m_Components.begin(),
        m_Components.end(),
        _Component
        );

    if(_Component == nullptr ||
        iterator == m_Components.end())
        return;

    _Component->m_Object = nullptr;
    m_Components.erase(iterator);
}

void FactoryObject::remove_component(FactoryObjectComponent* _Component)
{
    // detach
    this->detach_component(_Component);

    // remove
    if(_Component != nullptr)
        delete _Component;
}

void FactoryObject::remove_all_components()
{
    auto components = this->m_Components;

    for(auto component : components)
    {
        if(component == nullptr)
            continue;

        delete component;
        component = nullptr;
    }

    this->m_Components.clear();
}

// FactoryObjectHierarchy
FactoryObjectHierarchy::FactoryObjectHierarchy(
    const std::string&      _Name,
    FactoryObjectHierarchy* _Parent,
    const std::string&      _UUID
    ) : m_Name(_Name),
    m_UUID(Singleton<FactoryObjectManager>::Instance()->uuid(_UUID))
{
    // set parent
    this->set_parent(_Parent);
}

FactoryObjectHierarchy::~FactoryObjectHierarchy()
{
    // detach from parent if it's not being in hierarchy that is totally removed
    if(!this->m_TheHierarchyIsRemoved)
        this->set_parent(nullptr);

    // remove all children
    remove_all_children();
}

bool FactoryObjectHierarchy::has_attribute( uint64_t _Attribute )
{
    return (bool)(m_Attributes & _Attribute);
}

bool FactoryObjectHierarchy::has_parent() const
{
    return m_Parent != nullptr;
}

bool FactoryObjectHierarchy::has_children() const
{
    return !this->m_Children.empty();
}

bool FactoryObjectHierarchy::recursive_children_access_enabled()
{
    return this->m_RecursiveChildrenAccessEnabled;
}

// getters
UUID4Generator::UUID4 FactoryObjectHierarchy::get_id() const
{
    return m_UUID;
}

std::string FactoryObjectHierarchy::get_name() const
{
    return m_Name;
}

uint64_t FactoryObjectHierarchy::get_attributes()
{
    return m_Attributes;
}

FactoryObjectHierarchy* FactoryObjectHierarchy::get_root_item() const
{
    const FactoryObjectHierarchy* root = this;

    FactoryObjectHierarchy* item = nullptr;

    while( root->get_parent() != nullptr )
    {
        item = root->get_parent();
        root = root->get_parent();
    }

    return item;
}

FactoryObjectHierarchy::Geometry FactoryObjectHierarchy::get_geometry() const
{
    return m_Geometry;
}

void FactoryObjectHierarchy::set_name(const std::string& _Name)
{
    m_Name = _Name;
}

void FactoryObjectHierarchy::set_attributes(uint64_t _Attributes)
{
    m_Attributes |= _Attributes;
}

void FactoryObjectHierarchy::set_parent(FactoryObjectHierarchy* _Parent)
{
    // don't try to reattach to the same parent
    if(m_Parent == _Parent)
        return;

    // detach from current parent
    if(m_Parent != nullptr)
        m_Parent->detach_child(this);

    m_Parent = nullptr;

    // attach to a new parent
    if(_Parent != nullptr)
    {
        _Parent->attach_child(this);
        m_Parent = _Parent;
    }
}

void FactoryObjectHierarchy::set_geometry(const FactoryObjectHierarchy::Geometry& _Geometry)
{
    m_Geometry = _Geometry;
}

void FactoryObjectHierarchy::enable_recursive_children_access()
{
    this->m_RecursiveChildrenAccessEnabled = true;
}

void FactoryObjectHierarchy::disable_recursive_children_access()
{
    this->m_RecursiveChildrenAccessEnabled = false;
}

std::list<FactoryObjectHierarchy*> FactoryObjectHierarchy::find_children_recursuve(
    const std::function<bool(FactoryObjectHierarchy*)>& _Predicate,
    const std::function<bool(FactoryObjectHierarchy*)>& _RecursiveChildrenAccessPredicate) const
{
    std::list<FactoryObjectHierarchy*> output;

    FactoryObjectHierarchy::find_children_recursuve(
        this,
        _Predicate,
        output,
        _RecursiveChildrenAccessPredicate);

    return output;
}

void FactoryObjectHierarchy::find_children_recursuve(
    const std::function<bool(FactoryObjectHierarchy*)>& _Predicate,
    std::list<FactoryObjectHierarchy*>&                 _Output,
    const std::function<bool(FactoryObjectHierarchy*)>& _RecursiveChildrenAccessPredicate) const
{
    FactoryObjectHierarchy::find_children_recursuve(
        this,
        _Predicate,
        _Output,
        _RecursiveChildrenAccessPredicate);
}

void FactoryObjectHierarchy::apply_function_to_children_recursuve(
    const std::function<void(FactoryObjectHierarchy*)>& _Function,
    const std::function<bool(FactoryObjectHierarchy*)>& _RecursiveChildrenAccessPredicate) const
{
    FactoryObjectHierarchy::apply_function_to_children_recursuve( this, _Function, _RecursiveChildrenAccessPredicate );
}

size_t FactoryObjectHierarchy::count_children_recursive(
    const std::function<bool(FactoryObjectHierarchy*)>& _Predicate,
    const std::function<bool(FactoryObjectHierarchy*)>& _RecursiveChildrenAccessPredicate) const
{
    size_t size = 0;

    this->apply_function_to_children_recursuve(
        this,
        [&size, _Predicate](FactoryObjectHierarchy* _Object)
        {
            if( _Predicate != nullptr && _Predicate(_Object) )
            {
                size++;
            }
        },
        _RecursiveChildrenAccessPredicate
        );

    return size;
}

std::list<FactoryObjectHierarchy*> FactoryObjectHierarchy::get_children() const
{
    std::list<FactoryObjectHierarchy*> children;

    for(auto child : m_Children)
    {
        children.push_back(child);
    }

    return children;
}

void FactoryObjectHierarchy::remove_all_children()
{
    for(auto child : this->m_Children)
    {
        if(child == nullptr)
            continue;

        child->m_TheHierarchyIsRemoved = true;
        delete child;
        child = nullptr;
    }

    m_Children.clear();
}

void FactoryObjectHierarchy::remove_child(FactoryObjectHierarchy* _Object)
{
    if(_Object  == nullptr ||
        _Object == this    ||
        this->find_child(_Object->get_id()) == nullptr)
    {
        return;
    }

    delete _Object;
    _Object = nullptr;
}

void FactoryObjectHierarchy::regenerate_uuid()
{
    FactoryObjectHierarchy* parent = this->get_parent();

    this->set_parent(nullptr);

    this->m_UUID = Singleton<FactoryObjectManager>::Instance()->uuid( std::string() );

    this->set_parent( parent );
}

std::string FactoryObjectHierarchy::to_string()
{
    return this->get_name();
}

void FactoryObjectHierarchy::from_string(const std::string& _Value)
{
    (void)_Value;
}

void FactoryObjectHierarchy::draw_start(){}
void FactoryObjectHierarchy::draw_process(){}
void FactoryObjectHierarchy::draw_finish(){}
void FactoryObjectHierarchy::draw()
{
    draw_start();
    draw_process();
    draw_finish();
}

FactoryObjectHierarchy* FactoryObjectHierarchy::find_child_recursuve(
    const FactoryObjectHierarchy* _Object,
    std::function<bool(FactoryObjectHierarchy*)> _Predicate,
    std::function<bool(FactoryObjectHierarchy*)> _RecursiveChildrenAccessPredicate)
{
    if(_Predicate == nullptr || _Object == nullptr || !_Object->has_children())
        return nullptr;

    for(auto& child : _Object->m_Children)
    {
        FactoryObjectHierarchy* object = dynamic_cast<FactoryObjectHierarchy*>(child);

        if(object == nullptr)
            continue;

        if(_Predicate(object))
            return object;

        bool recursiveChildrenAccessEnabled = _RecursiveChildrenAccessPredicate != nullptr && _RecursiveChildrenAccessPredicate(object);
        recursiveChildrenAccessEnabled = recursiveChildrenAccessEnabled || object->recursive_children_access_enabled();

        if( object->has_children() && recursiveChildrenAccessEnabled )
        {
            FactoryObjectHierarchy* object = FactoryObjectHierarchy::find_child_recursuve(child, _Predicate, _RecursiveChildrenAccessPredicate);

            if(object != nullptr)
                return object;
        }
    }

    return nullptr;
}

FactoryObjectHierarchy* FactoryObjectHierarchy::find_child_recursuve(
    const FactoryObjectHierarchy* _Object,
    const UUID4Generator::UUID4&  _ID,
    std::function<bool(FactoryObjectHierarchy*) > _RecursiveChildrenAccessPredicate)
{
    if(_Object == nullptr || !_Object->has_children())
        return nullptr;

    auto iterator = _Object->m_Children.find(_ID);

    if(iterator != _Object->m_Children.end())
        return *iterator;

    for(auto& child : _Object->m_Children)
    {
        FactoryObjectHierarchy* object = dynamic_cast<FactoryObjectHierarchy*>(child);

        if( object == nullptr )
            continue;

        bool recursiveChildrenAccessEnabled = _RecursiveChildrenAccessPredicate != nullptr && _RecursiveChildrenAccessPredicate(object);
        recursiveChildrenAccessEnabled = recursiveChildrenAccessEnabled || object->recursive_children_access_enabled();

        if(object->has_children() &&
            recursiveChildrenAccessEnabled)
        {
            FactoryObjectHierarchy* object = FactoryObjectHierarchy::find_child_recursuve(child, _ID, _RecursiveChildrenAccessPredicate);

            if( object != nullptr )
                return object;
        }
    }

    return nullptr;
}

void FactoryObjectHierarchy::find_children_recursuve(
    const FactoryObjectHierarchy* _Object,
    std::function<bool( FactoryObjectHierarchy*)>  _Predicate,
    std::list<FactoryObjectHierarchy*>&            _Output,
    std::function< bool( FactoryObjectHierarchy*)> _RecursiveChildrenAccessPredicate)
{
    if( _Predicate == nullptr || _Object == nullptr || !_Object->has_children() )
        return;

    for(auto& child : _Object->m_Children)
    {
        FactoryObjectHierarchy* object =
            dynamic_cast<FactoryObjectHierarchy*>(child);

        if( object == nullptr )
            continue;

        if( _Predicate( object ) )
            _Output.push_back( object );

        bool recursiveChildrenAccessEnabled = _RecursiveChildrenAccessPredicate != nullptr && _RecursiveChildrenAccessPredicate(child);
        recursiveChildrenAccessEnabled = recursiveChildrenAccessEnabled || object->recursive_children_access_enabled();

        if( object->has_children() && recursiveChildrenAccessEnabled )
            FactoryObjectHierarchy::find_children_recursuve(child, _Predicate, _Output, _RecursiveChildrenAccessPredicate);
    }
}

void FactoryObjectHierarchy::apply_function_to_children_recursuve(
    const FactoryObjectHierarchy*                       _Object,
    const std::function<void(FactoryObjectHierarchy*)>& _Function,
    const std::function<bool(FactoryObjectHierarchy*)>& _RecursiveChildrenAccessPredicate)
{
    if(_Function == nullptr || _Object == nullptr || !_Object->has_children())
        return;

    for(auto& child : _Object->m_Children)
    {
        FactoryObjectHierarchy* object = dynamic_cast<FactoryObjectHierarchy*>(child);

        if(object == nullptr)
            continue;

        bool recursiveChildrenAccessEnabled = _RecursiveChildrenAccessPredicate != nullptr && _RecursiveChildrenAccessPredicate(object);
        recursiveChildrenAccessEnabled = recursiveChildrenAccessEnabled || object->recursive_children_access_enabled();

        _Function(object);

        if(object->has_children() &&
            recursiveChildrenAccessEnabled)
        {
            FactoryObjectHierarchy::apply_function_to_children_recursuve(child, _Function, _RecursiveChildrenAccessPredicate);
        }
    }
}

void FactoryObjectHierarchy::attach_child(FactoryObjectHierarchy* _Object)
{
    if(_Object != nullptr && _Object != this)
    {
        m_Children.insert({_Object->get_id(), _Object});
    }
}

void FactoryObjectHierarchy::detach_child(FactoryObjectHierarchy* _Object)
{
    if(_Object != nullptr && !m_Children.empty())
    {
        m_Children.erase(_Object->get_id());
    }
}
