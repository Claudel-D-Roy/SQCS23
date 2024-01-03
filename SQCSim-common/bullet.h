#ifndef BULLET_H__
#define BULLET_H__

#include <unordered_map>
#include "define.h"
#include "vector3.h"
#include "player.h"
#include "netprotocol.h"

class World;
class Player;

class Bullet {
public:
	Bullet(Vector3f pos, Vector3f dir);
	Bullet(Vector3f pos, Vector3f dir, uint64_t tid);
	Bullet(Vector3f pos, Vector3f dir, uint64_t tid, bool canhurt);
	~Bullet();

	bool Update(World* world, float elapsedtime, int perframe, std::unordered_map<uint64_t, Player*> m_mapPlayer, netprot::ChunkMod** chunkmod);
	void Transpose(int& x, int& z);
	Vector3f getPos() const;
	Vector3f getVel() const;
	//uint64_t getTeamID();

private:
	Vector3f m_startpos, 
			 m_currentpos, 
			 m_velocity;
	uint64_t m_shooter_id = 0;

	bool m_canhurt = true;
	

};

#endif // BULLET_H__

