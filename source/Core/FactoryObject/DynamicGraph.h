#ifndef DYNAMICGRAPH_H
#define DYNAMICGRAPH_H

#include "DynamicGraphNode.h"
#include "DynamicGraphEdge.h"

#include <stack>
#include <queue>

// Dynamic graph
class DynamicGraph : public DynamicGraphObject
{
protected:

    // constants
    FactoryObjectHierarchy* m_NodesRoot = nullptr;

    // info
    IDynamicGraphNodeCreator* m_NodeCreator = nullptr;
    IDynamicGraphEdgeCreator* m_EdgeCreator = nullptr;

    // service methods
    void breadth_first_search(
        std::queue< DynamicGraphNode* >& _Queue,
        DynamicGraphNode* _Vertex ) const;

    void depth_first_search(
        DynamicGraphNode* _Vertex,
        std::stack<DynamicGraphNode*>& _Output ) const;

public:

    // constructor
    DynamicGraph(FactoryObjectHierarchy* _Parent = nullptr);
    DynamicGraph(FactoryObjectHierarchy* _Parent, IDynamicGraphNodeCreator* _NodeCreator, IDynamicGraphEdgeCreator* _EdgeCreator);

    // virtual destructor
    virtual ~DynamicGraph();

    // getters
    FactoryObjectHierarchy* get_nodes_root() const;

    virtual void move(DynamicGraphObject* _Other);

    DynamicGraphNode* add_node(const std::string& _Name = std::string());

    void erase_node(DynamicGraphNode* _Node);

    size_t count_nodes();

    DynamicGraphEdge* add_edge(DynamicGraphNode* _Source, DynamicGraphNode* _Target, size_t _Attributes = 0UL);

    void erase_edge(DynamicGraphEdge* _Edge);

    // topological analysis functions
    std::list< std::vector<DynamicGraphNode*> > find_paths_to(
            DynamicGraphNode* _Source,
            std::function<bool(DynamicGraphNode*)> _TargetPredicate,
            std::function<std::list<DynamicGraphNode*>(DynamicGraphNode*)> _AdjacentEdgesRetrievalFunction = nullptr);

    std::vector<std::vector<DynamicGraphNode*>> find_connected_components(bool = true);

    std::vector<DynamicGraphNode*> topological_sort();

    std::vector<std::vector<DynamicGraphNode*>> rooted_level_tree(DynamicGraphNode* _Source = nullptr);

    void number_nodes_and_edges();

    void unmark_nodes_and_edges();
};

#endif // DYNAMICGRAPH_H
