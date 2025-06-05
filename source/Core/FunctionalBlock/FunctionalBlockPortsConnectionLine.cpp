#include <FunctionalBlockPortsConnectionLine.h>
#include <FunctionalBlockExecutionEnvironment.h>

void FunctionalBlockPortsConnectionLine::draw_start(){}

void FunctionalBlockPortsConnectionLine::draw_process()
{
    if(get_source() == nullptr ||
        get_target() == nullptr)
        return;

    auto graph = get_source()->get_parent_recursive<FunctionalBlockExecutionEnvironment>();

    //ImGui::GetWindowDrawList()->AddRect(
    //    rect.GetTL(),
    //    rect.GetBR(),
    //    m_Color,
    //    0.f,
    //    ImDrawFlags_::ImDrawFlags_None,
    //    m_MouseCatcher.Contains(executionEnvironment->get_mouse_scene_position()) ||
    //            get_parent<FunctionalBlockExecutionEnvironment::SelectionNode>() ? 16.f : 4.f
    //    );

    auto source = get_source()->get_geometry().get_rect().GetCenter();
    auto target = get_target()->get_geometry().get_rect().GetCenter();

    ImGui::GetWindowDrawList()->AddLine(
        graph->get_item_scene_position(source),
        graph->get_item_scene_position(target),
        IM_COL32(0, 255, 0, 255));

    // use with:
    //ImVec2 mouse_pos_projected_on_segment = ImLineClosestPoint(segment_a, segment_b, mouse_pos);
    //ImVec2 mouse_pos_delta_to_segment = mouse_pos_projected_on_segment - mouse_pos;
    //bool is_segment_hovered = (ImLengthSqr(mouse_pos_delta_to_segment ) <= max_distance * max_distance);
}

void FunctionalBlockPortsConnectionLine::draw_finish(){}
