#include <FunctionalBlockExecutionEnvironment.h>

#include <Singleton.h>

// FunctionalBlockExecutionEnvironment
FunctionalBlockExecutionEnvironment::FunctionalBlockExecutionEnvironment(
    FactoryObjectHierarchy*   _Parent,
    IDynamicGraphNodeCreator* _NodeCreator,
    IDynamicGraphEdgeCreator* _EdgeCreator) :
    DynamicGraph(_Parent, _NodeCreator, _EdgeCreator),
    m_Selection(new SelectionNode(get_nodes_root()))
{
    // set name
    this->set_name(STRINGIFY(FunctionalBlockExecutionEnvironment));
}

FunctionalBlockExecutionEnvironment::~FunctionalBlockExecutionEnvironment(){}

ImVec2 FunctionalBlockExecutionEnvironment::get_mouse_scene_position() const
{
    auto geometry = get_geometry();
    return (ImGui::GetIO().MousePos - geometry.get_origin() - geometry.get_offset()) / geometry.get_scale();
}

ImVec2 FunctionalBlockExecutionEnvironment::get_item_scene_position(ImVec2 _Position) const
{
    auto geometry = get_geometry();

    return _Position * geometry.get_scale() + geometry.get_origin() + geometry.get_offset();
};

float FunctionalBlockExecutionEnvironment::get_grid_size() const
{
    return m_GridSize;
}

pugi::xml_node FunctionalBlockExecutionEnvironment::pugi_serialize(pugi::xml_node& _Parent)
{
    auto xelement = _Parent.append_child(STRINGIFY(FunctionalBlockExecutionEnvironment));
    auto children = this->get_nodes_root()->get_children();

    for(auto child : children)
    {
        FunctionalBlock* object = dynamic_cast< FunctionalBlock* >(child);

        IPugiXMLSerializable* serializable =
            object != nullptr ?
                dynamic_cast<IPugiXMLSerializable*>(object) :
                    nullptr;

        if(object == nullptr ||
            serializable == nullptr)
            continue;
    }

    return xelement;
}

bool FunctionalBlockExecutionEnvironment::pugi_deserialize(pugi::xml_node& _Node)
{
    if(_Node.empty())
        return false;

    // retrieve elements creator components
    std::map<std::string, FunctionalBlockFactory::IFunctionalBlockCreator*> creators =
        Singleton<FunctionalBlockFactory>::Instance()->get_creators();

    // semirecursive search
    std::queue<std::pair<pugi::xml_node, FactoryObjectHierarchy*>> queue;
    std::vector<std::pair<pugi::xml_node, FactoryObjectHierarchy*>> vector;
    queue.push({_Node, this});

    while(!queue.empty())
    {
        auto top = queue.front();
        queue.pop();

        // try to create item
        auto creator = creators.find( top.first.attribute("TYPE").value() );

        if(creator != creators.end() &&
            creator->second != nullptr)
        {
            vector.push_back({top.first, creator->second->Create((top.second == nullptr ? this : top.second), top.first)});
        }
        else
        {
            for(auto i = top.first.begin(); i != top.first.end(); i++)
                queue.push({*i, top.second});
        }
    }

    for(auto pair : vector)
    {
        auto document = pair.first;
        auto object   = dynamic_cast<IPugiXMLSerializable*>(pair.second);

        if(object != nullptr && !document.empty())
            object->pugi_deserialize(document);
    }

    return true;
}

#include <QDebug>

void FunctionalBlockExecutionEnvironment::draw_start()
{
    ImGui::Begin(get_name().c_str());
    ImGuiIO& io = ImGui::GetIO();

    // retrieve geometry
    auto origin = ImGui::GetCursorScreenPos();
    auto size   = ImVec2(std::max(ImGui::GetContentRegionAvail().x, m_GridSize), std::max(ImGui::GetContentRegionAvail().y, m_GridSize));
    auto offset = get_geometry().get_offset();
    auto scale  = get_geometry().get_scale();

    // draw invisible button to handle mouse and key events
    ImGui::InvisibleButton("Scene2D", size, ImGuiButtonFlags_MouseButtonMask_);

    // drag
    if (ImGui::IsMouseDragging(ImGuiMouseButton_Middle))
        offset += io.MouseDelta;

    // zoom
    if (io.MouseWheel != 0)
        scale = ImClamp(scale + io.MouseWheel * scale / 16.f, 0.0001f, 100.f);

    // setup geometry
    set_geometry(Geometry(origin, size, offset, scale));
}

