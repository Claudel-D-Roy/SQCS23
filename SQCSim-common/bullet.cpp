#include "bullet.h"
#include "world.h"

Bullet::Bullet(Vector3f pos, Vector3f dir) : m_startpos(pos), m_currentpos(pos), m_velocity(dir) {}

Bullet::Bullet(Vector3f pos, Vector3f dir, uint64_t shooter_id) : m_startpos(pos), m_currentpos(pos), m_velocity(dir), m_shooter_id(shooter_id) {}


Bullet::Bullet(Vector3f pos, Vector3f dir, uint64_t shooter_id, bool canhurt): m_startpos(pos), m_currentpos(pos), m_velocity(dir), m_shooter_id(shooter_id), m_canhurt(canhurt) {}

Bullet::~Bullet() {}

bool Bullet::Update(World* world, float elapsedtime, int perframe, std::unordered_map<uint64_t, Player*> mapPlayer, netprot::ChunkMod** chunkmod) {
	int max = 100 / perframe;
	Player* shooter = nullptr;
	float damage = 0.098f;
	if (mapPlayer.count(m_shooter_id)) {
		shooter = mapPlayer.at(m_shooter_id);
		damage = shooter->boostdamage ? 0.123f : 0.098f;
	}

	for (int x = 0; x < max; ++x) {
		m_currentpos += m_velocity * elapsedtime;

		for (auto& [key, player] : mapPlayer) {
			if (key == m_shooter_id)
				continue;

			bool hit = false;
			if ((m_currentpos - player->GetPosition()).Length() < 1.5f) {
				hit = true;
			}
			else if ((m_currentpos - player->GetPOV()).Length() < .7f) {
				damage *= 2; // HEADSHOT!
				hit = true;
			}
			if (hit && !player->AmIDead()) {
				if (m_canhurt && !player->boostinvincible)
					player->InflictDamage(damage);
				player->m_hit = true;

				if (player->AmIDead())
					player->Killer = m_shooter_id;

				return true;
			}
		}

		if (!world->ChunkAt(m_currentpos))
			return true;
		else if (world->BlockAt(m_currentpos) != BTYPE_AIR) {
			if (m_canhurt) {
				if (chunkmod) {
					using namespace netprot;
					*chunkmod = new ChunkMod();
					(*chunkmod)->old_b_type = world->BlockAt(m_currentpos);
					(*chunkmod)->b_type = BTYPE_AIR;
					(*chunkmod)->pos = m_currentpos;
				}

				world->ChangeBlockAtPosition(BTYPE_AIR, m_currentpos);
			}
			return true;
		}
		else if ((m_currentpos - m_startpos).Length() > VIEW_DISTANCE) return true;
	}
	
	return false;
}

void Bullet::Transpose(int& x, int& z) {
	m_currentpos.x -= x * CHUNK_SIZE_X;
	m_currentpos.z -= z * CHUNK_SIZE_Z;
	m_startpos.x -= x * CHUNK_SIZE_X;
	m_startpos.z -= z * CHUNK_SIZE_Z;
}

Vector3f Bullet::getPos() const {
	return m_currentpos;
}

Vector3f Bullet::getVel() const {
	return m_velocity;
}

//uint64_t Bullet::getTeamID(){
//	return m_tid;
//}
