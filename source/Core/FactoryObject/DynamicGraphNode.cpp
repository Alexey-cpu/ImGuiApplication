#include "DynamicGraphEdge.h"
#include "DynamicGraphNode.h"

// constructors
DynamicGraphNode::DynamicGraphNode(
    const std::string&      _Name,
    FactoryObjectHierarchy* _Parent,
    const std::string&      _UUID) :
    DynamicGraphObject(_Name, _Parent, _UUID){}

// virtual destructor
DynamicGraphNode::~DynamicGraphNode()
{
    auto edges = this->m_Edges;

    for(auto edge : edges)
    {
        if( edge != nullptr )
            delete edge;
    }

    this->m_Edges.clear();
}

// public methods
std::list<DynamicGraphObject*> DynamicGraphNode::find(std::function<bool( DynamicGraphObject* )> _Predicate)
{
    if(_Predicate == nullptr)
        return std::list<DynamicGraphObject*>();

    std::list<DynamicGraphObject*> output;

    for(auto object : m_Edges)
    {
        if(_Predicate(object))
            output.push_back(object);
    }

    return output;
}

void DynamicGraphNode::add(DynamicGraphObject* _Edge)
{
    if(_Edge != nullptr)
        m_Edges.push_back(_Edge);
}

void DynamicGraphNode::erase(DynamicGraphObject* _Edge)
{
    auto iterator = std::find_if(
        m_Edges.begin(),
        m_Edges.end(),
        [_Edge](DynamicGraphObject* _Object)
        {
            return _Object == _Edge;
        }
    );

    if(iterator != m_Edges.end())
        m_Edges.erase(iterator);
}

std::list<DynamicGraphNode*> DynamicGraphNode::retrieve_adjacent_nodes()
{
    std::list<DynamicGraphNode*> output;

    for(auto object : m_Edges)
    {
        DynamicGraphEdge* edge =
            dynamic_cast<DynamicGraphEdge*>(object);

        if(edge == nullptr)
            continue;

        DynamicGraphNode* node =
            edge->get_adjacent_node<DynamicGraphNode>(this);

        if(node != nullptr)
            output.push_back(node);
    }

    return output;
}

const std::list<DynamicGraphObject*>& DynamicGraphNode::retrieve_adjacent_edges()
{
    return m_Edges;
}

// static API
std::list<DynamicGraphObject*> DynamicGraphNode::find_incoming_edges(DynamicGraphNode* _Node)
{
    if(_Node == nullptr)
        return std::list<DynamicGraphObject*>();

    return _Node->find(
        [_Node](DynamicGraphObject* _Object)->bool
        {
            DynamicGraphEdge* edge =
                dynamic_cast<DynamicGraphEdge*>(_Object);

            return edge != nullptr && edge->get_target() == _Node;
        }
    );
}

std::list< DynamicGraphObject* > DynamicGraphNode::find_outgoing_edges(DynamicGraphNode* _Node)
{
    if( _Node == nullptr )
        return std::list<DynamicGraphObject*>();

    return _Node->find(
        [_Node](DynamicGraphObject* _Object)->bool
        {
            DynamicGraphEdge* edge =
                dynamic_cast<DynamicGraphEdge*>(_Object);

            return edge != nullptr && edge->get_source() == _Node;
        }
    );
}

std::list<DynamicGraphObject*> DynamicGraphNode::find_edges(DynamicGraphNode* _Source, DynamicGraphNode* _Target)
{
    // create predicates
    auto sourcePredicate = [_Source, _Target](DynamicGraphObject* _Object)->bool
    {
        DynamicGraphEdge* edge = dynamic_cast<DynamicGraphEdge*>(_Object);

        return edge != nullptr &&
                edge->get_source() == _Source &&
                edge->get_target() == _Target;
    };

    auto targtPredicate = [_Source, _Target](DynamicGraphObject* _Object)->bool
    {
        DynamicGraphEdge* edge = dynamic_cast<DynamicGraphEdge*>( _Object );

        return edge != nullptr &&
                edge->get_source() == _Target &&
                edge->get_target() == _Source;
    };

    // search
    std::list<DynamicGraphObject*> sourceEdges;
    std::list<DynamicGraphObject*> targetEdges;

    if(_Source != nullptr)
        sourceEdges = _Source->find(sourcePredicate);

    if(_Target != nullptr)
        targetEdges = _Target->find(targtPredicate);

    sourceEdges.sort();
    targetEdges.sort();
    sourceEdges.merge(targetEdges);
    sourceEdges.unique();

    return sourceEdges;
}
