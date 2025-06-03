#include "DynamicGraphEdge.h"
#include "DynamicGraphNode.h"

// DynamicGraphEdge
DynamicGraphEdge::DynamicGraphEdge() : DynamicGraphObject(){}

bool DynamicGraphEdge::attach(
        DynamicGraphNode* _Source,
        DynamicGraphNode* _Target )
{
    if(_Source == nullptr && _Target == nullptr)
        return false;

    // detach branch from it's source and target nodes
    this->detach();

    // setup new source and target nodes
    this->m_Source = _Source;
    this->m_Target = _Target;

    // register edge in both source and target nodes, i.e there is a way
    // from source to target and vise versa
    if( this->get_source<DynamicGraphNode>() != nullptr )
        this->get_source<DynamicGraphNode>()->add(this);

    if(this->get_target<DynamicGraphNode>() != nullptr)
        this->get_target<DynamicGraphNode>()->add(this);

    return true;
}

void DynamicGraphEdge::detach(DynamicGraphNode* _Node)
{
    if(_Node != nullptr)
    {
        _Node->erase(this);
        return;
    }

    if(this->get_source<DynamicGraphNode>() != nullptr)
        this->get_source<DynamicGraphNode>()->erase(this);

    if(this->get_target<DynamicGraphNode>() != nullptr)
        this->get_target<DynamicGraphNode>()->erase(this);
}

// virtual destructor
DynamicGraphEdge::~DynamicGraphEdge()
{
    this->detach();
}
