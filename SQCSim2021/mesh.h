#ifndef MESH_H__
#define MESH_H__

#include "../SQCSim-common/chunk.h"
#include "../SQCSim-common/world.h"
#include "define.h"
#include "vertexbuffer.h"

class Mesh {
private:
    VertexBuffer m_vertexBuffer;
    VertexBuffer::VertexData* m_vd = nullptr;
    int m_vcount = 0;
    bool m_new = true;
    Chunk* m_chunk; // NE PAS DÉTRUIRE ICI.

    void AddBlockToMesh(VertexBuffer::VertexData* vd, int& count, BlockType bt, int x, int y, int z, float u, float v, float s, World* world);
    void RemoveChunk(int nbReduit);
public:
    Mesh(Chunk* chunk);
    ~Mesh();

    void FlushMeshToVBO();
    void FlushVBO();
    void Update(BlockInfo* blockinfo[BTYPE_LAST], World* world);
    void Render() const;

    bool IsDirty() const;
    bool IsNew();
    void GetPosition(unsigned int& x, unsigned int& y, World* world) const;
};
#endif