#ifndef MATERIAL_H
#define MATERIAL_H

#include "abstract/component.h"

#include <string>
#include <unordered_map>

class Texture;
class Material : public Component
{
    static uint32_t shaderProgram;
    static int32_t mvpLocation[];

public:
    explicit Material();

    Texture *GetTexture(std::string);
    void SetTexture(std::string, Texture *);

private:
    std::unordered_map<int32_t, Texture *> textures;

    void Initialize();
    void Do(GameObject *);
};

#endif // MATERIAL_H
