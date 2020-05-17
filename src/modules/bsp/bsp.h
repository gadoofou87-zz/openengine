#ifndef BSP_H
#define BSP_H

#include <glm/glm.hpp>

#include <fstream>
#include <string>
#include <vector>

// little-endian "VBSP"
#define IDBSPHEADER (('P'<<24)+('S'<<16)+('B'<<8)+'V')

#define MAX_MAP_DISP_POWER      4

#define MAX_DISP_CORNER_NEIGHBORS   4

#define NUM_DISP_POWER_VERTS(power) ( ((1 << (power)) + 1) * ((1 << (power)) + 1) )

#define MAX_DISPVERTS                   NUM_DISP_POWER_VERTS( MAX_MAP_DISP_POWER )

#define PAD_NUMBER(number, boundary) \
    ( ((number) + ((boundary)-1)) / (boundary) ) * (boundary)

enum
{
    LUMP_ENTITIES                   = 0,
    LUMP_PLANES                     = 1,
    LUMP_TEXDATA                    = 2,
    LUMP_VERTEXES                   = 3,
    LUMP_VISIBILITY                 = 4,
    LUMP_NODES                      = 5,
    LUMP_TEXINFO                    = 6,
    LUMP_FACES                      = 7,
    LUMP_LIGHTING                   = 8,
    LUMP_OCCLUSION                  = 9,
    LUMP_LEAFS                      = 10,
    LUMP_FACEIDS                    = 11,
    LUMP_EDGES                      = 12,
    LUMP_SURFEDGES                  = 13,
    LUMP_MODELS                     = 14,
    LUMP_WORLDLIGHTS                = 15,
    LUMP_LEAFFACES                  = 16,
    LUMP_LEAFBRUSHES                = 17,
    LUMP_BRUSHES                    = 18,
    LUMP_BRUSHSIDES                 = 19,
    LUMP_AREAS                      = 20,
    LUMP_AREAPORTALS                = 21,
    LUMP_UNUSED0                    = 22,
    LUMP_UNUSED1                    = 23,
    LUMP_UNUSED2                    = 24,
    LUMP_UNUSED3                    = 25,
    LUMP_DISPINFO                   = 26,
    LUMP_ORIGINALFACES              = 27,
    LUMP_PHYSDISP                   = 28,
    LUMP_PHYSCOLLIDE                = 29,
    LUMP_VERTNORMALS                = 30,
    LUMP_VERTNORMALINDICES          = 31,
    LUMP_DISP_LIGHTMAP_ALPHAS       = 32,
    LUMP_DISP_VERTS                 = 33,
    LUMP_DISP_LIGHTMAP_SAMPLE_POSITIONS = 34,
    LUMP_GAME_LUMP                  = 35,
    LUMP_LEAFWATERDATA              = 36,
    LUMP_PRIMITIVES                 = 37,
    LUMP_PRIMVERTS                  = 38,
    LUMP_PRIMINDICES                = 39,
    LUMP_PAKFILE                    = 40,
    LUMP_CLIPPORTALVERTS            = 41,
    LUMP_CUBEMAPS                   = 42,
    LUMP_TEXDATA_STRING_DATA        = 43,
    LUMP_TEXDATA_STRING_TABLE       = 44,
    LUMP_OVERLAYS                   = 45,
    LUMP_LEAFMINDISTTOWATER         = 46,
    LUMP_FACE_MACRO_TEXTURE_INFO    = 47,
    LUMP_DISP_TRIS                  = 48,
    LUMP_PHYSCOLLIDESURFACE         = 49,
    LUMP_WATEROVERLAYS              = 50,
    LUMP_LEAF_AMBIENT_INDEX_HDR     = 51,
    LUMP_LEAF_AMBIENT_INDEX         = 52,
    LUMP_LIGHTING_HDR               = 53,
    LUMP_WORLDLIGHTS_HDR            = 54,
    LUMP_LEAF_AMBIENT_LIGHTING_HDR  = 55,
    LUMP_LEAF_AMBIENT_LIGHTING      = 56,
    LUMP_XZIPPAKFILE                = 57,
    LUMP_FACES_HDR                  = 58,
    LUMP_MAP_FLAGS                  = 59,
    LUMP_OVERLAY_FADES              = 60,
};

#define HEADER_LUMPS        64

struct lump_t
{
    int32_t fileofs, filelen;
    int32_t version;
    int8_t fourCC[4];
};

struct dheader_t
{
    int32_t ident;
    int32_t version;
    lump_t lumps[HEADER_LUMPS];
    int32_t mapRevision;
};

struct dmodel_t
{
    glm::vec3 mins, maxs;
    glm::vec3 origin;
    int32_t headnode;
    int32_t firstface, numfaces;
};

