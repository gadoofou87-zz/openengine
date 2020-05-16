#include "bsp.h"
#include "gameobject.h"
#include "material.h"
#include "mesh.h"
#include "texture.h"

using glm::clamp;
using glm::distance;
using glm::dot;
using glm::pow;
using glm::vec2;
using glm::vec3;

#include <boost/algorithm/string/split.hpp>
#include <boost/algorithm/string/trim.hpp>
using boost::is_any_of;
using boost::split;
using boost::trim_if;

#include <limits>
#include <regex>
#include <stdexcept>
using std::ifstream;
using std::numeric_limits;
using std::regex;
using std::runtime_error;
using std::sregex_iterator;
using std::stoi;
using std::stof;
using std::string;
using std::vector;
using std::unordered_map;

// Inches to Meters
constexpr float Worldscale = 0.0254f;

void BSP::LoadBSPFile(string filename, bool bHDR)
{
    pFile.open(filename, ifstream::in | ifstream::binary);

    if (!pFile.is_open())
    {
        throw runtime_error("Could not open file");
    }

    pFile.read(reinterpret_cast<char *>(&g_pBSPHeader), sizeof(g_pBSPHeader));

    if (g_pBSPHeader.ident != IDBSPHEADER)
    {
        throw runtime_error("Bad signature");
    }

    CopyLump(LUMP_MODELS, dmodels);
    CopyLump(bHDR ? LUMP_LIGHTING_HDR
             : LUMP_LIGHTING,
             dlightdata);
    CopyLump(LUMP_ENTITIES, dentdata);
    CopyLump(LUMP_VERTEXES, dvertexes);
    CopyLump(LUMP_TEXINFO, texinfo);
    CopyLump(LUMP_TEXDATA, dtexdata);
    CopyLump(LUMP_DISPINFO, g_dispinfo);
    CopyLump(LUMP_DISP_VERTS, g_DispVerts);
    CopyLump(bHDR ? LUMP_FACES_HDR
             : LUMP_FACES,
             dfaces);
    CopyLump(LUMP_EDGES, dedges);
    CopyLump(LUMP_SURFEDGES, dsurfedges);
    CopyLump(LUMP_TEXDATA_STRING_DATA, g_TexDataStringData);
    CopyLump(LUMP_TEXDATA_STRING_TABLE, g_TexDataStringTable);

    pFile.close();

    root = new GameObject;
    root->SetScale(vec3(Worldscale));

    ParseEntities();

    dmodels.clear();
    dlightdata.clear();
    dentdata.clear();
    dvertexes.clear();
    texinfo.clear();
    dtexdata.clear();
    g_dispinfo.clear();
    g_DispVerts.clear();
    dfaces.clear();
    dedges.clear();
    g_TexDataStringData.clear();
    g_TexDataStringTable.clear();
}

void BSP::ParseEntities()
{
    string s(dentdata.begin(), dentdata.end());

    regex r1(R"(\{[^}]*\})");
    regex r2(R"(\"[^\"]*\")");

    for (auto it = sregex_iterator(s.begin(), s.end(), r1); it != sregex_iterator(); it++)
    {
        unordered_map<string, string> data;

        auto ent = (*it).str();

        for (auto it = sregex_iterator(ent.begin(), ent.end(), r2); it != sregex_iterator(); it++)
        {
            auto key = (*it++).str();
            auto value = (*it).str();
            trim_if(key, is_any_of("\""));
            trim_if(value, is_any_of("\""));
            data[key] = value;
        }

        GameObject *object;

        if (data["classname"] == "worldspawn")
        {
            object = BuildModel(0);
        }
        else
        {
            if (data.find("model") != data.end())
            {
                if (data["model"][0] == '*')
                {
                    object = BuildModel(stoi(data["model"].substr(1)));
                }
                else
                {
                    // Studiomodel
                    object = new GameObject;
                }
            }
            else
            {
                object = new GameObject;
            }
        }

        if (data.find("origin") != data.end())
        {
            vector<string> origin;
            split(origin, data["origin"], is_any_of(" "));
            object->SetPosition(FlipVector(vec3(stof(origin[0]), stof(origin[1]), stof(origin[2]))) * Worldscale);
        }

        object->SetParent(root);
    }
}

