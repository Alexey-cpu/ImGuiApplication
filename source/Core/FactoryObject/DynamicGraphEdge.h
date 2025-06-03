#ifndef DYNAMICGRAPHEDGE_H
#define DYNAMICGRAPHEDGE_H

#include "DynamicGraphObject.h"

class DynamicGraphNode;
class DynamicGraphEdge;

// DynamicGraphEdge
class DynamicGraphEdge : public DynamicGraphObject
{
public:

    // constructors
    DynamicGraphEdge();

    // virtual destructor
    virtual ~DynamicGraphEdge();

    // getters
    std::string get_name() const override
    {
        if( m_Source != nullptr && m_Target != nullptr )
            return m_Source->get_name() + "-->" + m_Target->get_name();

        if( m_Source != nullptr )
            return m_Source->get_name() + "-->" + "GROUND";

        return FactoryObjectHierarchy::get_name();
    }

    template< class __type = DynamicGraphNode >
    __type* get_adjacent_node(DynamicGraphObject* _Node) const
    {
        DynamicGraphObject* adjacent = nullptr;

        if( _Node == nullptr )
            return dynamic_cast<__type*>(adjacent);

        // retrieve source and target nodes
        DynamicGraphObject* source = get_source<DynamicGraphObject>();
        DynamicGraphObject* target = get_target<DynamicGraphObject>();

        if( source != nullptr && ( source->get_id() != _Node->get_id() ) )
            adjacent = source;

        if( target != nullptr && ( target->get_id() != _Node->get_id() ) )
            adjacent = target;

        return dynamic_cast<__type*>(adjacent);
    }

    template< typename __type = DynamicGraphNode >
    __type* get_source() const
    {
        return dynamic_cast< __type* >( m_Source );
    }

    template< typename __type = DynamicGraphNode >
    __type* get_target() const
    {
        return dynamic_cast< __type* >( m_Target );
    }

    // static API
    virtual bool attach(DynamicGraphNode* _Source,
                        DynamicGraphNode* _Target);

    void detach(DynamicGraphNode* _Node = nullptr);

    //friend class DynamicGraphNode;

protected:

    // info
    DynamicGraphObject* m_Source = nullptr;
    DynamicGraphObject* m_Target = nullptr;
};

// IDynamicGraphEdgeCreator
class IDynamicGraphEdgeCreator
{
public:

    // constructors
    IDynamicGraphEdgeCreator(){}

    // virtual destructor
    virtual ~IDynamicGraphEdgeCreator(){}

    // virtual methods
    virtual DynamicGraphEdge* Create()
    {
        return new DynamicGraphEdge();
    }
};

#endif // DYNAMICGRAPHEDGE_H
