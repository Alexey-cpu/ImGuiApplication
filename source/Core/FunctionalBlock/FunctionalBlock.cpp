#include "FunctionalBlock.h"
#include "FunctionalBlockExecutionEnvironment.h"

FunctionalBlock::FunctionalBlock(
    FactoryObjectHierarchy* _Parent,
    std::string             _Type,
    std::string             _ID) :
    DynamicGraphNode(std::string(), nullptr, _ID),
    m_Type(_Type)
{
    instantiate(_Parent);
}

FunctionalBlock::FunctionalBlock(
    FactoryObjectHierarchy* _Parent,
    std::string             _Type,
    pugi::xml_node          _Object) :
    DynamicGraphNode(
          std::string(),
          nullptr,
          std::string(_Object.child("ID").child_value())),
    m_Type(_Type)
{
    instantiate(_Parent);
}

FunctionalBlock::~FunctionalBlock(){}

void FunctionalBlock::instantiate(FactoryObjectHierarchy* _Parent)
{
    // create auxiliary nodes and setup editor
    m_InputsRoot  = new FactoryObjectHierarchy("inputs", this);
    m_OutputsRoot = new FactoryObjectHierarchy("outputs", this);

    // try to add functional block to execution environment
    FunctionalBlockExecutionEnvironment* environment =
        dynamic_cast<FunctionalBlockExecutionEnvironment*>(_Parent);

    if(environment != nullptr)
    {
        environment->move(this);
    }
    else // set functional block parent otherwise
    {
        environment =
            _Parent != nullptr ?
                _Parent->find_child<FunctionalBlockExecutionEnvironment>() :
                    nullptr;

        if(environment != nullptr)
            environment->move(this);
        else
            this->set_parent(_Parent);
    }

    // disable recursive access to children
    this->disable_recursive_children_access();
}

std::string FunctionalBlock::get_type() const
{
    return m_Type;
}

std::string FunctionalBlock::get_path() const
{
    const FactoryObjectHierarchy* root = this;

    std::string path;

    while(root->get_parent() != nullptr)
    {
        FunctionalBlock* parent =
            root->get_parent<FunctionalBlock>();

        if(parent != nullptr)
        {
            path.insert(0, (parent->get_name().empty() ? parent->get_type() : parent->get_name()) + "/" );
        }

        root = root->get_parent();
    }

    path += (this->get_name().empty() ? this->get_type() : this->get_name());

    return path;
}

FactoryObjectHierarchy* FunctionalBlock::get_inputs_root() const
{
    return m_InputsRoot;
}

FactoryObjectHierarchy* FunctionalBlock::get_outputs_root() const
{
    return m_OutputsRoot;
}

FunctionalBlockPort* FunctionalBlock::find_input( std::string _Name ) const
{
    return this->get_inputs_root()->find_child<FunctionalBlockPort>( _Name );
}

FunctionalBlockPort* FunctionalBlock::find_output( std::string _Name ) const
{
    return this->get_outputs_root()->find_child<FunctionalBlockPort>(  _Name );
}

std::list<FunctionalBlock*> FunctionalBlock::retrive_adjacent_functional_blocks(FunctionalBlockPort* _Port) const
{
    std::list<FunctionalBlock*> output;

    if(_Port == nullptr)
        return output;

    auto adjacentNodes = _Port->retrieve_adjacent_nodes();

    for(auto adjacentNode : adjacentNodes)
    {
        FunctionalBlock* adjacentFunctionalBlock =
            adjacentNode != nullptr ?
                adjacentNode->get_parent_recursive<FunctionalBlock>() :
                    nullptr;

        if(adjacentFunctionalBlock != nullptr)
            output.push_back(adjacentFunctionalBlock);
    }

    return output;
}

std::list<DynamicGraphNode*> FunctionalBlock::retrieve_adjacent_nodes()
{
    std::list<DynamicGraphNode*> nodes;

    this->get_outputs_root()->apply_function_to_children_recursuve(
        [&nodes](FactoryObjectHierarchy* _Object)
        {
            FunctionalBlockPort* port =
                dynamic_cast<FunctionalBlockPort*>(_Object);

            if(port == nullptr)
                return;

            for(auto object : port->retrieve_adjacent_edges())
            {
                DynamicGraphEdge* edge =
                    dynamic_cast<DynamicGraphEdge*>(object);

                if(edge == nullptr)
                    continue;

                DynamicGraphNode* adjacentPort =
                    edge->get_adjacent_node<DynamicGraphNode>(port);

                if(adjacentPort == nullptr)
                    continue;

                FunctionalBlock* parent =
                    adjacentPort->get_parent_recursive<FunctionalBlock>();

                if(parent != nullptr)
                    nodes.push_back(parent);
            }
        }
    );

    return nodes;
}

