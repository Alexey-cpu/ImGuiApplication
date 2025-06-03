#ifndef DYNAMICGRAPHNODE_H
#define DYNAMICGRAPHNODE_H

#include "DynamicGraphObject.h"

// DynamicGraphNode
class DynamicGraphNode : public DynamicGraphObject
{
public:

    // constructors
    DynamicGraphNode(
            const std::string&      _Name   = std::string(),
            FactoryObjectHierarchy* _Parent = nullptr,
            const std::string&      _UUID   = std::string());

    // virtual destructor
    virtual ~DynamicGraphNode();

    // public methods
    void add(DynamicGraphObject* _Edge);

    void erase(DynamicGraphObject* _Edge);

    virtual std::list<DynamicGraphNode*> retrieve_adjacent_nodes();

    const std::list<DynamicGraphObject*>& retrieve_adjacent_edges();

    std::list<DynamicGraphObject*> find(std::function<bool(DynamicGraphObject*)> _Predicate);

    static std::list<DynamicGraphObject*> find_incoming_edges(DynamicGraphNode* _Node);

    static std::list<DynamicGraphObject*> find_outgoing_edges(DynamicGraphNode* _Node);

    static std::list<DynamicGraphObject*> find_edges(DynamicGraphNode* _Source, DynamicGraphNode* _Target);

protected:

    // atributes
    std::list<DynamicGraphObject*> m_Edges;

    // friends
    friend class DynamicGraphEdge;
};

// IDynamicGraphNodeCreator
class IDynamicGraphNodeCreator
{
public:

    // constructors
    IDynamicGraphNodeCreator(){}

    // virtual destructor
    virtual ~IDynamicGraphNodeCreator(){}

    // virtual methods to override
    virtual DynamicGraphNode* Create(const std::string& _Name = std::string())
    {
        return new DynamicGraphNode(_Name);
    }
};

#endif // DYNAMICGRAPHNODE_H
