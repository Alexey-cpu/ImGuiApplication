#ifndef DYNAMICGRAPHOBJECT_H
#define DYNAMICGRAPHOBJECT_H

#include "FactoryObject.h"

// standart library includes
#include "ctime"

// Dynamic Graph object ( base class for nodes and edges )
class DynamicGraphObject : public FactoryObjectHierarchy
{
protected:
    size_t       m_Number = 0;
    mutable bool m_Marked = false;

public:

    // constructors
    DynamicGraphObject(
        const std::string&      _Name   = std::string(),
        FactoryObjectHierarchy* _Parent = nullptr,
        const std::string&      _UUID   = std::string()) :
        FactoryObjectHierarchy(_Name, _Parent, _UUID){}

    // virtual destructor
    virtual ~DynamicGraphObject(){}

    // getters
    size_t get_number() const
    {
        return m_Number;
    }

    // setters
    void set_number(const size_t& _Number)
    {
        m_Number = _Number;
    }

    // public methods
    bool& marked() const
    {
        return m_Marked;
    }
};

#endif // DYNAMICGRAPHOBJECT_H
