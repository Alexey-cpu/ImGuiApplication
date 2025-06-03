#ifndef FUNCTIONALBLOCKEXECUTIONCONTEXT_H
#define FUNCTIONALBLOCKEXECUTIONCONTEXT_H

#include <string>
#include <Utils.h>

template<typename __type>
class FunctionalBlockExecutionContext
{
public:

    // constructors
    FunctionalBlockExecutionContext(std::string _Context = std::string()) :
        m_Context(_Context.empty() ? STRINGIFY(FunctionalBlockExecutionContext) : _Context){}

    // virtual destructor
    virtual ~FunctionalBlockExecutionContext(){}

    // virtual functions
    virtual bool ignore_validation(__type* = nullptr) const
    {
        return false;
    }

    virtual bool ignore_processing_fail(__type* = nullptr) const
    {
        return true;
    }

    virtual bool is_valid(__type* = nullptr)
    {
        return true;
    }

    virtual bool preprocess(__type* = nullptr)
    {
        return true;
    }

    virtual bool postprocess(__type* = nullptr)
    {
        return true;
    }

    std::string get_context() const
    {
        return m_Context;
    }

protected:

    std::string m_Context;
};

class IDataTransferExecutionContext : public FunctionalBlockExecutionContext<IDataTransferExecutionContext>
{
public:

    // constructors
    IDataTransferExecutionContext(){}

    // virtual destructor
    virtual ~IDataTransferExecutionContext(){}
};

#endif // FUNCTIONALBLOCKEXECUTIONCONTEXT_H
