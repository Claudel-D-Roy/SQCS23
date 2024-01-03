#include "mesh.h"

Mesh::Mesh(Chunk* chunk): m_chunk(chunk) {}

Mesh::~Mesh() {
    FlushVBO();
}

void Mesh::FlushMeshToVBO() {
    m_vertexBuffer.SetMeshData(m_vd, m_vcount);
    m_vcount = 0;
    delete[] m_vd;
}

void Mesh::FlushVBO() {
    m_vertexBuffer.Flush();
}

void Mesh::Update(BlockInfo* blockinfo[BTYPE_LAST], World* world) {
    float u, v, s;
    // Update mesh
    if (m_chunk->IsDirty()) {
        int maxVertexCount = (CHUNK_SIZE_X * CHUNK_SIZE_Y * CHUNK_SIZE_Z) * (6 * 4);
        m_vd = new VertexBuffer::VertexData[maxVertexCount];
        m_vcount = 0;
        for (int x = 0; x < CHUNK_SIZE_X; ++x) {
            for (int z = 0; z < CHUNK_SIZE_Z; ++z) {
                for (int y = 0; y < CHUNK_SIZE_Y; ++y) {
                    if (m_vcount > USHRT_MAX)
                        break;

                    BlockType bt = m_chunk->GetBlock(x, y, z);

                    if (bt != BTYPE_AIR) {
                        blockinfo[bt]->GetTexture(u, v, s);
                        AddBlockToMesh(m_vd, m_vcount, bt, x, y, z, u, v, s, world);
                    }
                }
            }
        }
        if (m_vcount > USHRT_MAX) {
            m_vcount = USHRT_MAX;
            std::cout << "[ Chunk :: Update ] Chunk data truncaned , too much vertices to have a 16 bit index " << std::endl;
        }
    }
    m_chunk->MakeClean();
}

void Mesh::AddBlockToMesh(VertexBuffer::VertexData* vd, int& count, BlockType bt,
    int x, int y, int z, float u, float v, float s, World* world) {

    unsigned int cex, cey, cpx, cpy;

    m_chunk->GetPosition(cpx, cpy);

    world->GetScope(cex, cey);

    int cx = x + (cpx - cex) * CHUNK_SIZE_X, cy = z + (cpy - cey) * CHUNK_SIZE_Z;

    if (y == CHUNK_SIZE_Y - 1 || m_chunk->GetBlock(x, y + 1, z) == BTYPE_AIR) {                      //  y
        vd[count++] = VertexBuffer::VertexData(x, y + 1.f, z, .8f, .8f, .8f, u, v);
        vd[count++] = VertexBuffer::VertexData(x, y + 1.f, z + 1.f, .8f, .8f, .8f, u, v + s);
        vd[count++] = VertexBuffer::VertexData(x + 1.f, y + 1.f, z + 1.f, .8f, .8f, .8f, u + s, v + s);
        vd[count++] = VertexBuffer::VertexData(x + 1.f, y + 1.f, z, .8f, .8f, .8f, u + s, v);
    }

    if (y == 0 || m_chunk->GetBlock(x, y - 1, z) == BTYPE_AIR) {                                     // -y
        vd[count++] = VertexBuffer::VertexData(x, y, z + 1.f, .2f, .2f, .2f, u, v);
        vd[count++] = VertexBuffer::VertexData(x, y, z, .2f, .2f, .2f, u, v + s);
        vd[count++] = VertexBuffer::VertexData(x + 1.f, y, z, .2f, .2f, .2f, u + s, v + s);
        vd[count++] = VertexBuffer::VertexData(x + 1.f, y, z + 1.f, .2f, .2f, .2f, u + s, v);
    }

    if (world->BlockAt(cx + 1, y, cy) == BTYPE_AIR) {                      //  x
        vd[count++] = VertexBuffer::VertexData(x + 1.f, y, z, .9f, .9f, .9f, u, v);
        vd[count++] = VertexBuffer::VertexData(x + 1.f, y + 1.f, z, .9f, .9f, .9f, u, v + s);
        vd[count++] = VertexBuffer::VertexData(x + 1.f, y + 1.f, z + 1.f, .9f, .9f, .9f, u + s, v + s);
        vd[count++] = VertexBuffer::VertexData(x + 1.f, y, z + 1.f, .9f, .9f, .9f, u + s, v);
    }

    if (world->BlockAt(cx - 1, y, cy) == BTYPE_AIR) {                                     // -x
        vd[count++] = VertexBuffer::VertexData(x, y + 1.f, z + 1.f, .5f, .5f, .5f, u, v + s);
        vd[count++] = VertexBuffer::VertexData(x, y + 1.f, z, .5f, .5f, .5f, u + s, v + s);
        vd[count++] = VertexBuffer::VertexData(x, y, z, .5f, .5f, .5f, u + s, v);
        vd[count++] = VertexBuffer::VertexData(x, y, z + 1.f, .5f, .5f, .5f, u, v);
    }

    if (world->BlockAt(cx, y, cy + 1) == BTYPE_AIR) {                      //  z
        vd[count++] = VertexBuffer::VertexData(x, y, z + 1.f, .4f, .4f, .4f, u, v);
        vd[count++] = VertexBuffer::VertexData(x + 1.f, y, z + 1.f, .4f, .4f, .4f, u + s, v);
        vd[count++] = VertexBuffer::VertexData(x + 1.f, y + 1.f, z + 1.f, .4f, .4f, .4f, u + s, v + s);
        vd[count++] = VertexBuffer::VertexData(x, y + 1.f, z + 1.f, .4f, .4f, .4f, u, v + s);
    }

    if (world->BlockAt(cx, y, cy - 1) == BTYPE_AIR) {                                     // -z
        vd[count++] = VertexBuffer::VertexData(x, y + 1.f, z, 1.f, 1.f, 1.f, u, v + s);
        vd[count++] = VertexBuffer::VertexData(x + 1.f, y + 1.f, z, 1.f, 1.f, 1.f, u + s, v + s);
        vd[count++] = VertexBuffer::VertexData(x + 1.f, y, z, 1.f, 1.f, 1.f, u + s, v);
        vd[count++] = VertexBuffer::VertexData(x, y, z, 1.f, 1.f, 1.f, u, v);
    }
}

void Mesh::Render() const { m_vertexBuffer.Render(); }

bool Mesh::IsDirty() const { return m_chunk->IsDirty(); }

bool Mesh::IsNew() {
    bool bl = m_new;
    if (m_new)
        m_new = false;
    return bl;
}

void Mesh::GetPosition(unsigned int& x, unsigned int& y, World* world) const {
    unsigned int sx, sy;
    world->GetScope(sx, sy);
    m_chunk->GetPosition(x, y);
    x -= sx;
    y -= sy;
}
