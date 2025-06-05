#ifndef FUNCTIONALBLOCKEXECUTIONENVIRONMENT_H
#define FUNCTIONALBLOCKEXECUTIONENVIRONMENT_H

// custom includes
#include "FunctionalBlockPortsConnectionLine.h"
#include "FunctionalBlockExecutionContext.h"
#include "DynamicGraph.h"
//#include "DebugLog.h"
//#include "Singleton.h"

#include <imgui.h>

class FunctionalBlockExecutionEnvironmentPortsConnector
{
public:

    FunctionalBlockExecutionEnvironmentPortsConnector(){}
    virtual ~FunctionalBlockExecutionEnvironmentPortsConnector(){}

protected:

    FunctionalBlockPort* m_Source = nullptr;
    FunctionalBlockPort* m_Target = nullptr;
};

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

    ImVec2 get_mouse_scene_position() const;
    ImVec2 get_item_scene_position(ImVec2 _Position) const;
    float get_grid_size() const;

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

    enum DrawChannels
    {
        Main,
        Blocks,
        Lines,
        Ports,
        Last,
    };

    virtual pugi::xml_node pugi_serialize(pugi::xml_node& _Parent) override;
    virtual bool pugi_deserialize(pugi::xml_node& _Node) override;
    virtual void draw_start() override;
    virtual void draw_process() override;
    virtual void draw_finish() override;

protected:

    FactoryObjectHierarchy* m_Selection    = nullptr;
    FunctionalBlock*        m_MouseGrabber = nullptr;
    float                   m_GridSize     = 32.f;

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