void FunctionalBlockExecutionEnvironment::draw_process()
{
    auto drawList = ImGui::GetWindowDrawList();
    auto geometry = get_geometry();

    // compute local mouse position
    auto mousePosition = (ImGui::GetIO().MousePos - geometry.get_origin() - geometry.get_offset()) / geometry.get_scale();

    // push clipping rect
    drawList->PushClipRect(
        geometry.get_rect().GetTL(),
        geometry.get_rect().GetBR(), true);

    // draw background
    drawList->AddRectFilled(
        geometry.get_rect().GetTL(),
        geometry.get_rect().GetBR(),
        IM_COL32(64, 64, 64, 255));

    drawList->AddRect(
        geometry.get_rect().GetTL(),
        geometry.get_rect().GetBR(),
        IM_COL32(0, 255, 0, 255));

    auto offset = geometry.get_offset();

    auto gridSize = m_GridSize * geometry.get_scale();

    for (float x = fmodf(offset.x, gridSize); x < geometry.get_size().x; x += gridSize)
    {
        drawList->AddLine(
            ImVec2(geometry.get_rect().GetTL().x + x, geometry.get_rect().GetTL().y),
            ImVec2(geometry.get_rect().GetTL().x + x, geometry.get_rect().GetBR().y),
            IM_COL32(200, 200, 200, 40));
    }

    for (float y = fmodf(offset.y, gridSize); y < geometry.get_size().y; y += gridSize)
    {
        drawList->AddLine(
            ImVec2(geometry.get_rect().GetTL().x, geometry.get_rect().GetTL().y + y),
            ImVec2(geometry.get_rect().GetTR().x, geometry.get_rect().GetTL().y + y),
            IM_COL32(200, 200, 200, 40));
    }

    // draw cursor
    drawList->AddText(
        ImGui::GetIO().MousePos,
        IM_COL32(0, 255, 0, 255),
        (std::to_string(mousePosition.x) + " " + std::to_string(mousePosition.y)).c_str());

    // draw items
    bool clearSelection = false;

    apply_function_to_children_recursuve(
        [this, &mousePosition, &clearSelection](FactoryObject* _Object)
        {
            FunctionalBlock* currentObject =
                dynamic_cast<FunctionalBlock*>(_Object);

            if(currentObject == nullptr)
                return;

            // catch object mouse events
            currentObject->draw();

            // select object
            if(currentObject->m_MouseEvent.button == ImGuiMouseButton_::ImGuiMouseButton_Left &&
                currentObject->m_MouseEvent.type  == FunctionalBlock::MouseEvent::Type::Click)
            {
                m_MouseGrabber = currentObject;

                if(!ImGui::IsKeyDown(ImGuiKey::ImGuiKey_LeftCtrl) && m_MouseGrabber != nullptr)
                    clearSelection = true;
            }

            if(ImGui::IsKeyReleased(ImGuiKey::ImGuiKey_LeftCtrl) &&
                ImGui::IsMouseDown(ImGuiMouseButton_::ImGuiMouseButton_Left))
                clearSelection = true;

            if(ImGui::IsKeyPressed(ImGuiKey::ImGuiKey_Escape))
                clearSelection = true;

            // move object
            if(currentObject->m_MouseEvent.button == ImGuiMouseButton_::ImGuiMouseButton_Left &&
                currentObject->m_MouseEvent.type  == FunctionalBlock::MouseEvent::Type::Down &&
                m_MouseGrabber == nullptr)
                m_MouseGrabber = currentObject;

            // open context menu
            if(currentObject->m_MouseEvent.button == ImGuiMouseButton_::ImGuiMouseButton_Right)
            {
            }
        }
        );

    // move items and select mouse grabber item
    if(m_MouseGrabber != nullptr)
    {
        // move items
        auto targetPosition = mousePosition - m_MouseGrabber->get_geometry().get_size() * 0.5f;
        auto translation    = m_MouseGrabber->get_geometry().get_origin() - targetPosition;

        m_Selection->apply_function_to_children_recursuve(
            [this, &translation](FactoryObjectHierarchy* _Object)
            {
                FunctionalBlock* selectedObject =
                    dynamic_cast<FunctionalBlock*>(_Object);

                if(selectedObject == nullptr)
                    return;

                auto geometry = selectedObject->get_geometry();
                geometry.set_origin(geometry.get_origin() - translation);
                selectedObject->set_geometry(geometry);
            }
            );

        // select mouse grabber item
        m_MouseGrabber->set_parent(m_Selection);
    }

    // clear selected items list
    if(clearSelection)
    {
        auto children = m_Selection->get_children();
        for(auto child : children)
            child->set_parent(get_nodes_root());
    }

    // remove all selected items
    if(ImGui::IsKeyPressed(ImGuiKey::ImGuiKey_Delete))
    {
        auto children = m_Selection->get_children();

        for(auto child : children)
            delete child;
    }

    // handle mouse events
    for(size_t button = ImGuiMouseButton_::ImGuiMouseButton_Left;
         button < ImGuiMouseButton_::ImGuiMouseButton_Middle; button++)
    {
        // handle mouse click event
        if(ImGui::IsMouseClicked(button))
        {
            if(m_MouseGrabber == nullptr)
            {
                auto children = m_Selection->get_children();
                for(auto child : children)
                    child->set_parent(get_nodes_root());
            }
        }

        // handle mouse release event
        if(ImGui::IsMouseReleased(button))
        {
            m_MouseGrabber = nullptr;
        }
    }

    // pop clipping rect
    drawList->PopClipRect();
}

void FunctionalBlockExecutionEnvironment::draw_finish()
{
    ImGui::End();
}
