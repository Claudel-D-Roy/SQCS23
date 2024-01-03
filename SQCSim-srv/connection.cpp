#include "connection.h"



Connection::Connection(SOCKET sock,
	sockaddr_in sockaddr,
	LoginInfo *log,
	PlayerInfo *play) :
	m_sock(sock),
	m_addr(sockaddr),
	m_loginfo(*log),
	m_playinfo(*play) {

}

Connection::~Connection() { 
	delete player;
	closesocket(m_sock); }

uint64_t Connection::GetHash(bool self) const { return self ? m_loginfo.sid : m_playinfo.id; }

uint64_t Connection::GetTeamHash() const { return m_loginfo.tid; }

std::string Connection::GetName() const { return m_loginfo.name; }

void Connection::AddInput(Input in) { m_input_manifest.insert({ in.timestamp, in }); m_input_vector.push_back(in); }

Output* Connection::getOutput(Timestamp time) {
	auto out = m_output_manifest.find(time);
	if (out != m_output_manifest.end())
		return &out->second;
	return nullptr;
}

Sync Connection::getSync(Timestamp time) {
	Sync sync;
	auto out = m_output_manifest.find(time);
	if (out != m_output_manifest.end()) {
		sync.timestamp = out->second.timestamp;
		sync.position = out->second.position;
		sync.sid = m_loginfo.sid;
	}
	return sync;
}

SOCKET Connection::getSock() const { return m_sock; }

PlayerInfo* Connection::getInfo() const { return (PlayerInfo*)&m_playinfo; }

sockaddr_in* Connection::getAddr() const { return (sockaddr_in*)&m_addr; }

void Connection::getPacks(SOCKET sock) {
	std::vector<char*> lsPck;
	Input in;
	Sync sync;
	recvPacksFrom(sock, &m_buf, m_addr, &lsPck);
	for (auto& pck : lsPck) {
		uint32_t bsize = m_buf.len - (pck - m_buf.ptr);
		switch (netprot::getType(pck, 1)) {
			using enum netprot::PACKET_TYPE;
		case INPUT:
			if (player->AmIDead())
				break;
			else if (Deserialize(&in, pck, &bsize)) {
				m_input_manifest[in.timestamp] = in;
				m_input_vector.push_back(in);
			}
			break;
		case SYNC:
			if (Deserialize(&sync, pck, &bsize))
				m_nsync = true;
			break;
		default: break;
		}
	}
	lsPck.clear();
}

void Connection::sendPacks(SOCKET sock, std::unordered_map<uint64_t, Connection*> conns, const uint32_t timer) {
	static int outs = 0;
	static Timestamp last = 0;
	static uint32_t lasttimer = timer;

	if (m_output_vector.empty() && player->AmIDead()) {
		if (timer != lasttimer) {
			lasttimer = timer;
			Sync sync;
			sync.timestamp = sync.sid = m_loginfo.sid;
			sync.hp = 0;
			sync.ammo = -1;
			sync.timer = timer;
			sendPackTo<Sync>(sock, &sync, &m_bufout, &m_addr);
		}
	}

	while (!m_output_vector.empty()) {
		Output out = m_output_vector.front();
		
		for (auto& [key, conn] : conns) {
			if (m_playinfo.id == conn->GetHash(false))
				continue;
			sendPackTo<Output>(sock, &out, &m_bufout, conn->getAddr());
		}
		++outs;

		[[unlikely]] if (last == 0) // !
			last = out.timestamp;

		outs += out.timestamp + last;

		static bool syncdead = false;
		bool dead = player->AmIDead();

		if (outs >= SYNC_ACC || (!syncdead && dead)) {
			Sync sync;
			outs -= SYNC_ACC;
			sync.timestamp = out.timestamp;
			sync.position = out.position;
			sync.sid = m_loginfo.sid;
			sync.timer = timer;
			sync.timestamp = out.timestamp;
			sync.ammo = -1;
			sync.hp = player->GetHP();
			if (dead) {
				sync.timestamp = m_loginfo.sid;
				syncdead = true;
			}
			sendPackTo<Sync>(sock, &sync, &m_bufout, &m_addr);
		}

		m_output_vector.pop_front();
	}
}

Timestamp Connection::Run(World* world, std::unordered_map<uint64_t, Booster*> boosters) {
	Input in, last;
	Output out;
	Timestamp tstamp = 0;
	float el;

	bool dead = player->AmIDead();

	if (m_input_manifest.size() < 2 && !dead)
		return tstamp;

	while (m_last_in < m_input_vector.size() - 1 || dead) {
		if (!dead) {
			in = m_input_vector.at(m_last_in + 1);
			last = m_input_vector.at(m_last_in);
			if (in.timestamp <= m_tstamp) {
				++m_last_in;
				continue;
			}
			el = (double)(in.timestamp - last.timestamp) / 1000.;
			if (m_shoot_acc > 0.) {
				m_shoot_acc -= el;
				if (m_shoot_acc < 0.)
					m_shoot_acc = 0.;
			}
			player->SetDirection(in.direction);
		}
		else {
			el = 1. / 60.;
			in = Input();
			
		}
		
		player->ApplyPhysics(player->GetInput(in.keys.forward,
			in.keys.backward,
			in.keys.left,
			in.keys.right,
			in.keys.jump, false, el), 
			world, el);

		player->TakeBooster(boosters, el);

		if (player->GetPosition().y < -20.) {
			player->InflictDamage(9000.);
			player->Killer = GetHash(true);
		}

		out.states.jumping = player->GetIsAirborne();

		Vector3f horSpeed = player->GetVelocity();
		horSpeed.y = 0;
		out.states.running = horSpeed.Length() > .2f;
		out.states.still = !out.states.running && !out.states.jumping;
		out.states.hit = player->m_hit;
		player->m_hit = false;

		if (player->AmIDead()) {
			in.keys.shoot = false;
			in.keys.block = false;
			out.states.dead = true;
		}

		static bool toggle = false;
		if (in.keys.block) {
			if (!toggle) {
				toggle = true;
				bool block = false;
				ChunkMod* cmod = world->ChangeBlockAtCursor(BLOCK_TYPE::BTYPE_METAL,
					player->GetPosition(),
					player->GetDirection(),
					block, true);
				if (cmod)
					ChunkDiffs.push_back(std::move(cmod));
			}
		}
		else toggle = false;

		out.states.shooting = in.keys.shoot;

		if (out.states.jumping && out.states.shooting)
			out.states.jumpshot = true;
		else out.states.jumpshot = false;

		if (in.keys.shoot && m_shoot_acc <= 0.) {
			Bullets.emplace_back(new Bullet(player->GetPOV() + player->GetDirection(), player->GetDirection(), GetHash(true)));
			m_shoot_acc = BULLET_TIME;
		}

		out.position = player->GetPositionAbs();
		out.direction = in.direction;
		out.timestamp = in.timestamp;
		out.id = m_playinfo.id;
		m_output_manifest[out.timestamp] = out;
		m_output_vector.push_back(out);
		m_tstamp = tstamp = out.timestamp;

		if (!dead)
			++m_last_in;

		dead = false;
	}

	return tstamp;
}

void Connection::CleanInputManifest(Timestamp time) {
	//	auto wat = m_input_manifest.find(time);

	//	while (wat != m_input_manifest.begin())
	//		m_input_manifest.erase(wat--);
}

Timestamp Connection::GetTStamp() const { return m_tstamp; }
