#include "world.h"

World::World() {}

World::~World() {}

Array2d<Chunk*>& World::GetChunks() { return m_chunks; }

void World::BuildWorld() {
	for (int x = 0; x < WORLD_SIZE_X; ++x)
		for (int y = 0; y < WORLD_SIZE_Y; ++y)
			m_chunks.Set(x, y, new Chunk(x + m_center[0], y + m_center[1], m_seed));
}

void World::SetSeed(uint64_t seed) {
	m_seed = seed;
}




Chunk* World::ChunkAt(float x, float y, float z) const {
	int cx = (int)x / CHUNK_SIZE_X;
	int cz = (int)z / CHUNK_SIZE_Z;

	if (x < 0 || y < 0 || z < 0 ||
		x >= WORLD_SIZE_X * CHUNK_SIZE_X ||
		z >= CHUNK_SIZE_Z * WORLD_SIZE_Y ||
		y > CHUNK_SIZE_Y)
		return 0;

	return m_chunks.Get(cx, cz);
}

Chunk* World::ChunkAt(const Vector3f& pos) const { return ChunkAt(pos.x, pos.y, pos.z); }

void World::RemoveChunk(int nbReduit)
{
	for (int x = 0; x < WORLD_SIZE_X; ++x)
		for (int y = 0; y < WORLD_SIZE_Y; ++y)
		{
			Chunk* chk = nullptr;
			if (x < nbReduit)
				chk = m_chunks.Remove(x, y);
			if (y < nbReduit)
				chk = m_chunks.Remove(x, y);
			if (y > WORLD_SIZE_Y - nbReduit)
				chk = m_chunks.Remove(x, y);
			if (x > WORLD_SIZE_X - nbReduit)
				chk = m_chunks.Remove(x, y);

			if (!chk)
				continue;

			m_tbDeleted.emplace_back(chk);
		}


}

BlockType World::BlockAt(float x, float y, float z, BlockType defaultBlockType) const {
	Chunk* c = ChunkAt(x, y, z);

	if (!c)
		return defaultBlockType;

	int bx = (int)x % CHUNK_SIZE_X;
	int by = (int)y % CHUNK_SIZE_Y;
	int bz = (int)z % CHUNK_SIZE_Z;

	return c->GetBlock(bx, by, bz);
}

BlockType World::BlockAt(const Vector3f& pos, BlockType defaultBlockType) const {
	return BlockAt(pos.x, pos.y, pos.z, defaultBlockType);
}

void World::TransposeWorld(Vector3f& player, Bullet* bullets[MAX_BULLETS]) {
	int x = 0, y = 0;

	if (player.x > (WORLD_SIZE_X * CHUNK_SIZE_X) * .6f) ++x;
	else if (player.x < (WORLD_SIZE_X * CHUNK_SIZE_X) * .4f) --x;
	if (player.z > (WORLD_SIZE_Y * CHUNK_SIZE_Z) * .6f) ++y;
	else if (player.z < (WORLD_SIZE_Y * CHUNK_SIZE_Z) * .4f) --y;

	if (!x && !y) return;

	if (x > 0) {
		for (int ax = 0; ax < WORLD_SIZE_X; ++ax)
			for (int ay = 0; ay < WORLD_SIZE_Y; ++ay)
				if (ax - x >= 0) {
					m_chunks.Set(ax - x, ay,
						m_chunks.Remove(ax, ay));
					if (ax == WORLD_SIZE_X - 1 && m_chunks.Get(ax - x, ay))
						m_chunks.Get(ax - x, ay)->MakeDirty();
				}
				else if (m_chunks.Get(ax, ay)) m_tbDeleted.emplace_back(m_chunks.Remove(ax, ay));
	}
	else if (x < 0) {
		for (int ax = WORLD_SIZE_X - 1; ax >= 0; --ax)
			for (int ay = WORLD_SIZE_Y - 1; ay >= 0; --ay)
				if (ax - x < WORLD_SIZE_X) {
					m_chunks.Set(ax - x, ay,
						m_chunks.Remove(ax, ay));
					if (ax == 0 && m_chunks.Get(ax - x, ay))
						m_chunks.Get(ax - x, ay)->MakeDirty();
				}
				else if (m_chunks.Get(ax, ay)) m_tbDeleted.emplace_back(m_chunks.Remove(ax, ay));
	}

	if (y > 0) {
		for (int ax = 0; ax < WORLD_SIZE_X; ++ax)
			for (int ay = 0; ay < WORLD_SIZE_Y; ++ay)
				if (ay - y >= 0) {
					m_chunks.Set(ax, ay - y,
						m_chunks.Remove(ax, ay));
					if (ay == WORLD_SIZE_Y - 1 && m_chunks.Get(ax, ay - y))
						m_chunks.Get(ax, ay - y)->MakeDirty();
				}
				else if (m_chunks.Get(ax, ay)) m_tbDeleted.emplace_back(m_chunks.Remove(ax, ay));
	}
	else if (y < 0) {
		for (int ax = WORLD_SIZE_X - 1; ax >= 0; --ax)
			for (int ay = WORLD_SIZE_Y - 1; ay >= 0; --ay)
				if (ay - y < WORLD_SIZE_Y) {
					m_chunks.Set(ax, ay - y,
						m_chunks.Remove(ax, ay));
					if (ay == 0 && m_chunks.Get(ax, ay - y))
						m_chunks.Get(ax, ay - y)->MakeDirty();
				}
				else if (m_chunks.Get(ax, ay)) m_tbDeleted.emplace_back(m_chunks.Remove(ax, ay));
	}

	m_center[0] += x; m_center[1] += y;
	player.x -= x * CHUNK_SIZE_X;
	player.z -= y * CHUNK_SIZE_Z;

	for (int index = 0; index < MAX_BULLETS; ++index)
		if (bullets[index]) bullets[index]->Transpose(x, y);
}

