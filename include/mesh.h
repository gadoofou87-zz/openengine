#ifndef MESH_H
#define MESH_H

#include "abstract/component.h"

#include <glm/glm.hpp>

#include <vector>

class Mesh : public Component
{
public:
    explicit Mesh(const std::vector<uint32_t>&,
                  const std::vector<glm::vec3>&,
                  const std::vector<glm::vec2>&,
                  const std::vector<glm::vec2>&);
    ~Mesh();

private:
    size_t indicesCount;
    uint32_t vao[1], vbo[3], ebo[1];

    void Do(GameObject*);
};

#endif // MESH_H
