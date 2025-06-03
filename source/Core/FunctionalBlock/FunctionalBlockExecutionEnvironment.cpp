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

void FunctionalBlockExecutionEnvironment::draw()
{
    ImGui::Begin(get_name().c_str());

    ImGui::SetCursorScreenPos(ImVec2(0.f, 0.f));

    m_Origin = ImGui::GetCursorScreenPos();

    m_Size = ImVec2(
        std::max(ImGui::GetContentRegionAvail().x, m_GridSize),
        std::max(ImGui::GetContentRegionAvail().y, m_GridSize));

    m_Rect = ImRect(m_Origin, m_Origin + m_Size);

    // get draw list
    m_DrawList = ImGui::GetWindowDrawList();

    m_DrawList->ChannelsSplit(2);

    // push clipping rect
    m_DrawList->PushClipRect(m_Rect.GetTL(), m_Rect.GetBR(), true);

    // draw children
    CatchMouseButtons();
    DrawBackground();
    DrawText();

    // draw children and catch their mouse and key events
    std::vector<FunctionalBlock*> currentSelection;

    // find delta
    apply_function_to_children_recursuve(
        [this, &currentSelection](FactoryObject* _Object)
        {
            FunctionalBlock* focusObject =
                dynamic_cast<FunctionalBlock*>(_Object);

            if(focusObject == nullptr)
                return;

            // catch object mouse events
            focusObject->draw();

            // select object
            if(focusObject->m_MouseEvent.button == ImGuiMouseButton_::ImGuiMouseButton_Left &&
                focusObject->m_MouseEvent.type == FunctionalBlock::MouseEvent::Type::Click)
                currentSelection.push_back(focusObject);

            // move object
            if(focusObject->m_MouseEvent.button == ImGuiMouseButton_::ImGuiMouseButton_Left &&
                focusObject->m_MouseEvent.type == FunctionalBlock::MouseEvent::Type::Down)
            {
                auto targetPosition = SceneMousePosition() - focusObject->m_Size * m_Scale * 0.5f;
                auto translation    = focusObject->m_Origin - targetPosition;
                std::vector<FunctionalBlock*> objectsToMove;

                m_Selection->apply_function_to_children_recursuve(
                    [this, &translation, &focusObject, &objectsToMove](FactoryObjectHierarchy* _Object)
                    {
                        FunctionalBlock* selectedObject =
                            dynamic_cast<FunctionalBlock*>(_Object);

                        if(selectedObject == nullptr)
                            return;

                        auto delta = focusObject->m_Origin - selectedObject->m_Origin;

                        if(selectedObject != focusObject &&
                            sqrtf(delta.x * delta.x + delta.y + delta.y) < m_GridSize)
                        {
                            return;
                        }

                        objectsToMove.push_back(selectedObject);

                        //if(selectedObject != nullptr)
                        //    selectedObject->setGeometry(selectedObject->m_Origin - translation, selectedObject->m_Size);
                    }
                    );

                for(auto objectToMove : objectsToMove)
                {
                    if(objectToMove != nullptr)
                        objectToMove->setGeometry(objectToMove->m_Origin - translation, objectToMove->m_Size);
                }
            }

            // open context menu
            if(focusObject->m_MouseEvent.button == ImGuiMouseButton_::ImGuiMouseButton_Right)
            {
            }
        }
    );

    // move currently selected objects into a special node
    for(auto selectedItem : currentSelection)
        selectedItem->set_parent(m_Selection);

    // clear selection
    // handle left mouse click
    if(ImGui::IsKeyPressed(ImGuiKey::ImGuiKey_Escape))
    {
        auto children = m_Selection->get_children();

        for(auto child : children)
            child->set_parent(get_nodes_root());
    }

    // handle left mouse click
    if(ImGui::IsMouseClicked(ImGuiMouseButton_::ImGuiMouseButton_Left))
    {
    }

    // handle right mouse click
    if(ImGui::IsMouseClicked(ImGuiMouseButton_::ImGuiMouseButton_Right))
    {
    }

    // handle middle mouse click
    if(ImGui::IsMouseClicked(ImGuiMouseButton_::ImGuiMouseButton_Middle))
    {
    }

    // handle left mouse release
    if(ImGui::IsMouseReleased(ImGuiMouseButton_::ImGuiMouseButton_Left))
    {
    }

    // handle right mouse release
    if(ImGui::IsMouseReleased(ImGuiMouseButton_::ImGuiMouseButton_Right))
    {
    }

    // handle middle mouse release
    if(ImGui::IsMouseReleased(ImGuiMouseButton_::ImGuiMouseButton_Middle))
    {
    }

    // pop clipping rect
    m_DrawList->PopClipRect();

    ImGui::End();
}