void World::CleanUpWorld(int& deleteframes, bool clear = false) {
	if (clear) {
		while (m_tbDeleted.size() > 0) {
			delete m_tbDeleted.back();
			m_tbDeleted.pop_back();
		}
	}
	if (!m_tbDeleted.empty() && !deleteframes) {
		int deleted = 0;
		while (deleted < THREADS_DELETE_CHUNKS) {


		}
		delete m_tbDeleted.back();
		m_tbDeleted.pop_back();
		deleteframes = FRAMES_DELETE_CHUNKS;
	}
}

void World::GetScope(unsigned int& x, unsigned int& y) {
	x = m_center[0];
	y = m_center[1];
}

void World::Update(Bullet* bullets[MAX_BULLETS], const Vector3f& player_pos, BlockInfo* blockinfo[BTYPE_LAST]) {
	UpdateWorld(player_pos, blockinfo);
}

netprot::ChunkMod* World::ChangeBlockAtCursor(BlockType blockType, const Vector3f& player_pos, const Vector3f& player_dir, bool& block, bool net) {
	Vector3f currentPos = player_pos;
	Vector3f currentBlock = currentPos;
	Vector3f ray = player_dir;
	BlockType oldbtype;
	netprot::ChunkMod* cmod = nullptr;
	bool found = false;

	if (block) return cmod;

	while ((currentPos - currentBlock).Length() <= MAX_SELECTION_DISTANCE && !found) {
		currentBlock += ray / 10.f;

		BlockType bt = BlockAt(currentBlock);

		if (bt != BTYPE_AIR) {
			found = true;
			oldbtype = bt;
		}
	}

	if (found)
		if (blockType != BTYPE_AIR) {
			found = false;
			while ((currentPos - currentBlock).Length() >= 1.7f && !found) {
				currentBlock -= ray / 10.f;

				BlockType bt = BlockAt(currentBlock);

				if (bt == BTYPE_AIR) { // Vérification pour être sûr que le bloc à changer n'est pas dans le joueur.
					int Bx = (int)currentBlock.x;
					int By = (int)currentBlock.y;
					int Bz = (int)currentBlock.z;

					int Px = (int)currentPos.x;
					int PyA = (int)currentPos.y;
					int PyB = (int)(currentPos.y - .9f);
					int PyC = (int)(currentPos.y - 1.7f);
					int Pz = (int)currentPos.z;

					if (!(Bx == Px &&
						(By == PyA ||
							By == PyB ||
							By == PyC) &&
						Bz == Pz)) {
						found = true;
						oldbtype = bt;
						}
				}
			}
		}

	if (found && (int)currentBlock.y < CHUNK_SIZE_Y) {
		if (net) {
			cmod = new netprot::ChunkMod();
			cmod->old_b_type = oldbtype;
			cmod->b_type = blockType;
			cmod->pos = currentBlock;
		}
		int bx = (int)currentBlock.x % CHUNK_SIZE_X;
		int by = (int)currentBlock.y % CHUNK_SIZE_Y;
		int bz = (int)currentBlock.z % CHUNK_SIZE_Z;
		ChunkAt(currentBlock)->SetBlock(bx, by, bz, blockType, this);
		ChunkAt(currentBlock)->MakeModified();
		block = true;
	}

	return cmod;
}

void World::ChangeBlockAtPosition(BlockType blockType, Vector3f pos) {
	int bx = (int)pos.x % CHUNK_SIZE_X;
	int by = (int)pos.y % CHUNK_SIZE_Y;
	int bz = (int)pos.z % CHUNK_SIZE_Z;

	ChunkAt(pos)->SetBlock(bx, by, bz, blockType, this);
	ChunkAt(pos)->MakeModified();
}

