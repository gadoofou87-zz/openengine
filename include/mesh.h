#ifndef MESH_H
#define MESH_H

#include "abstract/component.h"

#include <glm/glm.hpp>

#include <vector>

struct Vertex
{
    glm::vec3 position;
    glm::vec2 uv1;
    glm::vec2 uv2;
};

class Mesh : public Component
{
public:
    explicit Mesh(const std::vector<uint32_t> &,
                  const std::vector<Vertex> &);
    ~Mesh();

private:
    size_t indicesCount;
    uint32_t vao, vbo, ebo;

    void Do(GameObject *);
};

#endif // MESH_H
