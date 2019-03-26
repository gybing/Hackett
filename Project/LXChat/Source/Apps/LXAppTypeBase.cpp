#include "JuceHeader.h"
#include "LXAppTypeBase.h"

//==============================================================================
struct AlphabeticAppSorter
{
    static int compareElements(const LXAppTypeBase* first, const LXAppTypeBase* second)
    {
        return first->name.compare(second->name);
    }
};

LXAppTypeBase::LXAppTypeBase(const String& demoName) : name(demoName)
{
    AlphabeticAppSorter sorter;
    getAppTypeList().addSorted(sorter, this);
}

LXAppTypeBase::~LXAppTypeBase()
{
    getAppTypeList().removeFirstMatchingValue(this);
}

Array<LXAppTypeBase*>& LXAppTypeBase::getAppTypeList()
{
    static Array<LXAppTypeBase*> list;
    return list;
}