pugi::xml_node FunctionalBlock::pugi_serialize(pugi::xml_node& _Parent)
{
    // create topics
    auto data = _Parent.append_child(this->get_type().c_str());

    // add topics to file
    data.append_attribute("TYPE" ).set_value(this->get_type().c_str());

    // serialize info
    data.append_child("ID").text().set(this->get_id().to_string().c_str());
    data.append_child("Name").text().set(this->get_name().c_str());

    // serialize inputs
    auto inputs  = data.append_child("Inputs");

    auto blockInputs = this->get_inputs_root()->get_children();

    for(auto blockInput : blockInputs)
    {
        FunctionalBlockPort* port =
            dynamic_cast<FunctionalBlockPort*>(blockInput);

        if(port != nullptr)
            port->pugi_serialize(inputs);
    }

    // serialize outputs
    auto outputs = data.append_child("Outputs");

    auto blockOutputs = this->get_outputs_root()->get_children();

    for(auto blockOutput : blockOutputs)
    {
        FunctionalBlockPort* port =
            dynamic_cast<FunctionalBlockPort*>(blockOutput);

        if(port != nullptr)
            port->pugi_serialize(outputs);
    }

    // serialize components
    auto components = this->get_components();

    for(auto component : components)
    {
        if(dynamic_cast<FunctionalBlockPort*>(component) != nullptr)
            continue;

        IPugiXMLSerializable* serializable =
            dynamic_cast<IPugiXMLSerializable*>(component);

        if(serializable != nullptr)
            serializable->pugi_serialize(data);
    }

    // serialize editor
    if(this->get_editor<FunctionalBlockEditor>() != nullptr)
        this->get_editor<FunctionalBlockEditor>()->pugi_serialize(data);

    // serialize children
    auto children = this->get_children();

    for(auto child : children)
    {
        IPugiXMLSerializable* serializable =
            dynamic_cast<IPugiXMLSerializable*>(child);

        if(serializable != nullptr)
            serializable->pugi_serialize(data);
    }

    return data;
}

bool FunctionalBlock::pugi_deserialize(pugi::xml_node& _Node)
{
    if(_Node.empty())
        return false;

    auto blockInputs  = this->get_inputs_root()->get_children();
    auto blockOutputs = this->get_outputs_root()->get_children();

    for( auto i = _Node.begin() ; i != _Node.end() ; i++ )
    {
        auto name = std::string((*i).name());

        // deserialize name
        if(name == "Name")
            this->set_name(std::string((*i).child_value()));

        // deserialize 'inputs'
        if(name == "Inputs" )
        {
            // deserialize block inputs
            for( auto j = (*i).begin() ; j != (*i).end() ; j++ )
            {
                auto name = (*j).child("Name");

                if(name.empty())
                    continue;

                // try to find port from file in this block
                auto port = this->find_input(std::string(name.child_value()));

                if(port == nullptr)
                    port = this->add_input<FactoryObjectHierarchy*>(std::string());

                port->pugi_deserialize((*j));
            }
        }

        // deserialize 'outputs'
        if(name == "Outputs")
        {
            // deserialize block inputs
            for( auto j = (*i).begin() ; j != (*i).end() ; j++ )
            {
                auto name = (*j).child("Name");

                if(name.empty())
                    continue;

                // try to find port from file in this block
                auto port = this->find_output(std::string(name.child_value()));

                if(port == nullptr)
                    port = this->add_output<FactoryObjectHierarchy*>(std::string());

                port->pugi_deserialize((*j));
            }
        }

        // deserialize 'editor'
        if(name == "Editor" &&
            this->get_editor<FunctionalBlockEditor>() != nullptr)
            this->get_editor<FunctionalBlockEditor>()->pugi_deserialize((*i));
    }

    // deserialize components
    auto components = this->get_components();

    for(auto component : components)
    {
        if(dynamic_cast<FunctionalBlockPort*>(component) != nullptr)
            continue;

        IPugiXMLSerializable* serializable =
            dynamic_cast<IPugiXMLSerializable*>(component);

        if(serializable != nullptr)
            serializable->pugi_deserialize(_Node);
    }

    return true;
}

//---------------------------------------------------------------------------------------------------------------------
// GEOMETRY
//---------------------------------------------------------------------------------------------------------------------
#include <QDebug>

