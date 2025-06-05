#ifndef FUNCTIONALBLOCKPORTSCONNECTIONLINE_H
#define FUNCTIONALBLOCKPORTSCONNECTIONLINE_H

#include "DynamicGraph.h"
#include "DynamicGraphEdge.h"
#include "FunctionalBlock.h"

// FunctionalBlockPortsConnectionLine
class FunctionalBlockPortsConnectionLine : public DynamicGraphEdge, public FactoryObjectRenderer
{
public:

    // constructor
    FunctionalBlockPortsConnectionLine() : DynamicGraphEdge(){}

    // virtual destructor
    virtual ~FunctionalBlockPortsConnectionLine(){}

    //
    bool check(DynamicGraphNode* _Source, DynamicGraphNode* _Target )
    {
        // retrive source and target ports
        FunctionalBlockPort* sourcePort = dynamic_cast<FunctionalBlockPort*>(_Source);
        FunctionalBlockPort* targetPort = dynamic_cast<FunctionalBlockPort*>(_Target);

        // retrive parent functional blocks of source and target ports
        FunctionalBlock* sourceBlock = sourcePort != nullptr ? sourcePort->get_parent_recursive<FunctionalBlock>() : nullptr;
        FunctionalBlock* targetBlock = targetPort != nullptr ? targetPort->get_parent_recursive<FunctionalBlock>() : nullptr;

        // retrive parent execution environment of the source and target functional blocks
        DynamicGraph* sourceBlockParent = sourceBlock != nullptr ? sourceBlock->get_parent_recursive<DynamicGraph>() : nullptr;
        DynamicGraph* targetBlockParent = targetBlock != nullptr ? targetBlock->get_parent_recursive<DynamicGraph>() : nullptr;

        // check ports compatability, restrict inputs recieve information from several outputs at the same time
        if(sourcePort  == nullptr ||
            targetPort  == nullptr ||
            sourceBlock == nullptr ||
            targetBlock == nullptr ||
            sourceBlockParent == nullptr ||
            targetBlockParent == nullptr ||
            //sourceBlockParent != targetBlockParent ||
            !sourcePort->is_compatible_to( targetPort ) ||
            ( sourcePort->get_port_type() == FunctionalBlockPort::Type::INPUT && !sourcePort->retrieve_adjacent_nodes().empty() ) ||
            ( targetPort->get_port_type() == FunctionalBlockPort::Type::INPUT && !targetPort->retrieve_adjacent_nodes().empty() ) )
        {
            return false;
        }

        return true;
    }

    virtual bool attach(DynamicGraphNode* _Source, DynamicGraphNode* _Target) override
    {
        if(check(_Source, _Target))
            return DynamicGraphEdge::attach(_Source, _Target);

        return false;
    }

    virtual void draw_process(const glm::mat4& _Transform) override;

protected:

    bool m_Selected = false;
    bool m_Focused  = false;
};

// FunctionalBlockPortsConnectionLineCreator
class FunctionalBlockPortsConnectionLineCreator : public IDynamicGraphEdgeCreator
{
public:

    // constructors
    FunctionalBlockPortsConnectionLineCreator(){}

    // virtual destructor
    virtual ~FunctionalBlockPortsConnectionLineCreator(){}

    // virtual functions override
    virtual DynamicGraphEdge* Create() override
    {
        return new FunctionalBlockPortsConnectionLine();
    }
};


#endif // FUNCTIONALBLOCKPORTSCONNECTIONLINE_H
