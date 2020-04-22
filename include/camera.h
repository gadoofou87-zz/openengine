#ifndef CAMERA_H
#define CAMERA_H

#include <glm/glm.hpp>

enum class Direction
{
    Forward,
    Backward,
    Left,
    Right
};

class Camera
{
public:
    explicit Camera(float, float);
    glm::mat4 GetViewMatrix() const;
    void ProcessKeyboard(Direction);
    void ProcessMouse(float, float);

private:
    float rotateSpeed;
    float moveSpeed;
    glm::vec3 position;
    glm::vec2 rotation;  // yaw pitch
    glm::vec3 front;
    glm::vec3 up;
    glm::vec3 right;
    glm::vec3 worldUp;

    void Update();
};

#endif // CAMERA_H
