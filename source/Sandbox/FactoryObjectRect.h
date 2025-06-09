// imgui
# ifndef IMGUI_DEFINE_MATH_OPERATORS
#     define IMGUI_DEFINE_MATH_OPERATORS
# endif
#include <imgui.h>
#include <imgui_internal.h>

// glm
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/matrix_transform.hpp>

struct Rect
{
    static Rect transform(Rect _Rect, const glm::mat4& _Transform)
    {
        _Rect.TopLeft     = _Transform * glm::vec4(_Rect.TopLeft.x, _Rect.TopLeft.y, 0.f, 1.f);
        _Rect.TopRight    = _Transform * glm::vec4(_Rect.TopRight.x, _Rect.TopRight.y, 0.f, 1.f);
        _Rect.BottomLeft  = _Transform * glm::vec4(_Rect.BottomLeft.x, _Rect.BottomLeft.y, 0.f, 1.f);
        _Rect.BottomRight = _Transform * glm::vec4(_Rect.BottomRight.x, _Rect.BottomRight.y, 0.f, 1.f);

        return _Rect;
    }

    glm::vec3 TopLeft     = glm::vec3(0.f, 0.f, 0.f);
    glm::vec3 TopRight    = glm::vec3(0.f, 0.f, 0.f);
    glm::vec3 BottomLeft  = glm::vec3(0.f, 0.f, 0.f);
    glm::vec3 BottomRight = glm::vec3(0.f, 0.f, 0.f);
};

class FactoryObjectRect
{
public:

    FactoryObjectRect(
        glm::vec2          _TopLeft     = glm::vec2(), 
        glm::vec2          _BottomRight = glm::vec2(),
        glm::vec2          _Pivot       = glm::vec2(0.0f, 0.0f), 
        FactoryObjectRect* _Parent      = nullptr)
    {
        // setup parent
        m_Parent = _Parent;

        // setup local rect
        m_Rect.TopLeft     = glm::vec4(_TopLeft.x, _TopLeft.y, 0.f, 1.f);
        m_Rect.TopRight    = glm::vec4(_BottomRight.x, _TopLeft.y, 0.f, 1.f);
        m_Rect.BottomRight = glm::vec4(_BottomRight.x, _BottomRight.y, 0.f, 1.f);
        m_Rect.BottomLeft  = glm::vec4(_TopLeft.x, _BottomRight.y, 0.f, 1.f);

        // setup pivot
        set_pivot(_Pivot);
    }
    
    virtual ~FactoryObjectRect(){}

    // setters
    void set_pivot(glm::vec2 _Pivot)
    {
        m_Pivot = _Pivot;
    }

    // API
    void rotate(float _Angle, glm::vec3 _Axis)
    {
        m_RotationAngle = _Angle;
        m_RotationAxis  = _Axis;
    }

    void render(ImU32 _Color)
    {
        // draw pivot point
        auto pivot  = get_pivot_point() - get_pivot_offset();
        ImGui::GetWindowDrawList()->AddCircleFilled(ImVec2(pivot.x, pivot.y), 16.f, _Color, 32);

        // draw rect
        auto worldTransformedRect = get_world_rect_transformed();

        ImGui::GetWindowDrawList()->AddQuad(
            ImVec2(worldTransformedRect.TopLeft.x, worldTransformedRect.TopLeft.y), 
            ImVec2(worldTransformedRect.TopRight.x, worldTransformedRect.TopRight.y), 
            ImVec2(worldTransformedRect.BottomRight.x, worldTransformedRect.BottomRight.y),
            ImVec2(worldTransformedRect.BottomLeft.x, worldTransformedRect.BottomLeft.y), 
            _Color
        );
    }

protected:

    glm::vec2 m_Pivot = 
    {
        0.5f, 
        0.5f
    };

    glm::vec3 m_RotationAxis  = glm::vec3(0.f, 0.f, 1.f);
    float     m_RotationAngle = 0.f;

    FactoryObjectRect* m_Parent = nullptr;

    // service methods
    Rect get_world_rect() const
    {
        if(m_Parent != nullptr)
        {
            auto parentWorldRectTransformed = m_Parent->get_world_rect_transformed();

            Rect resultingRect;
            resultingRect.TopLeft     = m_Rect.TopLeft + parentWorldRectTransformed.TopLeft;
            resultingRect.TopRight    = m_Rect.TopRight + parentWorldRectTransformed.TopLeft;
            resultingRect.BottomRight = m_Rect.BottomRight + parentWorldRectTransformed.TopLeft;
            resultingRect.BottomLeft  = m_Rect.BottomLeft + parentWorldRectTransformed.TopLeft;

            return resultingRect;
        }

        return m_Rect;
    }

    Rect get_world_rect_transformed() const
    {
        auto pivotPoint  = get_pivot_point();
        auto pivotOffset = get_pivot_offset();

        auto rect = get_world_rect();

        return Rect::transform(
            get_world_rect(),
            glm::translate(glm::mat4(1.f), glm::vec3(-pivotOffset.x, -pivotOffset.y, 0.f)) * 
            glm::translate(glm::mat4(1.f), glm::vec3(pivotPoint.x, pivotPoint.y, 0.f))   * 
            glm::rotate(glm::mat4(1.f), m_RotationAngle, m_RotationAxis)   * 
            glm::translate(glm::mat4(1.f), glm::vec3(-pivotPoint.x, -pivotPoint.y, 0.f)));
    }

    glm::vec2 get_pivot_point() const
    {
        auto worldRect = get_world_rect();

        auto topLeft = glm::vec2(worldRect.TopLeft.x, worldRect.TopLeft.y);
        auto minimum = glm::vec2(worldRect.TopLeft.x, worldRect.TopLeft.y);
        auto maximum = glm::vec2(worldRect.BottomRight.x, worldRect.BottomRight.y);

        return topLeft + (maximum - minimum) * m_Pivot;
    }

    glm::vec2 get_pivot_offset() const
    {
        auto worldRect = get_world_rect();

        auto minimum = glm::vec2(worldRect.TopLeft.x, worldRect.TopLeft.y);
        auto maximum = glm::vec2(worldRect.BottomRight.x, worldRect.BottomRight.y);

        return (maximum - minimum) * m_Pivot;
    }

private:

    mutable Rect m_Rect;
};