Surface BSP::BuildFace(int index)
{
    if (dfaces[index].dispinfo != -1)
    {
        return BuildDisplacement(index);
    }

    Surface surface;
    surface.index = index;

    for (int i = 1; i < dfaces[index].numedges - 1; i++)
    {
        surface.indices.push_back(0);
        surface.indices.push_back(i);
        surface.indices.push_back(i + 1);
    }

    for (int i = dfaces[index].firstedge; i < dfaces[index].firstedge + dfaces[index].numedges; i++)
    {
        auto vertex = FlipVector(dsurfedges[i] > 0
                                 ? dvertexes[dedges[abs(dsurfedges[i])].v[0]]
                                 : dvertexes[dedges[abs(dsurfedges[i])].v[1]]);

        auto tc1 = dot(vertex, FlipVector(texinfo[dfaces[index].texinfo].textureU))
                   + texinfo[dfaces[index].texinfo].textureUOffset;
        auto tc2 = dot(vertex, FlipVector(texinfo[dfaces[index].texinfo].textureV))
                   + texinfo[dfaces[index].texinfo].textureVOffset;
        auto tc3 = dot(vertex, FlipVector(texinfo[dfaces[index].texinfo].lightmapU))
                   + texinfo[dfaces[index].texinfo].lightmapUOffset
                   + .5f
                   - dfaces[index].m_LightmapTextureMinsInLuxels[0];
        auto tc4 = dot(vertex, FlipVector(texinfo[dfaces[index].texinfo].lightmapV))
                   + texinfo[dfaces[index].texinfo].lightmapVOffset
                   + .5f
                   - dfaces[index].m_LightmapTextureMinsInLuxels[1];

        tc1 /= dtexdata[texinfo[dfaces[index].texinfo].texdata].width;
        tc2 /= dtexdata[texinfo[dfaces[index].texinfo].texdata].height;
        tc3 /= dfaces[index].m_LightmapTextureSizeInLuxels[0] + 1;
        tc4 /= dfaces[index].m_LightmapTextureSizeInLuxels[1] + 1;

        surface.vertexes.push_back(vertex);
        surface.uv1.push_back(vec2(tc1, tc2));
        surface.uv2.push_back(vec2(tc3, tc4));
    }

    return surface;
}

Surface BSP::BuildDisplacement(int index)
{
    Surface surface;
    surface.index = index;

    vector<vec3> vertexes;

    for (int i = dfaces[index].firstedge; i < dfaces[index].firstedge + dfaces[index].numedges; i++)
    {
        vertexes.push_back(dsurfedges[i] > 0
                           ? dvertexes[dedges[abs(dsurfedges[i])].v[0]]
                           : dvertexes[dedges[abs(dsurfedges[i])].v[1]]);
    }

    auto minDist = numeric_limits<float>::max();
    int minIndex = 0;

    for (int i = 0; i < 4; i++)
    {
        auto dist = distance(vertexes[i], g_dispinfo[dfaces[index].dispinfo].startPosition);

        if (dist < minDist)
        {
            minDist = dist;
            minIndex = i;
        }
    }

    for (int i = 0; i < minIndex; i++)
    {
        auto temp = vertexes[0];
        vertexes[0] = vertexes[1];
        vertexes[1] = vertexes[2];
        vertexes[2] = vertexes[3];
        vertexes[3] = temp;
    }

    auto leftEdge = vertexes[1] - vertexes[0];
    auto rightEdge = vertexes[2] - vertexes[3];

    auto numEdgeVertices = (1 << g_dispinfo[dfaces[index].dispinfo].power) + 1;

    auto subdivideScale = 1.f / (numEdgeVertices - 1);

    auto lightdelta = 1.f / (numEdgeVertices - 1);

    auto leftEdgeStep = leftEdge * subdivideScale;
    auto rightEdgeStep = rightEdge * subdivideScale;

    for (int i = 0; i < numEdgeVertices; i++)
    {
        auto leftEnd = leftEdgeStep * (float) i;
        leftEnd += vertexes[0];
        auto rightEnd = rightEdgeStep * (float) i;
        rightEnd += vertexes[3];

        auto leftRightSeg = rightEnd - leftEnd;
        auto leftRightStep = leftRightSeg * subdivideScale;

        for (int j = 0; j < numEdgeVertices; j++)
        {
            auto dispVertIndex = g_dispinfo[dfaces[index].dispinfo].m_iDispVertStart;
            dispVertIndex += i * numEdgeVertices + j;
            auto dispVertInfo = g_DispVerts[dispVertIndex];

            auto flatVertex = leftEnd + (leftRightStep * (float) j);

            auto dispVertex = dispVertInfo.m_vVector * dispVertInfo.m_flDist;
            dispVertex += flatVertex;

            auto tc1 = dot(flatVertex, texinfo[dfaces[index].texinfo].textureU)
                       + texinfo[dfaces[index].texinfo].textureUOffset;
            auto tc2 = dot(flatVertex, texinfo[dfaces[index].texinfo].textureV)
                       + texinfo[dfaces[index].texinfo].textureVOffset;
            auto tc3 = lightdelta * j * dfaces[index].m_LightmapTextureSizeInLuxels[0]
                       + .5f;
            auto tc4 = lightdelta * i * dfaces[index].m_LightmapTextureSizeInLuxels[1]
                       + .5f;

            tc1 /= dtexdata[texinfo[dfaces[index].texinfo].texdata].width;
            tc2 /= dtexdata[texinfo[dfaces[index].texinfo].texdata].height;
            tc3 /= dfaces[index].m_LightmapTextureSizeInLuxels[0] + 1;
            tc4 /= dfaces[index].m_LightmapTextureSizeInLuxels[1] + 1;

            surface.vertexes.push_back(FlipVector(dispVertex));
            surface.uv1.push_back(vec2(tc1, tc2));
            surface.uv2.push_back(vec2(tc3, tc4));
        }
    }

    for (int i = 0; i < numEdgeVertices - 1; i++)
    {
        for (int j = 0; j < numEdgeVertices - 1; j++)
        {
            auto index = i * numEdgeVertices + j;

            if (index % 2)
            {
                surface.indices.push_back(index + numEdgeVertices);
                surface.indices.push_back(index + numEdgeVertices + 1);
                surface.indices.push_back(index + 1);
                surface.indices.push_back(index + numEdgeVertices);
                surface.indices.push_back(index + 1);
                surface.indices.push_back(index);
            }
            else
            {
                surface.indices.push_back(index + numEdgeVertices + 1);
                surface.indices.push_back(index + 1);
                surface.indices.push_back(index);
                surface.indices.push_back(index + numEdgeVertices);
                surface.indices.push_back(index + numEdgeVertices + 1);
                surface.indices.push_back(index);
            }
        }
    }

    return surface;
}

