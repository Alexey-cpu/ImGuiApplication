#include "FunctionalBlock.h"
#include "FunctionalBlockPort.h"
#include "FunctionalBlockExecutionEnvironment.h"

// FunctionalBlockPort
FunctionalBlockPort::FunctionalBlockPort(
        Type _PortType,
        std::string _Name,
        FactoryObjectHierarchy* _Parent,
        std::string _Object ) :
    DynamicGraphNode(_Name, _Parent, _Object), m_Type(_PortType){}

FunctionalBlockPort::~FunctionalBlockPort()
{
    // retrive edges list
    auto edges = this->retrieve_adjacent_edges();

    // delete all adjacent edges between functional block ports
    for(auto edge : edges)
    {
        if(edge == nullptr)
            continue;

        delete edge;
        edge = nullptr;
    }

    // clear edges list
    m_Edges.clear();
}

bool FunctionalBlockPort::is_compatible_to( FunctionalBlockPort* _Port ) const
{
    return
            _Port != nullptr &&
            this->get_type_info().hash_code() == _Port->get_type_info().hash_code() &&
            this->get_port_type() != _Port->get_port_type();
}

std::string FunctionalBlockPort::get_name() const
{
    return m_Name.empty() ? this->get_id().to_string() : m_Name;
}

bool FunctionalBlockPort::is_conntected()
{
    // delete all adjacent edges between functional block ports
    for(auto edge : m_Edges)
    {
        DynamicGraphEdge* adjacentEdge =
            dynamic_cast<DynamicGraphEdge*>(edge);

        if(adjacentEdge == nullptr)
            continue;

        auto adjacentNode =
            adjacentEdge->get_adjacent_node(this);

        if(adjacentNode != nullptr &&
            adjacentNode->get_parent_recursive<FunctionalBlock>() != nullptr)
            return true;
    }

    return false;
}

FunctionalBlockPort::Type FunctionalBlockPort::get_port_type() const
{
    return m_Type;
}

bool FunctionalBlockPort::pugi_deserialize(pugi::xml_node& _Node)
{
    if(_Node.empty())
        return false;

    // retrive port data
    pugi::xml_node portData =
        std::string(_Node.name()) == std::string(STRINGIFY(FunctionalBlockPort)) ?
            _Node :
                _Node.child(STRINGIFY(FunctionalBlockPort));

    // check
    if(portData.empty())
        return false;

    // retrive environment
    FunctionalBlockExecutionEnvironment* environment =
        this->get_parent_recursive<FunctionalBlockExecutionEnvironment>();

    if(environment == nullptr)
        return false;

    // retrieve port name
    this->set_name( portData.child("Name").child_value() );

    // extract adjacent ports info
    auto adjacentPortsData = portData.child("AdjacentPorts");

    if( adjacentPortsData == nullptr )
        return false;

    for( auto i = adjacentPortsData.begin() ; i != adjacentPortsData.end() ; i++ )
    {
        auto adjacentPortName   = (*i).child("Name");
        auto adjacentPortParent = (*i).child("Parent");
        auto adjacentBlock      = environment->find_child_recursuve<FunctionalBlock>(UUID4Generator::UUID4(adjacentPortParent.child_value()));

        if( adjacentBlock == nullptr )
            continue;

        // connect ports
        if(this->get_port_type() == Type::INPUT)
            environment->add_edge(adjacentBlock->find_output(adjacentPortName.child_value()), this);
        else
            environment->add_edge(this, adjacentBlock->find_input( adjacentPortName.child_value()));
    }

    return true;
}

