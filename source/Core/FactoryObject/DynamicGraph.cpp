#include <DynamicGraph.h>
#include <Utils.h>

DynamicGraph::DynamicGraph( FactoryObjectHierarchy* _Parent, IDynamicGraphNodeCreator* _NodeCreator, IDynamicGraphEdgeCreator* _EdgeCreator) :
    DynamicGraphObject(STRINGIFY(DynamicGraph), _Parent),
    m_NodesRoot(new FactoryObjectHierarchy(STRINGIFY(DynamicGraph::m_NodesRoot), this))
{
    // add nodes/edges creators
    m_NodeCreator = _NodeCreator;
    m_EdgeCreator = _EdgeCreator;
}

DynamicGraph::DynamicGraph(FactoryObjectHierarchy* _Parent) :
    DynamicGraph(_Parent, new IDynamicGraphNodeCreator() , new IDynamicGraphEdgeCreator()){}

DynamicGraph::~DynamicGraph()
{
    if( m_NodeCreator != nullptr )
        delete m_NodeCreator;

    if( m_EdgeCreator != nullptr )
        delete m_EdgeCreator;

    m_NodeCreator = nullptr;
    m_EdgeCreator = nullptr;
}

FactoryObjectHierarchy* DynamicGraph::get_nodes_root() const
{
    return m_NodesRoot;
}

void DynamicGraph::move(DynamicGraphObject* _Other)
{
    if(dynamic_cast<DynamicGraphNode*>(_Other) == nullptr &&
        dynamic_cast<DynamicGraph*>(_Other) == nullptr)
        return;

    _Other->set_parent(this->get_nodes_root());
}

DynamicGraphNode* DynamicGraph::add_node(const std::string& _Name)
{
    if( m_NodeCreator == nullptr )
        return nullptr;

    DynamicGraphNode* node =
            dynamic_cast<DynamicGraphNode*>(m_NodeCreator->Create(_Name));

    node->set_parent(this->get_nodes_root());

    return node;
}

void DynamicGraph::erase_node( DynamicGraphNode* _Node )
{
    this->get_nodes_root()->remove_child( _Node );
}

size_t DynamicGraph::count_nodes()
{
    return this->get_nodes_root()->count_children_recursive(
                [](FactoryObjectHierarchy* Object)->bool
                {
                    return dynamic_cast< DynamicGraphNode* >( Object ) != nullptr;
                }
        );
}

DynamicGraphEdge* DynamicGraph::add_edge(
        DynamicGraphNode* _Source,
        DynamicGraphNode* _Target,
        size_t _Attributes )
{
    if( m_EdgeCreator == nullptr )
        return nullptr;

    DynamicGraphEdge* edge = dynamic_cast<DynamicGraphEdge*>(m_EdgeCreator->Create());

    if(!edge->attach(_Source, _Target))
    {
        delete edge;
        edge = nullptr;
        return nullptr;
    }

    edge->set_attributes(_Attributes);

    return edge;
}

void DynamicGraph::erase_edge( DynamicGraphEdge* _Edge )
{
    if( _Edge != nullptr )
        delete _Edge;
}

// service methods
void DynamicGraph::breadth_first_search(
    std::queue< DynamicGraphNode* >& _Output,
    DynamicGraphNode* _Vertex ) const
{
    if(_Vertex == nullptr)
        return;

    // push the given vertex into queue
    std::queue< DynamicGraphNode* > _Queue;

    _Queue.push(_Vertex);
    _Output.push(_Vertex);
    _Vertex->marked() = true;

    while( !_Queue.empty() )
    {
        // extract vertex out of the queue
        DynamicGraphNode* current = _Queue.front();
        _Queue.pop();

        // add all not-marked adjacent nodes into the queue
        std::list<DynamicGraphNode*> adjacentNodes = current->retrieve_adjacent_nodes();

        for(auto adjacentNode : adjacentNodes)
        {
            DynamicGraphNode* node = dynamic_cast<DynamicGraphNode*>(adjacentNode);

            if(node != nullptr && !node->marked())
            {
                // nark yet unvisited node
                node->marked() = true;

                // enqueue yet unvisited node
                _Queue.push( node );
                _Output.push( node );
            }
        }
    }
}