GameObject *BSP::BuildModel(int index)
{
    auto model = new GameObject;

    unordered_map<int, vector<int>> dict;

    for (int i = dmodels[index].firstface; i < dmodels[index].firstface + dmodels[index].numfaces; i++)
    {
        dict[dtexdata[texinfo[dfaces[i].texinfo].texdata].nameStringTableID].push_back(i);
    }

    for (size_t i = 0; i < g_TexDataStringTable.size(); i++)
    {
        if (dict.find(i) == dict.end())
        {
            continue;
        }

        vector<uint32_t> indices;
        vector<vec3> vertexes;
        vector<vec2> uv1;
        vector<vec2> uv2;
        vector<Surface> surfaces;

        for (size_t j = 0; j < dict[i].size(); j++)
        {
            if (texinfo[dfaces[dict[i][j]].texinfo].flags & (SURF_SKY | SURF_NODRAW | SURF_HINT | SURF_SKIP))
            {
                continue;
            }

            surfaces.push_back(BuildFace(dict[i][j]));
        }

        auto submesh = new GameObject;
        submesh->SetParent(model);

        auto lightmap = PackLightmaps(surfaces);

        for (auto surface : surfaces)
        {
            auto pointOffset = vertexes.size();

            for (size_t k = 0; k < surface.indices.size(); k++)
            {
                indices.push_back(surface.indices[k] + pointOffset);
            }

            vertexes.insert(vertexes.end(), surface.vertexes.begin(), surface.vertexes.end());
            uv1.insert(uv1.end(), surface.uv1.begin(), surface.uv1.end());
            uv2.insert(uv2.end(), surface.uv2.begin(), surface.uv2.end());
        }

        auto material = new Material;
        auto mesh = new Mesh(indices, vertexes, uv1, uv2);

        material->SetTexture("_LightmapTex", lightmap);

        submesh->AddComponent(material);
        submesh->AddComponent(mesh);
    }

    return model;
}

Texture *BSP::PackLightmaps(vector<Surface> &surfaces)
{
    vector<Texture *> lightmaps;

    for (auto surface : surfaces)
    {
        if (dfaces[surface.index].lightofs == -1)
        {
            continue;
        }

        auto lightmap = new Texture(dfaces[surface.index].m_LightmapTextureSizeInLuxels[0] + 1,
                                    dfaces[surface.index].m_LightmapTextureSizeInLuxels[1] + 1);
        auto color = (ColorRGBExp32 *)(dlightdata.data() + dfaces[surface.index].lightofs);

        for (uintmax_t i = 0; i < lightmap->GetArea(); i++)
        {
            lightmap->SetPixel(i, Color(clamp<int>(color[i].r * pow(2, color[i].exponent), 0, 255),
                                        clamp<int>(color[i].g * pow(2, color[i].exponent), 0, 255),
                                        clamp<int>(color[i].b * pow(2, color[i].exponent), 0, 255),
                                        255));
        }

        lightmaps.push_back(lightmap);
    }

    auto atlas = new Texture(1, 1);
    auto rc = atlas->PackTextures(lightmaps);

    for (auto lightmap : lightmaps)
    {
        delete lightmap;
    }

    for (size_t i = 0; i < surfaces.size(); i++)
    {
        if (dfaces[surfaces[i].index].lightofs == -1)
        {
            continue;
        }

        for (size_t j = 0; j < surfaces[i].uv2.size(); j++)
        {
            auto x = surfaces[i].uv2[j].x * rc[i].size.x + rc[i].position.x;
            auto y = surfaces[i].uv2[j].y * rc[i].size.y + rc[i].position.y;

            x /= atlas->GetWidth();
            y /= atlas->GetHeight();

            surfaces[i].uv2[j] = vec2(x, y);
        }
    }

    atlas->Apply(false);

    return atlas;
}

template<typename T>
void BSP::CopyLump(int lump, vector<T> &dest)
{
    dest.resize(g_pBSPHeader.lumps[lump].filelen / sizeof(T));
    pFile.seekg(g_pBSPHeader.lumps[lump].fileofs, pFile.beg);
    pFile.read(reinterpret_cast<char *>(dest.data()), g_pBSPHeader.lumps[lump].filelen);
}

vec3 BSP::FlipVector(const vec3 &v)
{
    return vec3(v.x, v.z, -v.y);
}
