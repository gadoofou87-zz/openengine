#ifndef GAMEOBJECT_H
#define GAMEOBJECT_H

#include "abstract/disposable.h"

#include <glm/glm.hpp>

#include <vector>

class Application;
class Component;
class GameObject : Disposable
{
    static std::vector<GameObject*> instances;

public:
    explicit GameObject();
    ~GameObject();

    void AddComponent(Component*);
    void RemoveComponent(Component*);

    glm::mat4 GetModelMatrix();

    void SetParent(const GameObject*);
    void SetPosition(const glm::vec3&);
    void SetRotation(const glm::vec3&);
    void SetScale(const glm::vec3&);

private:
    const GameObject *parent;
    glm::mat4 model;
    glm::mat4 mat_position;
    glm::mat4 mat_rotation;
    glm::mat4 mat_scale;
    Component *materialComponent;
    Component *meshComponent;
    bool dirty;

    void Update();
    void Do();

    friend class Application;
};

#endif // GAMEOBJECT_H
