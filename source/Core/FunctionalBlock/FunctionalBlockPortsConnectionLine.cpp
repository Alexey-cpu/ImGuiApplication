#include <FunctionalBlockPortsConnectionLine.h>
#include <FunctionalBlockExecutionEnvironment.h>

#include <QDebug>

// FunctionalBlockPortsConnectionLinePathBuilder
class FunctionalBlockPortsConnectionLinePathBuilder
{
public:

    static std::vector<ImVec4> generate_normals(FunctionalBlockPort* _To, FunctionalBlockPort::Orientation _Orientation)
    {
        if(_To == nullptr )
            return std::vector<ImVec4>();

        std::vector<ImVec4> normals;

        ImVec4 left = ImVec4(
            _To->get_world_rect(true).GetCenter().x,
            _To->get_world_rect(true).GetCenter().y,
            (_To->get_world_rect(true).GetCenter() - ImVec2(_To->get_world_rect(true).GetSize().x, 0.0)).x,
            (_To->get_world_rect(true).GetCenter() - ImVec2(_To->get_world_rect(true).GetSize().x, 0.0)).y
            );

        ImVec4 right = ImVec4(
            _To->get_world_rect(true).GetCenter().x,
            _To->get_world_rect(true).GetCenter().y,
            (_To->get_world_rect(true).GetCenter() + ImVec2(_To->get_world_rect(true).GetSize().x, 0.)).x,
            (_To->get_world_rect(true).GetCenter() + ImVec2(_To->get_world_rect(true).GetSize().x, 0.)).y
            );

        ImVec4 top = ImVec4(
            _To->get_world_rect(true).GetCenter().x,
            _To->get_world_rect(true).GetCenter().y,
            (_To->get_world_rect(true).GetCenter() - ImVec2(0.0, _To->get_world_rect(true).GetSize().y)).x,
            (_To->get_world_rect(true).GetCenter() - ImVec2(0.0, _To->get_world_rect(true).GetSize().y)).y
            );

        ImVec4 bottom = ImVec4(
            _To->get_world_rect(true).GetCenter().x,
            _To->get_world_rect(true).GetCenter().y,
            (_To->get_world_rect(true).GetCenter() + ImVec2(0.0, _To->get_world_rect(true).GetSize().y)).x,
            (_To->get_world_rect(true).GetCenter() + ImVec2(0.0, _To->get_world_rect(true).GetSize().y)).y
            );


        switch (_Orientation)
        {
        case FunctionalBlockPort::Orientation::Top:
            normals.push_back(top);
            break;

        case FunctionalBlockPort::Orientation::Bottom:
            normals.push_back(bottom);
            break;

        case FunctionalBlockPort::Orientation::Left:
            normals.push_back(left);
            break;

        case FunctionalBlockPort::Orientation::Right:
            normals.push_back(right);
            break;

        case FunctionalBlockPort::Orientation::Horizontal:
            normals.push_back(left);
            normals.push_back(right);
            break;

        case FunctionalBlockPort::Orientation::Vertical:
            normals.push_back(top);
            normals.push_back(bottom);
            break;

        case FunctionalBlockPort::Orientation::AllWays:
            normals.push_back(left);
            normals.push_back(right);
            normals.push_back(top);
            normals.push_back(bottom);
            break;
        }

        return normals;
    }

