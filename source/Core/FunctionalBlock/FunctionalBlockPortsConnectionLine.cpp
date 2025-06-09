#include <FunctionalBlockPortsConnectionLine.h>
#include <FunctionalBlockExecutionEnvironment.h>

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

    static std::vector<ImVec2> build_rectangular_path(FunctionalBlockPort* source, FunctionalBlockPort* target)
    {
        return compute_path(
            source->get_world_rect(true).GetCenter(),
            target->get_world_rect(true).GetCenter(),
            generate_normals(source, source->get_orientation()),
            generate_normals(target, target->get_orientation()));
    }
};

// FunctionalBlockPortsConnectionLine
void FunctionalBlockPortsConnectionLine::draw()
{
    ImGui::GetWindowDrawList()->ChannelsSetCurrent(FunctionalBlockExecutionEnvironment::DrawChannels::Lines);

    auto source = get_source<FunctionalBlockPort>();
    auto target = get_target<FunctionalBlockPort>();

    if(source == nullptr || target == nullptr || !source->is_visible() || !target->is_visible())
        return;

    // update points
    auto worldTransform        = get_world_transform();
    auto inverseWorldTransform = glm::inverse(worldTransform);

    if(m_WorldPoints.empty() || m_LocalPoints.empty())
    {
        // compute path points in local coordinates
        m_LocalPoints = FunctionalBlockPortsConnectionLinePathBuilder::build_rectangular_path(source, target);

        // compute path points in world coordinates
        m_WorldPoints.resize(m_LocalPoints.size());
        for (size_t i = 0; i < m_LocalPoints.size(); i++) 
            m_WorldPoints[i] = FactoryObjectRenderer::transformPoint(m_LocalPoints[i], inverseWorldTransform);

        // identidfy path direction
        ImVec2 sourcePoint = source->get_world_rect(true).GetCenter();
        ImVec2 targetPoint = target->get_world_rect(true).GetCenter();
        m_Direction = ImLengthSqr(m_LocalPoints[0] - sourcePoint) < ImLengthSqr(m_LocalPoints[0] - targetPoint);
    }
    else
    {
        // compute path points in local coordinates
        for (size_t i = 0; i < m_WorldPoints.size(); i++)
        {
            auto vector = worldTransform * glm::vec4(m_WorldPoints[i].x, m_WorldPoints[i].y, 0.f, 1.f);
            m_LocalPoints[i] = ImVec2(vector.x, vector.y);
        }

        // translate path points
        ImVec2 sourcePoint = source->get_world_rect(true).GetCenter();
        ImVec2 targetPoint = target->get_world_rect(true).GetCenter();
        ImVec2 translation = (m_Direction ? sourcePoint : targetPoint) - m_LocalPoints[0];

        for(int i = 1; i < m_LocalPoints.size() - 1; i++) 
            m_LocalPoints[i] += translation;

        if(m_Direction)
        {
            m_LocalPoints[0] = sourcePoint;
            m_LocalPoints[m_LocalPoints.size() - 1] = targetPoint;
        }
        else
        {
            m_LocalPoints[0] = targetPoint;
            m_LocalPoints[m_LocalPoints.size() - 1] = sourcePoint;
        }
    }

    // catch mouse events
    bool  isHovered   = false;
    float maxDistance = 16.f;

    // identidfy if the line is hovered
    for(size_t i = 1; i < m_LocalPoints.size(); i++)
    {
        // catch mouse
        ImVec2 mousePositionProjectedOnSegment = ImLineClosestPoint(m_LocalPoints[i-0], m_LocalPoints[i-1], ImGui::GetIO().MousePos);
        ImVec2 mousePositionDeltaToSegment     = mousePositionProjectedOnSegment - ImGui::GetIO().MousePos;

        if((isHovered = (ImLengthSqr(mousePositionDeltaToSegment) <= maxDistance * maxDistance))) 
            break;
    }

    // identify the nearest to the mouse point
    int caughtPoint = -1;

    if(isHovered)
    {
        for(size_t i = 1; i < m_LocalPoints.size(); i++)
        {
            auto localPointBoundingRectSize = ImVec2(16.f, 16.f) * std::max(std::max(worldTransform[0][0], worldTransform[1][1]), worldTransform[2][2]);

            ImRect localPointBoundingRect = 
                ImRect(m_LocalPoints[i] - localPointBoundingRectSize * 0.5f, m_LocalPoints[i] + localPointBoundingRectSize);

            if(localPointBoundingRect.Contains(ImGui::GetIO().MousePos)) 
            {
                caughtPoint = i;
                break;
            }
        }
    }

    // draw path
    ImGui::GetWindowDrawList()->AddPolyline(
        &m_LocalPoints[0],
        m_LocalPoints.size(),
        m_Color,
        ImDrawFlags_::ImDrawFlags_RoundCornersAll,
        isHovered || m_Selected ? m_FocusedThickness : m_DefaultThickness
    );

    // select line
    if(isHovered || m_Focused)
    {
        // select line
        if(ImGui::IsMouseClicked(ImGuiMouseButton_::ImGuiMouseButton_Left))
        {
            m_Selected = true;
            
            if(!m_Focused)
                m_CaughtPoint = caughtPoint;
        }

        // Popup context menu
        if (ImGui::BeginPopupContextItem())
        {
            if(!m_Focused)
                m_CaughtPoint = caughtPoint;
            
            m_Focused = true;

            // line editing  is available only in a non-smoothed mode
            if(ImGui::MenuItem("AddPoint")) 
                insert_line_point();
            
            // remove point
            if(ImGui::MenuItem("RemovePoint")) 
                remove_line_point();

            // remove self
            if(ImGui::MenuItem("RemoveLine")) 
                remove_line();

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
        {
            m_Selected = false;
            m_Focused  = false;
        }

        if(ImGui::IsKeyPressed(ImGuiKey::ImGuiKey_Escape))
        {
            m_Selected = false;
            m_Focused  = false;
        }
    }

    if(m_Selected)
    {
        // remove line
        if(ImGui::IsKeyPressed(ImGuiKey::ImGuiKey_Delete)) 
            remove_line();

        // move line point
        if(ImGui::IsMouseDown(ImGuiMouseButton_::ImGuiMouseButton_Left) && m_CaughtPoint >= 0 && m_CaughtPoint < m_WorldPoints.size())
        {
            m_WorldPoints[m_CaughtPoint] = 
                FactoryObjectRenderer::transformPoint(ImGui::GetIO().MousePos, inverseWorldTransform);
        }
    }

    // highlight the nearest point
    if(m_Focused)
    {
        if(m_CaughtPoint >= 0 && m_CaughtPoint < m_WorldPoints.size())
        {
            ImGui::GetWindowDrawList()->AddEllipseFilled(
                m_LocalPoints[m_CaughtPoint], 
                ImVec2(16.f, 16.f) * std::max(std::max(worldTransform[0][0], worldTransform[1][1]), worldTransform[2][2]), 
                m_Color
            );
        }
    }
    else
    {
        if(isHovered && caughtPoint >= 0 && caughtPoint < m_WorldPoints.size())
        {
            ImGui::GetWindowDrawList()->AddEllipseFilled(
                m_LocalPoints[caughtPoint], 
                ImVec2(16.f, 16.f) * std::max(std::max(worldTransform[0][0], worldTransform[1][1]), worldTransform[2][2]), 
                m_Color
            );
        }
    }
}

void FunctionalBlockPortsConnectionLine::remove_line()
{
    delete this;
}

void FunctionalBlockPortsConnectionLine::remove_line_point()
{
    if(m_CaughtPoint < 0 || m_CaughtPoint >= m_WorldPoints.size()) 
        return;

    m_WorldPoints.erase(m_WorldPoints.begin() + m_CaughtPoint);
    m_LocalPoints.erase(m_LocalPoints.begin() + m_CaughtPoint);
    m_CaughtPoint = -1;
}

void FunctionalBlockPortsConnectionLine::insert_line_point()
{
    // compute transform
    auto worldTransform        = get_world_transform();
    auto inverseWorldTransform = glm::inverse(worldTransform);

    // look for the point where to insert
    size_t caughtPoint = -1;
    float  minDistance = std::numeric_limits<float>::max();

    for(size_t i = 1; i < m_LocalPoints.size(); i++)
    {
        auto length = ImLengthSqr(ImVec2(m_LocalPoints[i] - ImGui::GetIO().MousePos));

        if(length < minDistance)
        {
            minDistance = length;
            caughtPoint = i;
        }
    }

    caughtPoint =
        std::max<size_t>(std::min(caughtPoint, m_LocalPoints.size() - 1), 1);

    if(caughtPoint < 0 || caughtPoint >= m_WorldPoints.size()) 
        return;

    // insert point into local coordinate system path
    m_LocalPoints.insert(m_LocalPoints.begin() + caughtPoint, ImGui::GetIO().MousePos);

    // compute path points in world coordinates
    m_WorldPoints.resize(m_LocalPoints.size());
    for (size_t i = 0; i < m_LocalPoints.size(); i++) 
        m_WorldPoints[i] = FactoryObjectRenderer::transformPoint(m_LocalPoints[i], inverseWorldTransform);
}