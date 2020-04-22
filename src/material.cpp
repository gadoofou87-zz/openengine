#include "material.h"
#include "application.h"
#include "camera.h"
#include "gameobject.h"
#include "texture.h"

#include <glad.h>
#include <glm/gtc/type_ptr.hpp>
using glm::value_ptr;

#include <stdexcept>
using std::logic_error;
using std::runtime_error;
using std::string;
using std::vector;

const char *vShaderCode =
        #include "shader.vs"
        ;
const char *fShaderCode =
        #include "shader.frag"
        ;
uint32_t Material::shaderProgram;
int32_t Material::mvpLocation[3];

Material::Material()
{
    if (glIsProgram(shaderProgram))
    {
        return;
    }
    int32_t success, logLength;
    vector<char> log;
    auto vShader = glCreateShader(GL_VERTEX_SHADER);
    auto fShader = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(vShader, 1, &vShaderCode, nullptr);
    glShaderSource(fShader, 1, &fShaderCode, nullptr);
    glCompileShader(vShader);
    glGetShaderiv(vShader, GL_COMPILE_STATUS, &success);
    if (!success)
    {
        glGetShaderiv(vShader, GL_INFO_LOG_LENGTH, &logLength);
        log.resize(logLength);
        glGetShaderInfoLog(vShader, logLength, nullptr, log.data());
        throw runtime_error(string("Failed to compile vertex shader:\n") + log.data());
    }
    glCompileShader(fShader);
    glGetShaderiv(fShader, GL_COMPILE_STATUS, &success);
    if (!success)
    {
        glGetShaderiv(fShader, GL_INFO_LOG_LENGTH, &logLength);
        log.resize(logLength);
        glGetShaderInfoLog(fShader, logLength, nullptr, log.data());
        throw runtime_error(string("Failed to compile fragment shader:\n") + log.data());
    }
    shaderProgram = glCreateProgram();
    glAttachShader(shaderProgram, vShader);
    glAttachShader(shaderProgram, fShader);
    glLinkProgram(shaderProgram);
    glGetProgramiv(shaderProgram, GL_LINK_STATUS, &success);
    if (!success)
    {
        glGetProgramiv(shaderProgram, GL_INFO_LOG_LENGTH, &logLength);
        log.resize(logLength);
        glGetProgramInfoLog(shaderProgram, logLength, nullptr, log.data());
        throw runtime_error(string("Failed to link shader program:\n") + log.data());
    }
    glDeleteShader(vShader);
    glDeleteShader(fShader);
    mvpLocation[0] = glGetUniformLocation(shaderProgram, "model");
    mvpLocation[1] = glGetUniformLocation(shaderProgram, "view");
    mvpLocation[2] = glGetUniformLocation(shaderProgram, "projection");
}

Texture* Material::GetTexture(string name)
{
    auto textureLocation = glGetUniformLocation(shaderProgram, name.c_str());
    if (textureLocation != -1)
    {
        auto it = textures.find(textureLocation);
        if (it != textures.end())
        {
            return it->second;
        }
    }
    throw logic_error("Texture not found");
}

void Material::SetTexture(string name, Texture *texture)
{
    auto textureLocation = glGetUniformLocation(shaderProgram, name.c_str());
    if (textureLocation == -1)
    {
        throw logic_error("Unable to attach texture to shader program");
    }
    auto it = textures.find(textureLocation);
    if (it != textures.end())
    {
        textures.erase(it);
    }
    textures.insert({textureLocation, texture});
}

void Material::Do(GameObject *caller)
{
    glUseProgram(shaderProgram);
    int i = 0;
    for (auto it = textures.begin(); it != textures.end(); it++, i++)
    {
        glActiveTexture(GL_TEXTURE0 + i);
        glBindTexture(GL_TEXTURE_2D, it->second->name);
        glUniform1i(it->first, i);
    }
    glUniformMatrix4fv(mvpLocation[0], 1, GL_FALSE, value_ptr(caller->GetModelMatrix()));
    glUniformMatrix4fv(mvpLocation[1], 1, GL_FALSE, value_ptr(Application::GetCamera()->GetViewMatrix()));
    glUniformMatrix4fv(mvpLocation[2], 1, GL_FALSE, value_ptr(Application::GetProjectionMatrix()));
}
