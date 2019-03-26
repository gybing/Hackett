#ifndef LXCOMPONENTHELPER_H_INCLUDED
#define LXCOMPONENTHELPER_H_INCLUDED

class LXComponentHelper
{
public:
    static void broughtComponentToFront(Component* comp)
    {
        if (Component* parent = comp->getParentComponent())
        {
            parent->removeChildComponent(parent->getIndexOfChildComponent(comp));
            parent->addChildComponent(comp, 0xFFFF);
        }
    }
};

#endif  // LXPROTOCOL_H_INCLUDED
