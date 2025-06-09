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

class FactoryObjectRect
{
public:

    FactoryObjectRect(
        glm::vec2 _TopLeft     = glm::vec2(), 
        glm::vec2 _BottomRight = glm::vec2(),
        glm::vec2 _Pivot       = glm::vec2(0.0f, 0.0f))
    {
        m_Origin[FactoryObjectRectCorners::TopLeft]     = glm::vec4(_TopLeft.x, _TopLeft.y, 0.f, 1.f);
        m_Origin[FactoryObjectRectCorners::TopRight]    = glm::vec4(_BottomRight.x, _TopLeft.y, 0.f, 1.f);
        m_Origin[FactoryObjectRectCorners::BottomRight] = glm::vec4(_BottomRight.x, _BottomRight.y, 0.f, 1.f);
        m_Origin[FactoryObjectRectCorners::BottomLeft]  = glm::vec4(_TopLeft.x, _BottomRight.y, 0.f, 1.f);

        for(int i = 0; i < 4; i++) 
            m_Transformed[i] = m_Origin[i];

        // setup pivot
        set_pivot(_Pivot);
    }
    
    virtual ~FactoryObjectRect(){}

    // getters
    glm::vec2 get_top_left() const
    {
        return m_Origin[FactoryObjectRectCorners::TopLeft];
    }

    glm::vec2 get_top_right() const
    {
        return m_Origin[FactoryObjectRectCorners::TopRight];
    }

    glm::vec2 get_bottom_left() const
    {
        return m_Origin[FactoryObjectRectCorners::BottomLeft];
    }

    glm::vec2 get_bottom_right() const
    {
        return m_Origin[FactoryObjectRectCorners::BottomRight];
    }

    glm::vec2 get_minimum() const
    {
        return get_top_left();
    }

    glm::vec2 get_maximum() const
    {
        return get_bottom_right();
    }

    glm::vec2 get_center() const
    {
        return (get_minimum() + get_maximum()) * 0.5f;
    }

    // setters
    void set_pivot(glm::vec2 _Pivot)
    {
        // setup value
        m_Pivot = _Pivot;

        // recompute coordinates
        auto pivotOffset = get_pivot_offset();

        for(int i = 0; i < FactoryObjectRectCorners::BottomRight + 1; i++)
        {
            m_Transformed[i] = 
                glm::translate(glm::mat4(1.f), glm::vec3(-pivotOffset.x, -pivotOffset.y, 0.f)) * m_Origin[i];
        }
    }

    // API
    void rotate(float _Angle, glm::vec3 _Axis)
    {
        auto pivotPoint  = get_pivot_point();
        auto pivotOffset = get_pivot_offset();

        for(int i = 0; i < FactoryObjectRectCorners::BottomRight + 1; i++)
        {
            m_Transformed[i] = 
                glm::translate(glm::mat4(1.f), glm::vec3(-pivotOffset.x, -pivotOffset.y, 0.f)) * 
                glm::translate(glm::mat4(1.f), glm::vec3(pivotPoint.x, pivotPoint.y, 0.f))   * 
                glm::rotate(glm::mat4(1.f), _Angle, _Axis)   * 
                glm::translate(glm::mat4(1.f), glm::vec3(-pivotPoint.x, -pivotPoint.y, 0.f)) * m_Origin[i];
        }
    }

    void render(ImU32 _Color)
    {
        // draw pivot
        auto pivot  = get_pivot_point() - (get_maximum() - get_minimum()) * m_Pivot;
        ImGui::GetWindowDrawList()->AddCircleFilled(ImVec2(pivot.x, pivot.y), 16.f, _Color, 32);

        // draw rect
        ImGui::GetWindowDrawList()->AddQuad(
            ImVec2(m_Transformed[FactoryObjectRectCorners::TopLeft].x, m_Transformed[FactoryObjectRectCorners::TopLeft].y), 
            ImVec2(m_Transformed[FactoryObjectRectCorners::TopRight].x, m_Transformed[FactoryObjectRectCorners::TopRight].y), 
            ImVec2(m_Transformed[FactoryObjectRectCorners::BottomRight].x, m_Transformed[FactoryObjectRectCorners::BottomRight].y),
            ImVec2(m_Transformed[FactoryObjectRectCorners::BottomLeft].x, m_Transformed[FactoryObjectRectCorners::BottomLeft].y), 
            _Color
        );
    }

protected:

    // nested types
    enum FactoryObjectRectCorners
    {
        TopLeft,
        TopRight,
        BottomLeft,
        BottomRight,
    };

    // info
    glm::vec4 m_Origin[4] = 
    {
        glm::vec4(0.f, 0.f, 0.f, 0.f),
        glm::vec4(0.f, 0.f, 0.f, 0.f),
        glm::vec4(0.f, 0.f, 0.f, 0.f),
        glm::vec4(0.f, 0.f, 0.f, 0.f),
    };

    glm::vec4 m_Transformed[4] = 
    {
        glm::vec4(0.f, 0.f, 0.f, 0.f),
        glm::vec4(0.f, 0.f, 0.f, 0.f),
        glm::vec4(0.f, 0.f, 0.f, 0.f),
        glm::vec4(0.f, 0.f, 0.f, 0.f),
    };

    glm::vec2 m_Pivot = 
    {
        0.5f, 
        0.5f
    };

    // service methods
    glm::vec2 get_pivot_point() const
    {
        return (get_top_left() + (get_maximum() - get_minimum())* m_Pivot);
    }

    glm::vec2 get_pivot_offset() const
    {
        return (get_maximum() - get_minimum())* m_Pivot;
    }
};