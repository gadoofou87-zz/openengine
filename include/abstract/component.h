#ifndef ICOMPONENT_H
#define ICOMPONENT_H

#include "disposable.h"

class GameObject;
class Component : Disposable
{
public:
    virtual ~Component();

protected:
    explicit Component();

private:
    virtual void Do(GameObject *) = 0;

    friend class GameObject;
};

#endif // ICOMPONENT_H
