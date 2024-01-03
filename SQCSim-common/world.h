#ifndef WORLD_H__
#define WORLD_H__

#include <fstream>
#include <string>
#include <vector>
#include <future>
#include <thread>
#include "define.h"
#include "vector3.h"
#include "array2d.h"
#include "bullet.h"
#include "chunk.h"
#include "netprotocol.h"

class Chunk;
class Bullet;

class World {
public:
	World();
	~World();

	Array2d<Chunk*>& GetChunks();
	void BuildWorld();

	void SetSeed(uint64_t seed);

	Chunk* ChunkAt(float x, float y, float z) const;
	Chunk* ChunkAt(const Vector3f& pos) const;

	void RemoveChunk(int nbReduit);

	BlockType BlockAt(float x, float y, float z, BlockType defaultBlockType = BTYPE_AIR) const;
	BlockType BlockAt(const Vector3f& pos, BlockType defaultBlockType = BTYPE_AIR) const;

	void Update(Bullet* bullets[MAX_BULLETS], const Vector3f& player_pos, BlockInfo* blockinfo[BTYPE_LAST]);

	void GetScope(unsigned int& x, unsigned int& y);

	netprot::ChunkMod* ChangeBlockAtCursor(BlockType blockType, const Vector3f& player_pos, const Vector3f& player_dir, bool& block, bool net);
	void ChangeBlockAtPosition(BlockType blockType, Vector3f pos);
	void CleanUpWorld(int& deleteframes, bool clear);
	int GettbDeleted() const;
private:
	Array2d<Chunk*> m_chunks = Array2d<Chunk*>(WORLD_SIZE_X, WORLD_SIZE_Y);
	std::vector<Chunk*> m_tbDeleted;
	uint64_t m_seed = 0;

	unsigned int m_center[2] = { UINT16_MAX / 2 - WORLD_SIZE_X, UINT16_MAX / 2 - WORLD_SIZE_Y };

	//void UpdateChunk(int& updates, unsigned int chx, unsigned int chy, BlockInfo* blockinfo[BTYPE_LAST]);
	void UpdateWorld(const Vector3f& player, BlockInfo* blockinfo[BTYPE_LAST]);
	void TransposeWorld(Vector3f& player, Bullet* bullets[MAX_BULLETS]);

};
#endif // WORLD_H__