    static std::vector<ImVec2> compute_path(ImVec2 _Source, ImVec2 _Target, std::vector<ImVec4> _SourceNormals, std::vector<ImVec4> _TargetNormals)
    {
        auto containsLine = [](ImVec4 _Reference, ImVec4 _Check)->bool
        {
            ImVec2 refP1   = ImVec2(_Reference.x, _Reference.y);
            ImVec2 refP2   = ImVec2(_Reference.z, _Reference.w);
            ImVec2 checkP1 = ImVec2(_Check.x, _Check.y);
            ImVec2 checkP2 = ImVec2(_Check.z, _Check.w);

            bool c0 = std::abs((checkP1.x - refP1.x) * (refP2.y - refP1.y) - (checkP1.y - refP1.y) * (refP2.x - refP1.x)) < 0.001;
            bool c1 = std::abs((checkP2.x - refP1.x) * (refP2.y - refP1.y) - (checkP2.y - refP1.y) * (refP2.x - refP1.x)) < 0.001;

            auto p1 = ImRect(checkP1, checkP2).GetCenter();
            auto p2 = refP1;
            auto p3 = ImRect(checkP1, checkP2).GetCenter();
            auto p4 = refP2;


            auto a = ImLengthSqr(ImVec2((p2.x - p1.x), (p2.y - p1.y)));
            auto b = ImLengthSqr(ImVec2((p4.x - p3.x), (p4.y - p3.y)));

            return c0 && c1 && (a + b <= ImLengthSqr(ImVec2((refP2.x - refP1.x), (refP2.y - refP1.y))));
        };

        // build rect
        ImRect rect   = ImRect(_Source, _Target);
        float  height = rect.GetSize().y;
        float  width  = rect.GetSize().x;

        // generate paths
        const float delta = 64.0;

        // default path
        std::vector<ImVec2> defaultPath;
        defaultPath.push_back(_Source);
        defaultPath.push_back(_Target);

        // path1
        std::vector<ImVec2> path1;
        path1.push_back(_Source);
        path1.push_back(path1.back() + ImVec2(0.0, height));
        path1.push_back(_Target);

        // path2
        std::vector<ImVec2> path2;
        path2.push_back(_Source);
        path2.push_back(path2.back() + ImVec2(width, 0.0));
        path2.push_back(_Target);

        // path3
        std::vector<ImVec2> path3;
        path3.push_back(_Source);
        path3.push_back(path3.back() + ImVec2(0.0, height * 0.5));
        path3.push_back(path3.back() + ImVec2(width, 0.0));
        path3.push_back(_Target);

        // path4
        std::vector<ImVec2> path4;
        path4.push_back(_Source);
        path4.push_back(path4.back() + ImVec2(width * 0.5, 0.0));
        path4.push_back(path4.back() + ImVec2(0.0, height));
        path4.push_back(_Target);

        // path5
        std::vector<ImVec2> path5;
        path5.push_back(_Source);
        path5.push_back(path5.back() + ImVec2(0.0, (height > 0 ? height + delta : height - delta)));
        path5.push_back(path5.back() + ImVec2(width, 0.0));
        path5.push_back(_Target);

        // path6
        std::vector<ImVec2> path6;
        path6.push_back(_Source);
        path6.push_back(path6.back() + ImVec2(( width > 0 ? width + delta : width - delta ), 0.0));
        path6.push_back(path6.back() + ImVec2(0.0, height));
        path6.push_back(_Target);

        // path7
        std::vector<ImVec2> path7;
        path7.push_back(_Source);
        path7.push_back(path7.back() + ImVec2(0.0, (height > 0 ? -delta : +delta)));
        path7.push_back(path7.back() + ImVec2(width, 0.0));
        path7.push_back(_Target);

        // path8
        std::vector<ImVec2> path8;
        path8.push_back(_Source);
        path8.push_back(path8.back() - ImVec2( delta, 0.0));
        path8.push_back(path8.back() - ImVec2( 0.0, delta));
        path8.push_back(path8.back() + ImVec2((width > 0 ? width + delta : width - delta), 0.0));
        path8.push_back(_Target);

        // path9
        std::vector<ImVec2> path9;
        path9.push_back(_Source);
        path9.push_back(path9.back() + ImVec2(0.5 * width, 0.0));
        path9.push_back(path9.back() + ImVec2(0.0, (height > 0 ? height + delta : height - delta)));
        path9.push_back(path9.back() + ImVec2(0.5 * width, 0.0));
        path9.push_back(_Target);

        // path10
        std::vector<ImVec2> path10;
        path10.push_back(_Source);
        path10.push_back(path10.back() + ImVec2(0.0, height > 0 ? delta : -delta));
        path10.push_back(path10.back() + ImVec2(( width > 0 ? width + delta : width - delta ), 0.0));
        path10.push_back(path10.back() + ImVec2(0.0, (height > 0 ? height - delta : height + delta )));
        path10.push_back(_Target);

        // paths
        std::list<std::vector<ImVec2>> paths;
        paths.push_back(defaultPath);
        paths.push_back(path1);
        paths.push_back(path2);
        paths.push_back(path3);
        paths.push_back(path4);
        paths.push_back(path5);
        paths.push_back(path6);
        paths.push_back(path7);
        paths.push_back(path8);
        paths.push_back(path9);
        paths.push_back(path10);

        // choose the path
        std::list<std::vector<ImVec2>> finishPaths;

        for(auto& path : paths)
        {
            bool containsSourceNormal = false;
            bool containsTargetNormal = false;

            for(int i = 1; i < path.size(); i++)
            {
                ImVec2 p1 = path[i-1];
                ImVec2 p2 = path[i-0];
                ImVec4 ll = ImVec4(p1.x, p1.y, p2.x, p2.y);

                for(auto& normal : _SourceNormals)
                {
                    if(!containsSourceNormal)
                        containsSourceNormal = containsLine(ll, normal);
                    else
                        break;
                }

                // check target normals
                for(auto normal : _TargetNormals)
                {
                    if(!containsTargetNormal)
                        containsTargetNormal = containsLine(ll, normal);
                    else
                        break;
                }
            }

            if(containsSourceNormal && containsTargetNormal)
                finishPaths.push_back(path);
        }

        auto shortestPath = std::min_element(
            finishPaths.begin(),
            finishPaths.end(),
            [](std::vector<ImVec2> _A, std::vector<ImVec2> _B)->bool
            {
                return _A.size() < _B.size();
            }
            );

        return finishPaths.empty() ? defaultPath : *shortestPath;
    }

