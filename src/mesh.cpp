#include "mesh.h"

#include <glad.h>
using glm::vec2;
using glm::vec3;

using std::vector;

Mesh::Mesh(const vector<uint32_t> &indices,
           const vector<vec3> &vertexes,
           const vector<vec2> &uv1,
           const vector<vec2> &uv2)
    : indicesCount(indices.size())
{
    glGenVertexArrays(1, vao);
    glGenBuffers(3, vbo);
    glGenBuffers(1, ebo);
    glBindVertexArray(vao[0]);
    glBindBuffer(GL_ARRAY_BUFFER, vbo[0]);
    glBufferData(GL_ARRAY_BUFFER, vertexes.size() * sizeof(vec3), vertexes.data(), GL_STATIC_DRAW);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(vec3), 0);
    glEnableVertexAttribArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, vbo[1]);
    glBufferData(GL_ARRAY_BUFFER, uv1.size() * sizeof(vec2), uv1.data(), GL_STATIC_DRAW);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(vec2), 0);
    glEnableVertexAttribArray(1);
    glBindBuffer(GL_ARRAY_BUFFER, vbo[2]);
    glBufferData(GL_ARRAY_BUFFER, uv2.size() * sizeof(vec2), uv2.data(), GL_STATIC_DRAW);
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(vec2), 0);
    glEnableVertexAttribArray(2);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo[0]);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(GLuint), indices.data(), GL_STATIC_DRAW);
    glBindVertexArray(0);
}

Mesh::~Mesh()
{
    glDeleteVertexArrays(1, vao);
    glDeleteBuffers(3, vbo);
    glDeleteBuffers(1, ebo);
}

void Mesh::Do(GameObject *)
{
    glBindVertexArray(vao[0]);
    glDrawElements(GL_TRIANGLES, indicesCount, GL_UNSIGNED_INT, 0);
    glBindVertexArray(0);
}
