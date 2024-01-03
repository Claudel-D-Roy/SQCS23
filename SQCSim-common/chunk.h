#ifndef CHUNK_H__
#define CHUNK_H__

#include "define.h"
#include "array2d.h"
#include "array3d.h"
#include "blockinfo.h"
#include "opensimplex.h"

class World;

class Chunk {
    private:
        Array3d<BlockType> m_blocks = Array3d<BlockType>(CHUNK_SIZE_X, CHUNK_SIZE_Y, CHUNK_SIZE_Z);
        bool m_isDirty = true;
        bool m_isModified = false;

        unsigned int m_posX; // Position du chunk dans l'array constituant le monde.
        unsigned int m_posY;

    public:
        Chunk(unsigned int x, unsigned int y, int64_t seed);
        ~Chunk();

        void RemoveBlock(int x, int y, int z, World* world);
        void SetBlock(int x, int y, int z, BlockType type, World* world);
        BlockType GetBlock(int x, int y, int z);
        void CheckNeighbors(unsigned int x, unsigned int z, World* world);
        void GetPosition(unsigned int& x, unsigned int& y) const;
        void PlaceTree(int x, int y, int z, int height);

        void Structure(int x, int y, int z, int height);
        bool IsDirty() const;
        void MakeDirty();
        void MakeClean();
        void MakeModified();
};

#endif // CHUNK_H__
