#ifndef TEXTURE_H
#define TEXTURE_H

#include "abstract/disposable.h"

#include <glm/glm.hpp>
using uvec2 = glm::tvec2<uint32_t>;
using Color = glm::tvec4<uint8_t>;

#include <vector>

struct Rect
{
    uvec2 position;
    uvec2 size;
};

enum class TextureFormat
{
    DXT1,
    DXT1_ONEBITALPHA,
    DXT3,
    DXT5,
    RGB24,
    RGBA32
};

class Material;
class Texture : Disposable
{
public:
    explicit Texture(uint32_t, uint32_t, TextureFormat = TextureFormat::RGBA32);
    ~Texture();

    void Apply(bool);
    void LoadRawTextureData(const uintptr_t *);
    std::vector<Rect> PackTextures(const std::vector<Texture *> &);

    uintmax_t GetArea() const;
    uint32_t GetWidth() const;
    uint32_t GetHeight() const;

    Color GetPixel(uint32_t) const;
    Color GetPixel(uint32_t, uint32_t) const;

    void SetPixel(uint32_t, const Color &);
    void SetPixel(uint32_t, uint32_t, const Color &);

private:
    struct Node
    {
        Node *child[2] =
        {
            nullptr,
            nullptr
        };
        Rect rc;
        bool used = false;

        ~Node();
        Node *Insert(const Texture *);
    };
    uint32_t name;
    int32_t internalformat;
    int32_t format;
    uint32_t width, height;
    uint32_t bitsPerPixel;
    bool compressed;
    std::vector<uint8_t> buffer;

    void Allocate(uint32_t, uint32_t);
    std::vector<Rect> PackTextures(const std::vector<Texture *> &, uint32_t, uint32_t);

    friend class Material;
};

#endif // TEXTURE_H
