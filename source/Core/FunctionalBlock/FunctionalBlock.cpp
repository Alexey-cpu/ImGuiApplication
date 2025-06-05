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

void FunctionalBlock::draw_start(const glm::mat4& _Transform){}

void FunctionalBlock::draw_process(const glm::mat4& _Transform)
{
    ImGui::GetWindowDrawList()->ChannelsSetCurrent(FunctionalBlockExecutionEnvironment::DrawChannels::Blocks);

    // catch mouse events
    bool hovered = ImRect(
                       get_rect(true).GetTL() + get_rect().GetSize() * 0.1f,
                       get_rect(true).GetBR() - get_rect().GetSize() * 0.1f).Contains(ImGui::GetIO().MousePos);

    m_MouseEvent.type = MouseEvent::Type::None;

    if(hovered)
    {
        for(size_t button = ImGuiMouseButton_::ImGuiMouseButton_Left;
             button < ImGuiMouseButton_::ImGuiMouseButton_Middle; button++)
        {
            m_MouseEvent.button = (ImGuiMouseButton_)button;

            if(ImGui::IsMouseClicked(button))
            {
                m_MouseEvent.type = MouseEvent::Type::Click;
                break;
            }

            if(ImGui::IsMouseReleased(button))
            {
                m_MouseEvent.type = MouseEvent::Type::Release;
                break;
            }

            if(ImGui::IsMouseDown(button))
            {
                m_MouseEvent.type = MouseEvent::Type::Down;
                break;
            }
        }
    }

    // draw self
    set_transformation(_Transform);

    ImGui::GetWindowDrawList()->AddRect(
        get_rect(true).GetTL(),
        get_rect(true).GetBR(),
        m_Color,
        0.f,
        ImDrawFlags_::ImDrawFlags_None,
        hovered || get_parent<FunctionalBlockExecutionEnvironment::SelectionNode>() ? 16.f : 4.f
        );

    // draw ports
    auto inputsSpace  = get_rect().GetSize() / get_inputs_root()->get_children().size();
    auto outputsSpace = get_rect().GetSize() / get_outputs_root()->get_children().size();
    auto portSize     = ImVec2(std::min(inputsSpace.x, outputsSpace.x), std::min(inputsSpace.y, outputsSpace.y));

    {
        auto portOrigin  = get_rect().GetTL() + ImVec2(0.f, inputsSpace.y / 2.0);
        auto portOffset  = ImVec2(0.f, portSize.y);
        auto drawChannel = 0;

        get_inputs_root()->apply_function_to_children_recursuve(
            [&portOrigin, &portSize, &portOffset, &drawChannel, &_Transform](FactoryObject* _Object)
            {
                FunctionalBlockPort* port =
                    dynamic_cast<FunctionalBlockPort*>(_Object);

                // TODO: draw ports here !!!
                if(port != nullptr)
                {
                    auto origin = portOrigin - portSize * 0.5f;
                    port->set_rect(ImRect(ImRect(origin, origin + portSize)));
                    port->set_transformation(_Transform);
                    port->draw(_Transform);
                    portOrigin += portOffset;
                    drawChannel++;
                }
            }
            );
    }

    // draw outputs
    {
        auto portOrigin = get_rect().GetTR() + ImVec2(0.f, portSize.y / 2.0);
        auto portOffset = ImVec2(0.f, portSize.y);
        auto drawChannel = 0;

        get_outputs_root()->apply_function_to_children_recursuve(
            [&portOrigin, &portSize, &portOffset, &drawChannel, &_Transform](FactoryObject* _Object)
            {
                FunctionalBlockPort* port =
                    dynamic_cast<FunctionalBlockPort*>(_Object);

                // TODO: draw ports here !!!
                if(port != nullptr)
                {
                    auto origin = portOrigin - portSize * 0.5f;
                    port->set_rect(ImRect(ImRect(origin, origin + portSize)));
                    port->set_transformation(_Transform);
                    port->draw(_Transform);
                    portOrigin += portOffset;
                    drawChannel++;
                }
            }
            );
    }
}

void FunctionalBlock::draw_finish(const glm::mat4& _Transform)
{
}
