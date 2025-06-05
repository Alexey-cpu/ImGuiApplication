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

/*
float FunctionalBlockExecutionEnvironment::get_grid_size() const
{
    return m_GridSize;
}

ImVec2 FunctionalBlockExecutionEnvironment::get_mouse_scene_position() const
{
    //auto geometry = get_geometry();
    return ImVec2();//(ImGui::GetIO().MousePos - geometry.get_origin() - geometry.get_offset()) / geometry.get_scale();
}

ImVec2 FunctionalBlockExecutionEnvironment::get_item_scene_position(ImVec2 _Position) const
{
    return ImVec2();

    //auto geometry = get_geometry();

    //return _Position * geometry.get_scale() + geometry.get_origin() + geometry.get_offset();
};
*/

void FunctionalBlockExecutionEnvironment::draw_start(const glm::mat4& _Transform)
{
    // retrieve IO
    ImGuiIO& io = ImGui::GetIO();

    // configure scene
    ImGui::Begin(get_name().c_str());
    ImGui::SetCursorScreenPos(ImVec2(0.f, 0.f));

    auto origin = ImGui::GetCursorScreenPos();
    auto size   = ImVec2(std::max(ImGui::GetContentRegionAvail().x, 64.f), std::max(ImGui::GetContentRegionAvail().y, 64.f));

    ImGui::InvisibleButton(
        "Scene2D",
        size,
        ImGuiButtonFlags_::ImGuiButtonFlags_MouseButtonMask_);

    ImGui::GetWindowDrawList()->ChannelsSplit(FunctionalBlockExecutionEnvironment::DrawChannels::Last);
    ImGui::GetWindowDrawList()->ChannelsSetCurrent(FunctionalBlockExecutionEnvironment::DrawChannels::Main);

    // compute transformation matrix
    glm::mat4 transform = get_transform();

    // drag
    if (ImGui::IsMouseDragging(ImGuiMouseButton_Middle))
        transform = glm::translate(transform, glm::vec3(io.MouseDelta.x, io.MouseDelta.y, 0.0f));

    // zoom
    if (io.MouseWheel != 0)
    {
        float scale = std::max(std::max(transform[0][0], transform[1][1]), transform[2][2]);
        scale = ImClamp(scale + io.MouseWheel * scale / 16.f, 0.0001f, 100.f);
        transform[0][0] = scale;
        transform[1][1] = scale;
        transform[2][2] = scale;
    }

    // setup geometry and transformation matrix
    set_rect(ImRect(origin, origin + size));
    set_transformation(transform);
}

void FunctionalBlockExecutionEnvironment::draw_process(const glm::mat4& _Transform)
{
    auto drawList = ImGui::GetWindowDrawList();

    // push clipping rect
    drawList->PushClipRect(
        get_rect().GetTL(),
        get_rect().GetBR(), true);

    // draw background
    drawList->AddRectFilled(
        get_rect().GetTL(),
        get_rect().GetBR(),
        IM_COL32(64, 64, 64, 255));

    drawList->AddRect(
        get_rect().GetTL(),
        get_rect().GetBR(),
        IM_COL32(0, 255, 0, 255));

    // draw grid
    auto rect      = get_rect();
    auto transform = get_transform();
    auto offset    = ImVec2(transform[3][0], transform[3][1]);
    auto scale     = std::max(std::max(transform[0][0], transform[1][1]), transform[2][2]);
    auto gridSize  = m_GridSize * scale;

    for (float x = fmodf(offset.x, gridSize); x < rect.GetSize().x; x += gridSize)
    {
        drawList->AddLine(
            ImVec2(get_rect().GetTL().x + x, get_rect().GetTL().y),
            ImVec2(get_rect().GetTL().x + x, get_rect().GetBR().y),
            IM_COL32(200, 200, 200, 40));
    }

    for (float y = fmodf(offset.y, gridSize); y < rect.GetSize().y; y += gridSize)
    {
        drawList->AddLine(
            ImVec2(get_rect().GetTL().x, get_rect().GetTL().y + y),
            ImVec2(get_rect().GetTR().x, get_rect().GetTL().y + y),
            IM_COL32(200, 200, 200, 40));
    }

    // draw cursor
    glm::mat4 mouseTransform = glm::mat4(1.f);
    ImVec2    mousePostion   = (ImGui::GetIO().MousePos - offset) / scale;

    drawList->AddText(
        ImGui::GetMousePos(),
        IM_COL32(0, 255, 0, 255),
        (std::to_string(mousePostion.x) + " " + std::to_string(mousePostion.y)).c_str());

    // draw mouse grabber port connection line
    if(m_MouseGrabberPort != nullptr)
    {
        drawList->AddLine(
            m_MouseGrabberPort->get_rect(true).GetCenter(),
            ImGui::GetIO().MousePos,
            IM_COL32(255, 0, 0, 255));
    }

    // draw items
    bool clearSelection = false;

    apply_function_to_children_recursuve(
        [this, &clearSelection](FactoryObject* _Object)
        {
            FunctionalBlock* currentObject =
                dynamic_cast<FunctionalBlock*>(_Object);

            if(currentObject == nullptr)
                return;

            // catch object mouse events
            currentObject->draw(get_transform());

            // select object
            if(currentObject->m_MouseEvent.button == ImGuiMouseButton_::ImGuiMouseButton_Left &&
                currentObject->m_MouseEvent.type  == FunctionalBlock::MouseEvent::Type::Click)
            {
                m_MouseGrabberBlock = currentObject;

                if(!ImGui::IsKeyDown(ImGuiKey::ImGuiKey_LeftCtrl) && m_MouseGrabberBlock != nullptr)
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
                m_MouseGrabberBlock == nullptr)
                m_MouseGrabberBlock = currentObject;

            // open context menu
            if(currentObject->m_MouseEvent.button == ImGuiMouseButton_::ImGuiMouseButton_Right)
            {
            }
        }
        );

    // move items and select mouse grabber item
    if(m_MouseGrabberBlock != nullptr)
    {
        // move items
        auto targetPosition = mousePostion - m_MouseGrabberBlock->get_rect().GetSize() * 0.5f;
        auto translation    = m_MouseGrabberBlock->get_rect().GetTL() - targetPosition;

        m_Selection->apply_function_to_children_recursuve(
            [this, &translation, &gridSize](FactoryObjectHierarchy* _Object)
            {
                FunctionalBlock* selectedObject =
                    dynamic_cast<FunctionalBlock*>(_Object);

                if(selectedObject == nullptr)
                    return;

                auto geometry = selectedObject->get_rect();
                auto origin   = geometry.GetTL() - translation;
                auto size     = geometry.GetSize();

                origin = ImVec2(std::roundf(origin.x / m_GridSize) * m_GridSize,
                                std::roundf(origin.y / m_GridSize) * m_GridSize);

                selectedObject->set_rect(ImRect(origin, origin + size));
            }
            );

        // select mouse grabber item
        m_MouseGrabberBlock->set_parent(m_Selection);
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
            if(m_MouseGrabberBlock == nullptr)
            {
                auto children = m_Selection->get_children();
                for(auto child : children)
                    child->set_parent(get_nodes_root());
            }
        }

        // handle mouse release event
        if(ImGui::IsMouseReleased(button))
        {
            m_MouseGrabberBlock = nullptr;
            m_MouseGrabberPort  = nullptr;
        }
    }

    // pop clipping rect
    drawList->PopClipRect();
}

void FunctionalBlockExecutionEnvironment::draw_finish(const glm::mat4& _Transform)
{
    ImGui::End();
}