void World::UpdateWorld(const Vector3f& player, BlockInfo* blockinfo[BTYPE_LAST]) {
	int cx = player.x;
	int cy = player.z;
	static int frameGenerate = 1;
	static int frameUpdate = 2;
	static int frameDelete = 3;
	int side = 0;
	int threads = 0;
	std::future<Chunk*> genThList[THREADS_GENERATE_CHUNKS];
	
	if (frameGenerate > 0) --frameGenerate;
	if (frameUpdate > 0) --frameUpdate;
	if (frameDelete > 0) --frameDelete;
	
	if (!frameGenerate)
		while (side * CHUNK_SIZE_X <= VIEW_DISTANCE * 2 + CHUNK_SIZE_X) {
			int tx = -side, ty = -side;
			int chx = 0;
			int chy = 0;

			for (; tx <= side; ++tx) {
				if (frameGenerate)
					break;
				chx = cx + tx * CHUNK_SIZE_X;
				chy = cy + ty * CHUNK_SIZE_Z;
				if (chx < WORLD_SIZE_X * CHUNK_SIZE_X && chy < WORLD_SIZE_Y * CHUNK_SIZE_Z &&
					chx >= 0 && chy >= 0 && !ChunkAt(chx, 1, chy))
					genThList[threads++] = std::async(std::launch::async, 
						[](unsigned int x, unsigned int y, uint64_t seed) { 
							return new Chunk(x, y, seed); }, 
							chx / CHUNK_SIZE_X + m_center[0], 
							chy / CHUNK_SIZE_Z + m_center[1], 
							m_seed);
				if (threads == THREADS_GENERATE_CHUNKS) frameGenerate = FRAMES_RENDER_CHUNKS;
			}
			for (; ty <= side; ++ty) {
				if (frameGenerate)
					break;
				chx = cx + tx * CHUNK_SIZE_X;
				chy = cy + ty * CHUNK_SIZE_Z;
				if (chx < WORLD_SIZE_X * CHUNK_SIZE_X && chy < WORLD_SIZE_Y * CHUNK_SIZE_Z &&
					chx >= 0 && chy >= 0 && !ChunkAt(chx, 1, chy))
					genThList[threads++] = std::async(std::launch::async, 
						[](unsigned int x, unsigned int y, uint64_t seed) { 
							return new Chunk(x, y, seed); }, 
							chx / CHUNK_SIZE_X + m_center[0], 
							chy / CHUNK_SIZE_Z + m_center[1], 
							m_seed);
				if (threads == THREADS_GENERATE_CHUNKS) frameGenerate = FRAMES_RENDER_CHUNKS;
			}
			for (; tx >= -side; --tx) {
				if (frameGenerate)
					break;
				chx = cx + tx * CHUNK_SIZE_X;
				chy = cy + ty * CHUNK_SIZE_Z;
				if (chx < WORLD_SIZE_X * CHUNK_SIZE_X && chy < WORLD_SIZE_Y * CHUNK_SIZE_Z &&
					chx >= 0 && chy >= 0 && !ChunkAt(chx, 1, chy))
					genThList[threads++] = std::async(std::launch::async, 
						[](unsigned int x, unsigned int y, uint64_t seed) { 
							return new Chunk(x, y, seed); }, 
							chx / CHUNK_SIZE_X + m_center[0], 
							chy / CHUNK_SIZE_Z + m_center[1], 
							m_seed);
				if (threads == THREADS_GENERATE_CHUNKS) frameGenerate = FRAMES_RENDER_CHUNKS;
			}
			for (; ty >= -side; --ty) {
				if (frameGenerate)
					break;
				chx = cx + tx * CHUNK_SIZE_X;
				chy = cy + ty * CHUNK_SIZE_Z;
				if (chx < WORLD_SIZE_X * CHUNK_SIZE_X && chy < WORLD_SIZE_Y * CHUNK_SIZE_Z &&
					chx >= 0 && chy >= 0 && !ChunkAt(chx, 1, chy))
					genThList[threads++] = std::async(std::launch::async, 
						[](unsigned int x, unsigned int y, uint64_t seed) { 
							return new Chunk(x, y, seed); }, 
							chx / CHUNK_SIZE_X + m_center[0], 
							chy / CHUNK_SIZE_Z + m_center[1],
							m_seed);
				if (threads == THREADS_GENERATE_CHUNKS) frameGenerate = FRAMES_RENDER_CHUNKS;
			}
			if (frameGenerate)
				break;
			++side;
		}

	if (threads > 0) {
		for (int i = 0; i < threads; ++i)
			genThList[i].wait();

		for (int i = 0; i < threads; ++i) {
			unsigned int x, y;
			Chunk* chunk = genThList[i].get();
			chunk->GetPosition(x, y);
			m_chunks.Set(x - m_center[0], y - m_center[1], chunk);
		}
	}

	side = 0;
	threads = 0;
}

int World::GettbDeleted() const { return m_tbDeleted.size(); }
