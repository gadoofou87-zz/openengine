#ifndef IDISPOSABLE_H
#define IDISPOSABLE_H

#include <vector>

class Application;
class Disposable
{
public:
    virtual ~Disposable();

protected:
    explicit Disposable();

private:
    static std::vector<Disposable *> allocated;

    friend class Application;
};

#endif // IDISPOSABLE_H