#define SURF_LIGHT      0x0001
#define SURF_SKY2D      0x0002
#define SURF_SKY        0x0004
#define SURF_WARP       0x0008
#define SURF_TRANS      0x0010
#define SURF_NOPORTAL   0x0020
#define SURF_TRIGGER    0x0040
#define SURF_NODRAW     0x0080

#define SURF_HINT       0x0100

#define SURF_SKIP       0x0200
#define SURF_NOLIGHT    0x0400
#define SURF_BUMPLIGHT  0x0800
#define SURF_NOSHADOWS  0x1000
#define SURF_NODECALS   0x2000
#define SURF_NOCHOP     0x4000
#define SURF_HITBOX     0x8000

struct texinfo_t
{
    glm::vec3 textureU;
    float textureUOffset;
    glm::vec3 textureV;
    float textureVOffset;
    glm::vec3 lightmapU;
    float lightmapUOffset;
    glm::vec3 lightmapV;
    float lightmapVOffset;
    int32_t flags;
    int32_t texdata;
};

struct dtexdata_t
{
    glm::vec3 reflectivity;
    int32_t nameStringTableID;
    int32_t width, height;
    int32_t view_width, view_height;
};

struct CDispSubNeighbor
{
    uint16_t m_iNeighbor;
    uint8_t m_NeighborOrientation;
    uint8_t m_Span;
    uint8_t m_NeighborSpan;
};

struct CDispNeighbor
{
    CDispSubNeighbor m_SubNeighbors[2];
};

struct CDispCornerNeighbors
{
    uint16_t m_Neighbors[MAX_DISP_CORNER_NEIGHBORS];
    uint8_t m_nNeighbors;
};

struct CDispVert
{
    glm::vec3 m_vVector;
    float m_flDist;
    float m_flAlpha;
};

struct ddispinfo_t
{
    glm::vec3 startPosition;
    int32_t m_iDispVertStart;
    int32_t m_iDispTriStart;
    int32_t power;
    int32_t minTess;
    float smoothingAngle;
    int32_t contents;
    uint16_t m_iMapFace;
    int32_t m_iLightmapAlphaStart;
    int32_t m_iLightmapSamplePositionStart;
    CDispNeighbor m_EdgeNeighbors[4];
    CDispCornerNeighbors m_CornerNeighbors[4];
    enum unnamed { ALLOWEDVERTS_SIZE = PAD_NUMBER( MAX_DISPVERTS, 32 ) / 32 };
    uint32_t m_AllowedVerts[ALLOWEDVERTS_SIZE];
};

struct dedge_t
{
    uint16_t v[2];
};

#define MAXLIGHTMAPS    4

struct dface_t
{
    uint16_t planenum;
    uint8_t side;
    uint8_t onNode;
    int32_t firstedge;
    int16_t numedges;
    int16_t texinfo;
    int16_t dispinfo;
    int16_t surfaceFogVolumeID;
    uint8_t styles[MAXLIGHTMAPS];
    int32_t lightofs;
    float area;
    int32_t m_LightmapTextureMinsInLuxels[2];
    int32_t m_LightmapTextureSizeInLuxels[2];
    int32_t origFace;
    uint16_t m_NumPrims;
    uint16_t firstPrimID;
    uint32_t smoothingGroups;
};

struct ColorRGBExp32
{
    uint8_t r, g, b;
    int8_t exponent;
};

struct Vertex;
struct Surface
{
    int index;
    std::vector<uint32_t> indices;
    std::vector<Vertex> vertexes;
};

class GameObject;
class Texture;
class BSP
{
public:
    void LoadBSPFile(std::string, bool);

private:
    std::ifstream pFile;

    dheader_t g_pBSPHeader;
    std::vector<dmodel_t> dmodels;
    std::vector<uint8_t> dlightdata;
    std::vector<char> dentdata;
    std::vector<glm::vec3> dvertexes;
    std::vector<texinfo_t> texinfo;
    std::vector<dtexdata_t> dtexdata;
    std::vector<ddispinfo_t> g_dispinfo;
    std::vector<CDispVert> g_DispVerts;
    std::vector<dface_t> dfaces;
    std::vector<dedge_t> dedges;
    std::vector<int32_t> dsurfedges;
    std::vector<char> g_TexDataStringData;
    std::vector<int32_t> g_TexDataStringTable;

    GameObject *root;

    void ParseEntities();
    Surface BuildFace(int);
    Surface BuildDisplacement(int);
    GameObject *BuildModel(int);
    Texture *PackLightmaps(std::vector<Surface> &);

    template<typename T>
    void CopyLump(int, std::vector<T> &);

    static glm::vec3 FlipVector(const glm::vec3 &);
};

#endif // BSP_H