void DynamicGraph::depth_first_search(
    DynamicGraphNode* _Vertex,
    std::stack<DynamicGraphNode*>& _Output ) const
{
    if(_Vertex == nullptr)
        return;

    // initialize DFS
    std::stack<DynamicGraphNode*> _Stack;
    _Stack.push( _Vertex );
    bool done = false;

    // DFS routine
    while(!_Stack.empty())
    {
        // suppose the node is done
        done = true;

        // get the top of the stack
        DynamicGraphNode* current = _Stack.top();
        current->marked() = true;

        // discover all the adjacent nodes and retrive yet unvisited node
        std::list<DynamicGraphNode*> adjacentNodes = current->retrieve_adjacent_nodes();

        for(auto adjacentNode : adjacentNodes)
        {
            DynamicGraphNode* node = dynamic_cast<DynamicGraphNode*>(adjacentNode);

            if(node != nullptr && !node->marked())
            {
                done = false;
                _Stack.push( node );
                break;
            }
        }

        if( done )
        {
            _Output.push( current );
            _Stack.pop();
        }
    }
}

void DynamicGraph::number_nodes_and_edges()
{
    unmark_nodes_and_edges();

    size_t nodeNumber = 0;
    size_t edgeNumber = 0;

    this->get_nodes_root()->apply_function_to_children_recursuve(
        [&nodeNumber, &edgeNumber](FactoryObjectHierarchy* _Object)
        {
            DynamicGraphNode* node = dynamic_cast<DynamicGraphNode*>(_Object);

            if( node == nullptr )
                return;

            node->set_number(nodeNumber++);

            for(auto j = node->retrieve_adjacent_edges().begin(); j != node->retrieve_adjacent_edges().end(); j++)
            {
                DynamicGraphEdge* edge = dynamic_cast<DynamicGraphEdge*>(*j);

                if( edge == nullptr || edge->marked() )
                    continue;

                edge->marked() = true;
                edge->set_number(edgeNumber++);
            }
        }
    );

    unmark_nodes_and_edges();
}

void DynamicGraph::unmark_nodes_and_edges()
{
    this->get_nodes_root()->apply_function_to_children_recursuve(
        []( FactoryObjectHierarchy* _Object )
        {
            DynamicGraphNode* node = dynamic_cast< DynamicGraphNode* >(_Object);

            if( node == nullptr )
                return;

            node->marked() = false;

            for(auto j = node->retrieve_adjacent_edges().begin() ; j != node->retrieve_adjacent_edges().end() ; j++)
            {
                DynamicGraphEdge* edge = dynamic_cast< DynamicGraphEdge* >(*j);

                if( edge == nullptr )
                    continue;

                edge->marked() = false;
            }

            auto adjacentNodes = node->retrieve_adjacent_nodes();

            for(auto adjacentNode : adjacentNodes)
            {
                if( adjacentNode != nullptr )
                {
                    adjacentNode->marked() = false;
                }
            }
        }
    );
}

std::list< std::vector<DynamicGraphNode*> > DynamicGraph::find_paths_to(
        DynamicGraphNode* _Source,
        std::function<bool(DynamicGraphNode*)> _TargetPredicate,
        std::function<std::list< DynamicGraphNode* >(DynamicGraphNode*) > _AdjacentEdgesRetrievalFunction )
{
    if(_Source == nullptr ||
        _TargetPredicate == nullptr)
        return std::list< std::vector<DynamicGraphNode*> >();

    this->unmark_nodes_and_edges();

    std::list< std::vector<DynamicGraphNode*> > paths;
    std::vector<DynamicGraphNode*> stack;

    stack.push_back(_Source);

    while(!stack.empty())
    {
        DynamicGraphNode* front = *std::prev( stack.end() );

        bool done = true;

        // retrieve adjacent nodes
        auto adjacentNodes = _AdjacentEdgesRetrievalFunction(front);

        for(auto adjacentNode : adjacentNodes)
        {
            if(adjacentNode == nullptr ||
                adjacentNode->marked())
                continue;

            // mark node
            adjacentNode->marked() = true;

            // enqueue node
            stack.push_back(adjacentNode);

            done = false;
        }

        if(done)
        {
            stack.pop_back();
        }

        // check predicate
        if(_TargetPredicate(front))
        {
            std::vector<DynamicGraphNode*> output = stack;
            output.push_back(front);
            paths.push_back(output);
        }
    }

    this->unmark_nodes_and_edges();

    return paths;
}

