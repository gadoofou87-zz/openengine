#include "gameobject.h"
#include "abstract/component.h"
#include "application.h"
#include "material.h"
#include "mesh.h"

#include <glm/gtc/matrix_transform.hpp>
using glm::mat4;
using glm::rotate;
using glm::scale;
using glm::translate;
using glm::vec3;

#include <algorithm>
#include <stdexcept>
using std::logic_error;
using std::vector;

vector<GameObject*> GameObject::instances;

GameObject::GameObject()
    : parent(nullptr)
    , model(1.f)
    , mat_position(1.f)
    , mat_rotation(1.f)
    , mat_scale(1.f)
    , materialComponent(nullptr)
    , meshComponent(nullptr)
    , dirty(false)
{
    instances.push_back(this);
}

GameObject::~GameObject()
{
    instances.erase(find(instances.begin(), instances.end(), this));
}

void GameObject::AddComponent(Component *component)
{
    if (dynamic_cast<Material*>(component))
    {
        if (materialComponent)
        {
            throw logic_error("Material is already attached to this object");
        }
        materialComponent = component;
    }
    else if (dynamic_cast<Mesh*>(component))
    {
        if (meshComponent)
        {
            throw logic_error("Mesh is already attached to the object");
        }
        meshComponent = component;
    }
    else
    {
        throw logic_error("This component is not supported");
    }
}

void GameObject::RemoveComponent(Component *component)
{
    if (component == materialComponent)
    {
        materialComponent = nullptr;
    }
    else if (component == meshComponent)
    {
        meshComponent = nullptr;
    }
    else
    {
        throw logic_error("This component is not attached to this object");
    }
}

mat4 GameObject::GetModelMatrix()
{
    return model;
}

void GameObject::SetParent(const GameObject *parent)//, bool worldPositionStays)
{
    this->parent = parent;
    dirty = true;
}

void GameObject::SetPosition(const vec3 &v)
{
    mat_position = mat4(1.f);
    mat_position = translate(mat_position, v);
    dirty = true;
}

void GameObject::SetRotation(const vec3 &v)
{
    mat_rotation = mat4(1.f);
    mat_rotation = rotate(mat_rotation, v.x, vec3(1.f, 0.f, 0.f));
    mat_rotation = rotate(mat_rotation, v.y, vec3(0.f, 1.f, 0.f));
    mat_rotation = rotate(mat_rotation, v.z, vec3(0.f, 0.f, 1.f));
    dirty = true;
}

void GameObject::SetScale(const vec3 &v)
{
    mat_scale = mat4(1.f);
    mat_scale = scale(mat_scale, v);
    dirty = true;
}

void GameObject::Update()
{
    if (parent)
    {
        mat_position *= parent->mat_position;
        mat_rotation *= parent->mat_rotation;
        mat_scale *= parent->mat_scale;
    }
    model = mat_position *
            mat_rotation *
            mat_scale;
    dirty = false;
}

void GameObject::Do()
{
    if (dirty)
    {
        Update();
    }
    if (materialComponent && meshComponent)
    {
        materialComponent->Do(this);
        meshComponent->Do(this);
    }
}
