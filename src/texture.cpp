#include "texture.h"

#include <glad.h>

#include <cstring>
#include <stdexcept>
using std::logic_error;
using std::vector;

Texture::Texture(uint32_t width, uint32_t height, TextureFormat textureFormat)
{
    if (width < 0 || height < 0)
    {
        throw logic_error("Width and/or height values cannot be negative");
    }

    switch (textureFormat)
    {
    case TextureFormat::DXT1:
    {
        internalformat = GL_COMPRESSED_RGB_S3TC_DXT1_EXT;
        bitsPerPixel = 4;
        compressed = true;
    }
    break;

    case TextureFormat::DXT1_ONEBITALPHA:
    {
        internalformat = GL_COMPRESSED_RGBA_S3TC_DXT1_EXT;
        bitsPerPixel = 4;
        compressed = true;
    }
    break;

    case TextureFormat::DXT3:
    {
        internalformat = GL_COMPRESSED_RGBA_S3TC_DXT3_EXT;
        bitsPerPixel = 8;
        compressed = true;
    }
    break;

    case TextureFormat::DXT5:
    {
        internalformat = GL_COMPRESSED_RGBA_S3TC_DXT5_EXT;
        bitsPerPixel = 8;
        compressed = true;
    }
    break;

    case TextureFormat::RGB24:
    {
        internalformat = GL_RGB8;
        format = GL_RGB;
        bitsPerPixel = 24;
        compressed = false;
    }
    break;

    case TextureFormat::RGBA32:
    {
        internalformat = GL_RGBA8;
        format = GL_RGBA;
        bitsPerPixel = 32;
        compressed = false;
    }
    break;
    }

    Allocate(width, height);
    glGenTextures(1, &name);
}

Texture::~Texture()
{
    glDeleteTextures(1, &name);
}

void Texture::Apply(bool updateMipmaps)
{
    glBindTexture(GL_TEXTURE_2D, name);

    if (compressed)
    {
        glCompressedTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, buffer.size(), buffer.data());
    }
    else
    {
        glTexImage2D(GL_TEXTURE_2D, 0, internalformat, width, height, 0, format, GL_UNSIGNED_BYTE, buffer.data());
    }

    if (updateMipmaps)
    {
        glGenerateMipmap(GL_TEXTURE_2D);
    }
    else
    {
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    }

    glBindTexture(GL_TEXTURE_2D, 0);
}

void Texture::LoadRawTextureData(const uintptr_t *data)
{
    memcpy(buffer.data(), data, buffer.size());
}

vector<Rect> Texture::PackTextures(const vector<Texture *> &textures)
{
    return PackTextures(textures, width, height);
}

uintmax_t Texture::GetArea() const
{
    return width * height;
}

uint32_t Texture::GetWidth() const
{
    return width;
}

uint32_t Texture::GetHeight() const
{
    return height;
}

Color Texture::GetPixel(uint32_t pixel) const
{
    if (compressed)
    {
        throw logic_error("Not supported for compressed textures");
    }

    if (pixel >= GetArea())
    {
        throw logic_error("Out of the image");
    }

    Color color(255);
    auto offset = pixel * bitsPerPixel / 8;

    if (bitsPerPixel >= 8)
    {
        color.r = buffer[offset];
    }

    if (bitsPerPixel >= 16)
    {
        color.g = buffer[offset + 1];
    }

    if (bitsPerPixel >= 24)
    {
        color.b = buffer[offset + 2];
    }

    if (bitsPerPixel >= 32)
    {
        color.a = buffer[offset + 3];
    }

    return color;
}

Color Texture::GetPixel(uint32_t x, uint32_t y) const
{
    return GetPixel(y * width + x);
}

void Texture::SetPixel(uint32_t pixel, const Color &color)
{
    if (compressed)
    {
        throw logic_error("Not supported for compressed textures");
    }

    if (pixel >= GetArea())
    {
        throw logic_error("Out of the image");
    }

    auto offset = pixel * bitsPerPixel / 8;

    if (bitsPerPixel >= 8)
    {
        buffer[offset] = color.r;
    }

    if (bitsPerPixel >= 16)
    {
        buffer[offset + 1] = color.g;
    }

    if (bitsPerPixel >= 24)
    {
        buffer[offset + 2] = color.b;
    }

    if (bitsPerPixel >= 32)
    {
        buffer[offset + 3] = color.a;
    }
}

void Texture::SetPixel(uint32_t x, uint32_t y, const Color &color)
{
    return SetPixel(y * width + x, color);
}

void Texture::Allocate(uint32_t newWidth, uint32_t newHeight)
{
    width = newWidth;
    height = newHeight;
    buffer.resize(width * height * bitsPerPixel / 8);
}

vector<Rect> Texture::PackTextures(const vector<Texture *> &textures, uint32_t startW, uint32_t startH)
{
    auto root = new Node;
    root->rc.position.x = 0;
    root->rc.position.y = 0;
    root->rc.size.x = startW;
    root->rc.size.y = startH;

    vector<Rect> rc;

    for (const auto texture : textures)
    {
        auto pnode = root->Insert(texture);

        if (!pnode)
        {
            delete root;
            return PackTextures(textures,
                                startW * 2,
                                startH * 2);

        }

        pnode->used = true;
        rc.push_back(pnode->rc);
    }

    Allocate(root->rc.size.x, root->rc.size.y);

    for (size_t i = 0; i < rc.size(); i++)
    {
        for (uint32_t x = 0; x < rc[i].size.x; x++)
        {
            for (uint32_t y = 0; y < rc[i].size.y; y++)
            {
                SetPixel(rc[i].position.x + x,
                         rc[i].position.y + y,
                         textures[i]->GetPixel(x, y));
            }
        }
    }

    delete root;
    return rc;
}

Texture::Node::~Node()
{
    if (!child[0])
    {
        return;
    }

    delete child[0];
    delete child[1];
}

Texture::Node *Texture::Node::Insert(const Texture *texture)
{
    if (child[0])
    {
        auto newNode = child[0]->Insert(texture);
        return newNode
               ? newNode
               : child[1]->Insert(texture);
    }

    if (used)
    {
        return nullptr;
    }

    int32_t dw = rc.size.x - texture->width;
    int32_t dh = rc.size.y - texture->height;

    if (dw < 0 || dh < 0)
    {
        return nullptr;
    }

    if (dw == 0 && dh == 0)
    {
        return this;
    }

    child[0] = new Node;
    child[1] = new Node;

    if (dw > dh)
    {
        child[0]->rc.position = rc.position;
        child[0]->rc.size.x = texture->width;
        child[0]->rc.size.y = rc.size.y;

        child[1]->rc.position.x = rc.position.x + texture->width;
        child[1]->rc.position.y = rc.position.y;
        child[1]->rc.size.x = rc.size.x - texture->width;
        child[1]->rc.size.y = rc.size.y;
    }
    else
    {
        child[0]->rc.position = rc.position;
        child[0]->rc.size.x = rc.size.x;
        child[0]->rc.size.y = texture->height;

        child[1]->rc.position.x = rc.position.x;
        child[1]->rc.position.y = rc.position.y + texture->height;
        child[1]->rc.size.x = rc.size.x;
        child[1]->rc.size.y = rc.size.y - texture->height;
    }

    return child[0]->Insert(texture);
}
