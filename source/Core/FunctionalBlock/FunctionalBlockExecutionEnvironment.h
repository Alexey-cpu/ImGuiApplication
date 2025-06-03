#ifndef FUNCTIONALBLOCKEXECUTIONENVIRONMENT_H
#define FUNCTIONALBLOCKEXECUTIONENVIRONMENT_H

// custom includes
#include "FunctionalBlockPortsConnectionLine.h"
#include "FunctionalBlockExecutionContext.h"
#include "DynamicGraph.h"
//#include "DebugLog.h"
//#include "Singleton.h"

#include <imgui.h>

// FunctionalBlockExecutionEnvironment
class FunctionalBlockExecutionEnvironment :
    public DynamicGraph,
    public IPugiXMLSerializable
{
public:

    class SelectionNode : public FactoryObjectHierarchy
    {
    public:

        SelectionNode(FactoryObjectHierarchy* _Parent = nullptr) :
            FactoryObjectHierarchy("Selection", _Parent){}
    };

    // constructors
    FunctionalBlockExecutionEnvironment(
            FactoryObjectHierarchy*   _Parent      = nullptr,
            IDynamicGraphNodeCreator* _NodeCreator = nullptr,
            IDynamicGraphEdgeCreator* _EdgeCreator = new FunctionalBlockPortsConnectionLineCreator() );

    // virtual destructor
    virtual ~FunctionalBlockExecutionEnvironment();

    template<typename __type>
    bool process()
    {
        // check context
        FunctionalBlockExecutionContext<__type>* context =
                dynamic_cast<FunctionalBlockExecutionContext<__type>*>(this);

        if(context == nullptr)
        {
            this->abort();
            return false;
        }

        // preprocessing
        if(!preprocess<__type>())
        {
            this->abort();
            return false;
        }

        // validate blocks
        if(!context->ignore_validation())
        {
            if(!validate_blocks<__type>())
            {
                this->abort();
                return false;
            }
        }

        // execute blocks
        std::vector<DynamicGraphNode*> excecutionOrder = this->topological_sort();

        for(auto item : excecutionOrder)
        {
            FunctionalBlock* object =
                    dynamic_cast<FunctionalBlock*>(item);

            if(object == nullptr)
            {
                this->abort();
                return false;
            }

            if(!object->process<__type>() &&
                !context->ignore_processing_fail())
            {
                this->abort();
                return false;
            }
        }

        // postprocessing
        if(!postprocess<__type>())
        {
            this->abort();
            return false;
        }

        return true;
    }

    template<typename __type>
    bool validate_blocks()
    {
        if(this->get_nodes_root() == nullptr)
            return false;

        auto notValidFunctionalBlocks = this->get_nodes_root()->find_children_recursuve(
            [](FactoryObjectHierarchy* _Object)
            {
                FunctionalBlock* object =
                    dynamic_cast<FunctionalBlock*>(_Object);

                FunctionalBlockExecutionContext<__type>* context =
                    object != nullptr ?
                        dynamic_cast<FunctionalBlockExecutionContext<__type>*>(object) :
                        nullptr;

                return context != nullptr &&
                       !context->is_valid();
            }
            );

        return notValidFunctionalBlocks.empty();
    }

    template< typename __type >
    bool preprocess()
    {
        // check context
        FunctionalBlockExecutionContext<__type>* context =
            dynamic_cast<FunctionalBlockExecutionContext<__type>*>(this);

        return context != nullptr &&
               context->preprocess();
    }

    template< typename __type >
    bool postprocess()
    {
        FunctionalBlockExecutionContext<__type>* context =
            dynamic_cast<FunctionalBlockExecutionContext<__type>*>(this);

        return context != nullptr &&
               context->postprocess();
    }

    // IPugiXMLSerializable
    virtual pugi::xml_node pugi_serialize(pugi::xml_node& _Parent) override;
    virtual bool pugi_deserialize(pugi::xml_node& _Node) override;

    //----------------------------------------------------------------------------------
    // GEOMETRY
    //----------------------------------------------------------------------------------
    enum DrawChannels
    {
        Main,
        Interactions
    };

    void draw();

    ImVec2 LocalItemPosition(ImVec2 _Position, bool _AlignToGrid = false) const
    {
        return _Position * m_Scale + m_Origin + m_Offset;
    };

    ImVec2 LocalMousePostion(ImVec2 _Position) const
    {
        return (_Position - m_Origin - m_Offset) / m_Scale;
    };

    void DrawBackground()
    {
        if(m_DrawList == nullptr)
            return;

        // draw background color
        m_DrawList->AddRectFilled(
            m_Rect.GetTL(),
            m_Rect.GetBR(),
            IM_COL32(64, 64, 64, 255));

        // draw border
        m_DrawList->AddRect(
            m_Rect.GetTL(),
            m_Rect.GetBR(),
            IM_COL32(0, 255, 0, 255));

        auto offset = m_Offset;

        // draw vertical grid lines
        auto gridSize = m_GridSize * m_Scale;

        for (float x = fmodf(offset.x, gridSize); x < m_Size.x; x += gridSize)
        {
            m_DrawList->AddLine(
                ImVec2(m_Rect.GetTL().x + x, m_Rect.GetTL().y),
                ImVec2(m_Rect.GetTL().x + x, m_Rect.GetBR().y),
                IM_COL32(200, 200, 200, 40));
        }

        // draw horizontal grid lines
        for (float y = fmodf(offset.y, gridSize); y < m_Size.y; y += gridSize)
        {
            m_DrawList->AddLine(
                ImVec2(m_Rect.GetTL().x, m_Rect.GetTL().y + y),
                ImVec2(m_Rect.GetTR().x, m_Rect.GetTL().y + y),
                IM_COL32(200, 200, 200, 40));
        }
    }

    void CatchMouseButtons()
    {
        ImGuiIO& io = ImGui::GetIO();

        ImGui::InvisibleButton(
            "Scene2D",
            m_Size,
            ImGuiButtonFlags_MouseButtonMask_);

        // drag
        if (ImGui::IsMouseDragging(ImGuiMouseButton_Middle))
        {
            m_Offset.x += io.MouseDelta.x;
            m_Offset.y += io.MouseDelta.y;
        }

        // zoom
        if (io.MouseWheel != 0)
            m_Scale = ImClamp(m_Scale + io.MouseWheel * m_Scale / 16.f, 0.0001f, 100.f);
    }

    void DrawText()
    {
        if(m_DrawList == nullptr)
            return;

        auto mousePosition = LocalMousePostion(ImGui::GetIO().MousePos);

        m_DrawList->AddText(
            ImGui::GetIO().MousePos,
            IM_COL32(0, 255, 0, 255),
            (std::to_string(mousePosition.x) + " " + std::to_string(mousePosition.y)).c_str());
    }

    ImVec2 SceneMousePosition() const
    {
        return LocalMousePostion(ImGui::GetIO().MousePos);
    }

    // geometry
    ImVec2      m_Origin   = ImVec2();
    ImVec2      m_Size     = ImVec2();
    ImRect      m_Rect     = ImRect();
    ImVec2      m_Offset   = ImVec2(0.f, 0.f);
    ImDrawList* m_DrawList = nullptr;
    float       m_GridSize = 32.f;
    float       m_Scale    = 1.f;

    // draw list info

    //----------------------------------------------------------------------------------

protected:

    FactoryObjectHierarchy* m_Selection = nullptr;

    // service methods
    virtual void abort()
    {
        // run abort operation within child functional block execution environments
        this->apply_function_to_children_recursuve(
            [](FactoryObjectHierarchy* _Object)
            {
                FunctionalBlockExecutionEnvironment* environment =
                    dynamic_cast<FunctionalBlockExecutionEnvironment*>(_Object);

                if(environment != nullptr)
                    environment->abort();
            }
        );
    }
};

// IFunctionalBlockExecutionEnvironmentExporter
class IFunctionalBlockExecutionEnvironmentExporter
{
public:

    enum ComplexNumerForm
    {
        VECTOR,
        ORTHOGONAL,
        POLAR
    };

    IFunctionalBlockExecutionEnvironmentExporter(ComplexNumerForm _ComplexNumbersExportForm) :
        m_ComplexNumbersExportForm(_ComplexNumbersExportForm){}

    virtual ~IFunctionalBlockExecutionEnvironmentExporter(){}

    virtual bool Export(std::string _Path) = 0;

protected:

    ComplexNumerForm m_ComplexNumbersExportForm = ComplexNumerForm::VECTOR;
};

#endif // FUNCTIONALBLOCKEXECUTIONENVIRONMENT_H