std::vector<std::vector<DynamicGraphNode*>> DynamicGraph::find_connected_components(bool _BFS)
{
    // unmark only nodes
    this->get_nodes_root()->apply_function_to_children_recursuve(
        []( FactoryObjectHierarchy* _Object )
        {
            DynamicGraphNode* node = dynamic_cast<DynamicGraphNode*>(_Object);

            if(node == nullptr)
                return;

            node->marked() = false;

            auto adjacentNodes = node->retrieve_adjacent_nodes();

            for(auto adjacentNode : adjacentNodes)
            {
                if(adjacentNode != nullptr)
                    adjacentNode->marked() = false;
            }
        }
    );

    // generate output
    std::vector<std::vector< DynamicGraphNode*>> connectedComponents;

    if(_BFS)
    {
        this->get_nodes_root()->apply_function_to_children_recursuve(
            [&connectedComponents, this]( FactoryObjectHierarchy* _Object )
            {
                DynamicGraphNode* node =
                    dynamic_cast<DynamicGraphNode*>(_Object);

                if(node == nullptr || node->marked())
                    return;

                // retrieve current connected component by BFS
                std::queue<DynamicGraphNode*> queue;

                this->breadth_first_search(queue, node);

                // push not-empty connected component into the output vector
                std::vector<DynamicGraphNode*> vector;

                while(!queue.empty())
                {
                    vector.push_back(queue.front());
                    queue.pop();
                }

                if(!vector.empty())
                    connectedComponents.push_back(vector);
            }
        );
    }
    else
    {
        this->get_nodes_root()->apply_function_to_children_recursuve(
            [&connectedComponents, this]( FactoryObjectHierarchy* _Object )
            {
                DynamicGraphNode* node =
                    dynamic_cast<DynamicGraphNode*>(_Object);

                if(node == nullptr || node->marked())
                    return;

                // retrieve current connected component by BFS
                std::stack<DynamicGraphNode*> stack;

                this->depth_first_search(node, stack);

                // push not-empty connected component into the output vector
                std::vector<DynamicGraphNode*> vector;

                while(!stack.empty())
                {
                    vector.push_back(stack.top());
                    stack.pop();
                }

                if(!vector.empty())
                    connectedComponents.push_back(vector);
            }
        );
    }

    // unmark only nodes
    for(auto& connectedComponent : connectedComponents)
    {
        for(auto node : connectedComponent)
            node->marked() = false;
    }

    return connectedComponents;
}

std::vector<DynamicGraphNode*> DynamicGraph::topological_sort()
{
    // unmark only nodes
    this->get_nodes_root()->apply_function_to_children_recursuve(
        []( FactoryObjectHierarchy* _Object )
        {
            DynamicGraphNode* node = dynamic_cast<DynamicGraphNode*>(_Object);

            if(node == nullptr)
                return;

            node->marked() = false;

            auto adjacentNodes = node->retrieve_adjacent_nodes();

            for(auto adjacentNode : adjacentNodes)
            {
                if(adjacentNode != nullptr)
                    adjacentNode->marked() = false;
            }
        }
    );

    // driver code
    std::stack<DynamicGraphNode*> stack;

    this->get_nodes_root()->apply_function_to_children_recursuve(
        [&stack, this](FactoryObjectHierarchy* _Object)
        {
            DynamicGraphNode* node = dynamic_cast<DynamicGraphNode*>(_Object);

            if(node != nullptr && !node->marked())
                this->depth_first_search(node, stack);
        }
    );

    // retrieve output
    std::vector<DynamicGraphNode*> output;

    while(!stack.empty())
    {
        // unmark node
        auto node = stack.top();
        node->marked() = false;

        // add node to output
        output.push_back(node);
        stack.pop();
    }

    return output;
}

