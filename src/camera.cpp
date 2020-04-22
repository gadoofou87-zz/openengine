#include "camera.h"
#include "application.h"

#include <glm/gtc/matrix_transform.hpp>
using glm::cross;
using glm::lookAt;
using glm::mat4;
using glm::normalize;
using glm::radians;

Camera::Camera(float rotateSpeed, float moveSpeed)
    : rotateSpeed(rotateSpeed)
    , moveSpeed(moveSpeed)
    , position(0.f, 0.f, 0.f)
    , rotation(-90.f, 0.f)
    , front(0.f, 0.f, -1.f)
    , worldUp(0.f, 1.f, 0.f)
{
    Update();
}

mat4 Camera::GetViewMatrix() const
{
    return lookAt(position, position + front, up);
}

void Camera::ProcessKeyboard(Direction direction)
{
    float velocity = Application::GetDeltaTime() * moveSpeed;
    switch (direction)
    {
    case Direction::Forward:
        position += velocity * front;
        break;
    case Direction::Backward:
        position -= velocity * front;
        break;
    case Direction::Left:
        position -= velocity * right;
        break;
    case Direction::Right:
        position += velocity * right;
        break;
    }
}

void Camera::ProcessMouse(float xoffset, float yoffset)
{
    xoffset *= rotateSpeed;
    yoffset *= rotateSpeed;

    rotation.x += xoffset;
    rotation.y += yoffset;

    if (rotation.y > 89.f)
        rotation.y = 89.f;
    if (rotation.y < -89.f)
        rotation.y = -89.f;

    Update();
}

void Camera::Update()
{
    front.x = cos(radians(rotation.x)) * cos(radians(rotation.y));
    front.y = sin(radians(rotation.y));
    front.z = sin(radians(rotation.x)) * cos(radians(rotation.y));
    front = normalize(front);
    right = normalize(cross(front, worldUp));
    up = normalize(cross(right, front));
}