    static std::vector<ImVec2> build_rectangular_path(
        FunctionalBlockPort* source,
        FunctionalBlockPort* target)
    {
        return compute_path(
            source->get_world_rect(true).GetCenter(),
            target->get_world_rect(true).GetCenter(),
            generate_normals(source, source->get_orientation()),
            generate_normals(target, target->get_orientation()));
    }

    // static std::vector<ImVec2> insertPoint(QPointF _ScenePosition)
    // {
    //     // catch cursor
    //     if(m_Catcher.CatchSingleExistingPathPoint(_ScenePosition))
    //         return;

    //     auto  referencePoint                      = this->mapFromScene(_ScenePosition);
    //     qreal minDistanceToReferencePoint         = __huge__<qreal>();
    //     int   minDistanceToReferencePointElement  = 0;
    //     auto  path                                = this->path();

    //     // rebuild path
    //     for(int i = 0 ; i < path.elementCount(); i++)
    //     {
    //         qreal lenght = QLineF(path.elementAt(i), referencePoint).length();

    //         if(lenght <= minDistanceToReferencePoint)
    //         {
    //             minDistanceToReferencePoint        = lenght;
    //             minDistanceToReferencePointElement = i;
    //         }
    //     }

    //     minDistanceToReferencePointElement =
    //         __max__(__min__(minDistanceToReferencePointElement, path.elementCount() - 1), 1);

    //     // generate new path
    //     QPainterPath newPath(path.elementAt(0));

    //     for(int i = 1; i < minDistanceToReferencePointElement; i++)
    //         newPath.lineTo(path.elementAt(i));
    //     newPath.lineTo(referencePoint);

    //     for(int i = minDistanceToReferencePointElement; i < path.elementCount(); i++)
    //         newPath.lineTo(path.elementAt(i));

    //     this->setPath(newPath);

    //     // catch once again
    //     if(m_Catcher.CatchSingleExistingPathPoint(_ScenePosition))
    //         this->update();
    // }
};

