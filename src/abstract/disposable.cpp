#include "abstract/disposable.h"

#include <algorithm>
using std::vector;

vector<Disposable *> Disposable::allocated;

Disposable::Disposable()
{
    allocated.push_back(this);
}

Disposable::~Disposable()
{
    allocated.erase(find(allocated.begin(), allocated.end(), this));
}
