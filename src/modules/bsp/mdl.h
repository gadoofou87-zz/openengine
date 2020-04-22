#ifndef MDL_H
#define MDL_H

#include <glm/glm.hpp>
#include <glm/gtc/quaternion.hpp>

#include <cstdint>

#define MAX_NUM_BONES_PER_VERT 3

#define MAX_NUM_LODS 8

struct studiohdr_t
{
    int32_t id;
    int32_t version;
    int32_t checksum;
    int8_t name[64];
    int32_t length;
    glm::vec3 eyeposition;
    glm::vec3 illumposition;
    glm::vec3 hull_min;
    glm::vec3 hull_max;
    glm::vec3 view_bbmin;
    glm::vec3 view_bbmax;
    int32_t flags;
    int32_t numbones;
    int32_t boneindex;
    int32_t numbonecontrollers;
    int32_t bonecontrollerindex;
    int32_t numhitboxsets;
    int32_t hitboxsetindex;
    int32_t numlocalanim;
    int32_t localanimindex;
    int32_t numlocalseq;
    int32_t localseqindex;
    int32_t activitylistversion;
    int32_t eventsindexed;
    int32_t numtextures;
    int32_t textureindex;
    int32_t numcdtextures;
    int32_t cdtextureindex;
    int32_t numskinref;
    int32_t numskinfamilies;
    int32_t skinindex;
    int32_t numbodyparts;
    int32_t bodypartindex;
    int32_t numlocalattachments;
    int32_t localattachmentindex;
    int32_t numlocalnodes;
    int32_t localnodeindex;
    int32_t localnodenameindex;
    int32_t numflexdesc;
    int32_t flexdescindex;
    int32_t numflexcontrollers;
    int32_t flexcontrollerindex;
    int32_t numflexrules;
    int32_t flexruleindex;
    int32_t numikchains;
    int32_t ikchainindex;
    int32_t nummouths;
    int32_t mouthindex;
    int32_t numlocalposeparameters;
    int32_t localposeparamindex;
    int32_t surfacepropindex;
    int32_t keyvalueindex;
    int32_t keyvaluesize;
    int32_t numlocalikautoplaylocks;
    int32_t localikautoplaylockindex;
    float mass;
    int32_t contents;
    int32_t numincludemodels;
    int32_t includemodelindex;
    int32_t virtualModel;
    int32_t szanimblocknameindex;
    int32_t numanimblocks;
    int32_t animblockindex;
    int32_t animblockModel;
    int32_t bonetablebynameindex;
    int32_t pVertexBase;
    int32_t pIndexBase;
    uint8_t constdirectionallightdot;
    uint8_t rootLOD;
    uint8_t numAllowedRootLODs;
    uint8_t unused[1];
    int32_t unused4;
    int32_t numflexcontrollerui;
    int32_t flexcontrolleruiindex;
    float flVertAnimFixedPointScale;
    int32_t unused3[1];
    int32_t studiohdr2index;
    int32_t unused2[1];
};

struct mstudiotexture_t
{
    int32_t sznameindex;
    int32_t flags;
    int32_t used;
    int32_t unused1;
    int32_t material;
    int32_t clientmaterial;
    int32_t unused[10];
};

struct mstudiobodyparts_t
{
    int32_t sznameindex;
    int32_t nummodels;
    int32_t base;
    int32_t modelindex;
};

struct mstudio_modelvertexdata_t
{
    int32_t pVertexData;
    int32_t pTangentData;
};

struct mstudiomodel_t
{
    int8_t name[64];
    int32_t type;
    float boundingradius;
    int32_t nummeshes;
    int32_t meshindex;
    int32_t numvertices;
    int32_t vertexindex;
    int32_t tangentsindex;
    int32_t numattachments;
    int32_t attachmentindex;
    int32_t numeyeballs;
    int32_t eyeballindex;
    mstudio_modelvertexdata_t vertexdata;
    int32_t unused[8];
};

struct mstudio_meshvertexdata_t
{
    int32_t modelvertexdata;
    int32_t numLODVertexes[MAX_NUM_LODS];
};

struct mstudiomesh_t
{
    int32_t material;
    int32_t modelindex;
    int32_t numvertices;
    int32_t vertexoffset;
    int32_t numflexes;
    int32_t flexindex;
    int32_t materialtype;
    int32_t materialparam;
    int32_t meshid;
    glm::vec3 center;
    mstudio_meshvertexdata_t vertexdata;
    int32_t unused[8];
};

struct mstudioboneweight_t
{
    float weight[MAX_NUM_BONES_PER_VERT];
    int8_t bone[MAX_NUM_BONES_PER_VERT];
    uint8_t numbones;
};

struct mstudiovertex_t
{
    mstudioboneweight_t m_BoneWeights;
    glm::vec3 m_vecPosition;
    glm::vec3 m_vecNormal;
    glm::vec2 m_vecTexCoord;
};

// little-endian "IDSV"
#define MODEL_VERTEX_FILE_ID		(('V'<<24)+('S'<<16)+('D'<<8)+'I')
#define MODEL_VERTEX_FILE_VERSION	4

struct vertexFileHeader_t
{
    int32_t id;
    int32_t version;
    int32_t checksum;
    int32_t numLODs;
    int32_t numLODVertexes[MAX_NUM_LODS];
    int32_t numFixups;
    int32_t fixupTableStart;
    int32_t vertexDataStart;
    int32_t tangentDataStart;
};

struct vertexFileFixup_t
{
    int32_t lod;
    int32_t sourceVertexID;
    int32_t numVertexes;
};

#define OPTIMIZED_MODEL_FILE_VERSION 7

struct FileHeader_t
{
    int32_t version;
    int32_t vertCacheSize;
    uint16_t maxBonesPerStrip;
    uint16_t maxBonesPerTri;
    int32_t maxBonesPerVert;
    int32_t checkSum;
    int32_t numLODs;
    int32_t materialReplacementListOffset;
    int32_t numBodyParts;
    int32_t bodyPartOffset;
};

struct BodyPartHeader_t
{
    int32_t numModels;
    int32_t modelOffset;
};

struct ModelHeader_t
{
    int32_t numLODs;
    int32_t lodOffset;
};

struct ModelLODHeader_t
{
    int32_t numMeshes;
    int32_t meshOffset;
    float switchPoint;
};

struct MeshHeader_t
{
    int32_t numStripGroups;
    int32_t stripGroupHeaderOffset;
    uint8_t flags;
};

struct StripGroupHeader_t
{
    int32_t numVerts;
    int32_t vertOffset;
    int32_t numIndices;
    int32_t indexOffset;
    int32_t numStrips;
    int32_t stripOffset;
    uint8_t flags;
};

struct StripHeader_t
{
    int32_t numIndices;
    int32_t indexOffset;
    int32_t numVerts;
    int32_t vertOffset;
    int16_t numBones;
    uint8_t flags;
    int32_t numBoneStateChanges;
    int32_t boneStateChangeOffset;
};

struct Vertex_t
{
    uint8_t boneWeightIndex[MAX_NUM_BONES_PER_VERT];
    uint8_t numBones;
    uint16_t origMeshVertID;
    int8_t boneID[MAX_NUM_BONES_PER_VERT];
};

#endif // MDL_H