// FunctionalBlockPortsConnectionLine
void FunctionalBlockPortsConnectionLine::draw_process(const glm::mat4& _Transform)
{
    auto source = get_source<FunctionalBlockPort>();
    auto target = get_target<FunctionalBlockPort>();

    if(source == nullptr ||
        target == nullptr)
        return;

    // automatically build path if it's empty or translate it's pojnts if it's not empty
    if(m_Points.empty())
    {
        m_Points = FunctionalBlockPortsConnectionLinePathBuilder::build_rectangular_path(source, target);
    }
    else
    {
        // apply transform to points
        // for(size_t i = 0; i < m_Points.size(); i++)
        // {
        //     auto vector = _Transform * glm::vec4(m_Points[i].x, m_Points[i].y, 0.f, 1.f);
        //     m_Points[i] = ImVec2(vector.x, vector.y);
        // }

        ImVec2 sourcePoint = source->get_world_rect(true).GetCenter();
        ImVec2 targetPoint = target->get_world_rect(true).GetCenter();
        ImVec2 translation = (ImLengthSqr(m_Points[0] - sourcePoint) < ImLengthSqr(m_Points[0] - targetPoint) ? sourcePoint: targetPoint) - m_Points.front();

        for(int i = 1; i < m_Points.size() - 1; i++) 
            m_Points[i] += translation;

        if(ImLengthSqr(m_Points.front() - sourcePoint) <
            ImLengthSqr(m_Points.back() - sourcePoint))
        {
            m_Points[0] = sourcePoint;
            m_Points[m_Points.size() - 1] = targetPoint;
        }
        else
        {
            m_Points[0] = targetPoint;
            m_Points[m_Points.size() - 1] = sourcePoint;
        }
    }

    // catch mouse events
    bool  is_segment_hovered = false;
    float max_distance       = 16.f;

    for(size_t i = 1; i < m_Points.size(); i++)
    {
        // catch mouse
        ImVec2 mouse_pos_projected_on_segment = ImLineClosestPoint(m_Points[i-0], m_Points[i-1], ImGui::GetIO().MousePos);
        ImVec2 mouse_pos_delta_to_segment = mouse_pos_projected_on_segment - ImGui::GetIO().MousePos;
        is_segment_hovered = (ImLengthSqr(mouse_pos_delta_to_segment) <= max_distance * max_distance);

        if(is_segment_hovered)
        {
            // highlight the nearest point point
            for (size_t j = 1; j < m_Points.size() - 1; j++)
            {
                if(ImLengthSqr(m_Points[j] - ImGui::GetIO().MousePos) < max_distance)
                {
                    ImGui::GetWindowDrawList()->AddEllipse(m_Points[j], ImVec2(16.f, 16.f), IM_COL32(0, 255, 0, 255));
                    break;
                }
            }
            

            break;
        }
    }

    // select line
    if(is_segment_hovered || m_Focused)
    {
        // select line
        if(ImGui::IsMouseClicked(ImGuiMouseButton_::ImGuiMouseButton_Left))
            m_Selected = true;

        // Popup context menu
        if (ImGui::BeginPopupContextItem())
        {
            m_Focused = true;

            if(ImGui::MenuItem("AddPoint"))
            {
            }
            
            // remove point
            if(ImGui::MenuItem("RemovePoint"))
            {   
            }

            // remove self
            if(ImGui::MenuItem("RemoveLine"))
            {
                delete this;
            }

            ImGui::EndPopup();
        }
        else
        {
            m_Focused = false;
        }
    }
    else
    {
        if(ImGui::IsMouseClicked(ImGuiMouseButton_::ImGuiMouseButton_Left))
            m_Selected = false;

        if(ImGui::IsKeyPressed(ImGuiKey::ImGuiKey_Escape))
        {
            m_Selected = false;
            m_Focused = false;
        }
    }

    // delete line
    if(m_Selected)
    {
        if(ImGui::IsKeyPressed(ImGuiKey::ImGuiKey_Delete))
        {
            // delete self
            delete this;
        }
    }

    // draw path
    ImGui::GetWindowDrawList()->AddPolyline(
        &m_Points[0],
        m_Points.size(),
        IM_COL32(0, 255, 0, 255),
        ImDrawFlags_::ImDrawFlags_None,
        is_segment_hovered || m_Selected ? 8.f : 1.f
        );

    // smooting
    // if(m_Points.size() >= 4)
    // {
    //     ImGui::GetWindowDrawList()->AddBezierCubic(
    //         m_Points[0],
    //         m_Points[1],
    //         m_Points[2],
    //         m_Points[3],
    //         IM_COL32(0, 255, 0, 255),
    //         is_segment_hovered || m_Selected ? 8.f : 1.f
    //         );
    // }

    // else if(m_Points.size() >= 3)
    // {
    //     ImGui::GetWindowDrawList()->AddBezierQuadratic(
    //         m_Points[0],
    //         m_Points[1],
    //         m_Points[2],
    //         IM_COL32(0, 255, 0, 255),
    //         is_segment_hovered || m_Selected ? 8.f : 1.f
    //         );
    // }
    // else
    // {
    //     ImGui::GetWindowDrawList()->AddPolyline(
    //         &m_Points[0],
    //         m_Points.size(),
    //         IM_COL32(0, 255, 0, 255),
    //         ImDrawFlags_::ImDrawFlags_None,
    //         is_segment_hovered || m_Selected ? 8.f : 1.f
    //         );
    // }
}