pugi::xml_node FunctionalBlockPort::pugi_serialize(pugi::xml_node& _Parent)
{
    // helping lambdas
    auto serialize_helper = [](pugi::xml_node& _Parent, FunctionalBlockPort* _Port)->pugi::xml_node
    {
        auto data = _Parent.append_child(STRINGIFY(FunctionalBlockPort));

        // serialize port Name
        auto name = data.append_child( "Name" );
        name.text().set(_Port->get_name().c_str());

        // serialize port parent
        FunctionalBlock* parent =
            _Port->get_parent_recursive< FunctionalBlock >();

        if( parent != nullptr )
        {
            auto parentX = data.append_child("Parent");
            parentX.text().set( parent->get_id().to_string().c_str());
        }

        return data;
    };

    // main code
    auto data = serialize_helper(_Parent, this);

    // write adjacent ports list
    auto adjacentPorts = data.append_child("AdjacentPorts");
    auto adjacentEdges = this->retrieve_adjacent_edges();

    for(auto adjacentEdge : adjacentEdges)
    {
        DynamicGraphEdge* edge =
            dynamic_cast<DynamicGraphEdge*>(adjacentEdge);

        FunctionalBlockPort* adjacentPort =
            edge != nullptr ?
                edge->get_adjacent_node<FunctionalBlockPort>( this ) :
                    nullptr;

        if(adjacentPort != nullptr)
            serialize_helper(adjacentPorts, adjacentPort);
    }

    return data;
}

void FunctionalBlockPort::draw_start()
{
    // setup draw channel
    ImGui::GetWindowDrawList()->ChannelsSetCurrent(FunctionalBlockExecutionEnvironment::DrawChannels::Ports);

    // catch mouse events
}

void FunctionalBlockPort::draw_process()
{
    auto executionEnvironment =
        get_parent_recursive<FunctionalBlockExecutionEnvironment>();

    if(executionEnvironment == nullptr)
        return;

    auto geometry = get_geometry();

    // draw self
    auto rect = ImRect(
        executionEnvironment->get_item_scene_position(geometry.get_rect().GetTL()),
        executionEnvironment->get_item_scene_position(geometry.get_rect().GetBR()));


    ImGui::GetWindowDrawList()->AddEllipse(
        rect.GetCenter(),
        rect.GetSize() * 0.5f,
        m_Color,
        0.f,
        ImGui::GetWindowDrawList()->_CalcCircleAutoSegmentCount(ImMax(rect.GetSize().x, rect.GetSize().y) * 0.5f),
        geometry.get_rect().Contains(executionEnvironment->get_mouse_scene_position()) ||
               get_parent<FunctionalBlockExecutionEnvironment::SelectionNode>() ? 16.f : 4.f);

    ImGui::GetWindowDrawList()->AddEllipseFilled(
        rect.GetCenter(),
        rect.GetSize() * 0.5f,
        m_Color);

    // draw connection lines
    if(get_port_type() == FunctionalBlockPort::Type::OUTPUT)
    {
        for(auto adjacentEdge : retrieve_adjacent_edges())
        {
            if(adjacentEdge != nullptr)
                adjacentEdge->draw();
        }
    }
}

#include <QDebug>

void FunctionalBlockPort::draw_finish()
{
    // catch mouse event
    auto executionEnvironment =
        get_parent_recursive<FunctionalBlockExecutionEnvironment>();

    if(executionEnvironment == nullptr ||
        !get_geometry().get_rect().Contains(executionEnvironment->get_mouse_scene_position()))
        return;

    if(ImGui::IsMouseClicked(ImGuiMouseButton_::ImGuiMouseButton_Left) &&
        ImGui::IsMouseDown(ImGuiMouseButton_::ImGuiMouseButton_Left))
    {
        executionEnvironment->m_MouseGrabberPort = this;
    }

    if(ImGui::IsMouseReleased(ImGuiMouseButton_::ImGuiMouseButton_Left) &&
        executionEnvironment->m_MouseGrabberPort != this &&
        executionEnvironment->m_MouseGrabberPort != nullptr)
    {
        qDebug() << "Connecting ports !!!";

        auto edge = executionEnvironment->add_edge(executionEnvironment->m_MouseGrabberPort, this);

        if(edge != nullptr) qDebug() << "Connection succeeded !!!";
    }
}