void FunctionalBlock::catchMouseEvents()
{
    auto executionEnvironment =
        get_parent_recursive<FunctionalBlockExecutionEnvironment>();

    if(executionEnvironment == nullptr ||
        !m_Rect.Contains(executionEnvironment->SceneMousePosition()))
        return;

    m_MouseEvent.type = MouseEvent::Type::None;

    for(size_t button = ImGuiMouseButton_::ImGuiMouseButton_Left;
         button < ImGuiMouseButton_::ImGuiMouseButton_Middle; button++)
    {
        m_MouseEvent.button = (ImGuiMouseButton_)button;

        if(ImGui::IsMouseClicked(button))
        {
            m_MouseEvent.type = MouseEvent::Type::Click;
            return;
        }

        if(ImGui::IsMouseReleased(button))
        {
            m_MouseEvent.type = MouseEvent::Type::Release;
            return;
        }

        if(ImGui::IsMouseDown(button))
        {
            m_MouseEvent.type = MouseEvent::Type::Down;
            return;
        }
    }
}

void FunctionalBlock::drawContent()
{
    auto executionEnvironment =
        get_parent_recursive<FunctionalBlockExecutionEnvironment>();

    if(executionEnvironment == nullptr)
        return;

    // draw self
    auto rect = ImRect(executionEnvironment->LocalItemPosition(m_Rect.GetTL()), executionEnvironment->LocalItemPosition(m_Rect.GetBR()));

    executionEnvironment->m_DrawList->AddRect(
        rect.GetTL(),
        rect.GetBR(),
        m_Color,
        0.f,
        ImDrawFlags_::ImDrawFlags_None,
        m_Rect.Contains(executionEnvironment->SceneMousePosition()) ||
                get_parent<FunctionalBlockExecutionEnvironment::SelectionNode>() ? 16.f : 4.f
        );

    // draw inputs
    {
        auto portSpace  = m_Rect.GetSize() / get_inputs_root()->get_children().size();
        auto portOrigin = m_Rect.GetTL() + ImVec2(0.f, portSpace.y / 2.0);
        auto portSize   = portSpace;
        auto portOffset = ImVec2(0.f, portSize.y);

        get_inputs_root()->apply_function_to_children_recursuve(
            [&portOrigin, &portSize, &portOffset](FactoryObject* _Object)
            {
                FunctionalBlockPort* port =
                    dynamic_cast<FunctionalBlockPort*>(_Object);

                // TODO: draw ports here !!!
                if(port != nullptr)
                {
                    ImRect r(portOrigin, portOrigin + portSize);
                    port->setGeometry(r.GetTL(), r.GetSize() * 0.5f);
                    port->draw();
                    portOrigin += portOffset;
                }
            }
            );
    }

    // draw outputs
    {
        auto portSpace  = m_Rect.GetSize() / get_inputs_root()->get_children().size();
        auto portOrigin = m_Rect.GetTR() + ImVec2(0.f, portSpace.y / 2.0);
        auto portSize   = portSpace;
        auto portOffset = ImVec2(0.f, portSize.y);

        get_outputs_root()->apply_function_to_children_recursuve(
            [&portOrigin, &portSize, &portOffset](FactoryObject* _Object)
            {
                FunctionalBlockPort* port =
                    dynamic_cast<FunctionalBlockPort*>(_Object);

                // TODO: draw ports here !!!
                if(port != nullptr)
                {
                    ImRect r(portOrigin, portOrigin + portSize);
                    port->setGeometry(r.GetTL(), r.GetSize() * 0.5f);
                    port->draw();
                    portOrigin += portOffset;
                }
            }
        );
    }
}

void FunctionalBlock::draw()
{
    catchMouseEvents();
    drawContent();
}

void FunctionalBlock::setGeometry(ImVec2 _Origin, ImVec2 _Size)
{
    auto executionEnvironment =
        get_parent_recursive<FunctionalBlockExecutionEnvironment>();

    // align self to a grid
    auto gridSize = executionEnvironment != nullptr ? executionEnvironment->m_GridSize : 1.f;

    m_Origin = ImVec2(std::roundf(_Origin.x / gridSize), std::roundf(_Origin.y / gridSize)) * gridSize;
    m_Size   = _Size;
    m_Rect   = ImRect(m_Origin, m_Origin + m_Size);

    /*
    FunctionalBlock* parent = get_parent<FunctionalBlock>();
    //auto origin = m_Origin + (parent != nullptr ? parent->m_Origin : ImVec2(0.f, 0.f));

    // setup parent-relative geometry
    if(parent != nullptr)
    {
        m_Origin = _Origin + (parent != nullptr ? parent->m_Origin : ImVec2(0.f, 0.f));
        m_Size   = _Size;
        m_Rect   = ImRect(m_Origin, m_Origin + m_Size);
    }
    else
    {
        m_Origin = ImVec2(std::roundf(_Origin.x / gridSize), std::roundf(_Origin.y / gridSize)) * gridSize;
        m_Size   = _Size;
        m_Rect   = ImRect(m_Origin, m_Origin + m_Size);
    }
*/
}
