#ifndef APPLICATION_H
#define APPLICATION_H

#include <glm/glm.hpp>

class Camera;
class Application
{
public:
    explicit Application(const char*, int = 800, int = 600, float = .15f, float = 30.f);
    ~Application();

    static const Camera* GetCamera();
    static double GetDeltaTime();
    static glm::mat4 GetProjectionMatrix();

    int exec();

private:
    static Application *instance;
    double deltaTime;
};

#endif // APPLICATION_H