std::vector< std::vector<DynamicGraphNode*> > DynamicGraph::rooted_level_tree(DynamicGraphNode* _Source)
{
    // unmark all nodes and edges
    this->unmark_nodes_and_edges();

    // number nodes and edges of graph
    this->number_nodes_and_edges();

    // find the node with the maximum number
    size_t max = 0;

    this->apply_function_to_children_recursuve(
        [&max](FactoryObjectHierarchy* _Object)
        {
            DynamicGraphNode* object =
                dynamic_cast<DynamicGraphNode*>( _Object );

            if( object == nullptr )
            {
                return;
            }

            // discover this node
            max = __max__( object->get_number(), max );

            // discover adjacent nodes
            auto adjacentNodes = object->retrieve_adjacent_nodes();


            for(auto adjacentNode : adjacentNodes)
            {
                max = __max__( adjacentNode->get_number(), max );
            }
        }
    );

    std::vector< size_t > vector( max + 1 );

    for(size_t i = 0 ; i < vector.size() ; i++)
    {
        vector[i] = 0;
    }

    // BFS
    auto bfs = [&vector]( FactoryObjectHierarchy* _Object )
    {
        DynamicGraphNode* vertex =
            dynamic_cast<DynamicGraphNode*>(_Object);

        if( vertex == nullptr || vertex->marked() )
        {
            return;
        }

        // push the given vertex into queue
        std::queue< DynamicGraphNode* > queue;

        queue.push( vertex );
        vertex->marked() = true;

        while( !queue.empty() )
        {
            // extract vertex out of the queue
            DynamicGraphNode* current = queue.front();
            queue.pop();

            // add all not-marked adjacent nodes into the queue
            std::list<DynamicGraphNode*> adjacentNodes = current->retrieve_adjacent_nodes();

            for(auto adjacentNode : adjacentNodes)
            {
                DynamicGraphNode* node =
                    dynamic_cast<DynamicGraphNode*>(adjacentNode);

                if(node != nullptr && !node->marked())
                {
                    // nark yet unvisited node
                    node->marked() = true;

                    // enqueue yet unvisited node
                    queue.push( node );

                    // compute vertex level
                    vector[ node->get_number() ] += vector[ current->get_number() ] + 1;
                }
            }
        }
    };

    // start from this vertex
    if( _Source != nullptr )
    {
        bfs(_Source);
    }

    this->get_nodes_root()->apply_function_to_children_recursuve(
        [&bfs]( FactoryObjectHierarchy* _Object )
        {
            bfs(_Object);
        }
    );

    // unmark all nodes and edges
    this->unmark_nodes_and_edges();

    // level up
    size_t maxLevel = *std::max_element( vector.begin(), vector.end() );

    std::vector< std::vector<DynamicGraphNode*> > output;
    output.reserve(maxLevel + 1);

    for(size_t i = 0 ; i < (maxLevel + 1) ; i++)
    {
        output.push_back( std::vector<DynamicGraphNode*>() );
    }

    this->apply_function_to_children_recursuve(
        [&output, &vector](FactoryObjectHierarchy* _Object)
        {
            DynamicGraphNode* object =
                dynamic_cast<DynamicGraphNode*>(_Object);

            if(object == nullptr)
                return;

            if(!object->marked())
                output[vector[object->get_number()]].push_back(object);

            // discover adjacent nodes
            auto adjacentNodes = object->retrieve_adjacent_nodes();

            for( auto adjacentNode : adjacentNodes )
            {
                DynamicGraphNode* object =
                    dynamic_cast<DynamicGraphNode*>( adjacentNode );

                if( object == nullptr )
                {
                    continue;
                }

                if( !object->marked() )
                {
                    output[ vector[ object->get_number() ] ].push_back( object );
                    object->marked() = true;
                }
            }
        }
    );

    // unmark all nodes and edges
    this->unmark_nodes_and_edges();

    return output;
}
