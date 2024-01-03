#include "netprotocol.h"

void netprot::Serialize(Input* in, char* buf[], uint32_t* buflen) {
	*buf[0] = (char)netprot::PACKET_TYPE::INPUT;

	uint64_t time = in->timestamp;
	uint8_t time8[sizeof(uint64_t)] = { (uint8_t)((time >> 56) & 0xFF),
									   (uint8_t)((time >> 48) & 0xFF),
									   (uint8_t)((time >> 40) & 0xFF),
									   (uint8_t)((time >> 32) & 0xFF),
									   (uint8_t)((time >> 24) & 0xFF),
									   (uint8_t)((time >> 16) & 0xFF),
									   (uint8_t)((time >> 8) & 0xFF),
										(uint8_t)(time & 0xFF) };

	memcpy(*buf + 1, time8, sizeof(uint64_t));

	uint64_t sid = in->sid;
	uint8_t sid8[sizeof(uint64_t)] = { (uint8_t)((sid >> 56) & 0xFF),
									  (uint8_t)((sid >> 48) & 0xFF),
									  (uint8_t)((sid >> 40) & 0xFF),
									  (uint8_t)((sid >> 32) & 0xFF),
									  (uint8_t)((sid >> 24) & 0xFF),
									  (uint8_t)((sid >> 16) & 0xFF),
									  (uint8_t)((sid >> 8) & 0xFF),
									  (uint8_t)(sid & 0xFF) };

	memcpy(*buf + sizeof(uint64_t) + 1, sid8, sizeof(uint64_t));

	Keys keys = in->keys;
	uint8_t keys8 = // Reste un bit.
		(keys.forward ? 0b10000000 : 0) |
		(keys.backward ? 0b01000000 : 0) |
		(keys.left ? 0b00100000 : 0) |
		(keys.right ? 0b00010000 : 0) |
		(keys.jump ? 0b00001000 : 0) |
		(keys.shoot ? 0b00000100 : 0) |
		(keys.block ? 0b00000010 : 0);

	memcpy(*buf + sizeof(uint64_t) * 2 + 1, &keys8, sizeof(uint8_t));

	uint32_t vec[3];
	memcpy(vec, &in->direction, sizeof(Vector3f)); // Pour d�naturer les floats.

	uint8_t vec8[3 * sizeof(uint32_t)] = {
							(uint8_t)((vec[0] >> 24) & 0xFF),
							(uint8_t)((vec[0] >> 16) & 0xFF),
							(uint8_t)((vec[0] >> 8) & 0xFF),
							 (uint8_t)(vec[0] & 0xFF),
						(uint8_t)((vec[1] >> 24) & 0xFF),
						(uint8_t)((vec[1] >> 16) & 0xFF),
						(uint8_t)((vec[1] >> 8) & 0xFF),
						 (uint8_t)(vec[1] & 0xFF),
					 (uint8_t)((vec[2] >> 24) & 0xFF),
					 (uint8_t)((vec[2] >> 16) & 0xFF),
					(uint8_t)((vec[2] >> 8) & 0xFF),
					 (uint8_t)(vec[2] & 0xFF) };

	memcpy(*buf + sizeof(uint64_t) * 2 + 2, vec8, sizeof(uint32_t) * 3);

	*buflen = sizeof(uint64_t) * 2 + 2 + sizeof(uint32_t) * 3;
}

void netprot::Serialize(Output* out, char* buf[], uint32_t* buflen) {
	*buf[0] = (char)netprot::PACKET_TYPE::OUTPUT;

	uint64_t time = out->timestamp;
	uint8_t time8[sizeof(uint64_t)] = { (uint8_t)((time >> 56) & 0xFF),
									   (uint8_t)((time >> 48) & 0xFF),
									   (uint8_t)((time >> 40) & 0xFF),
									   (uint8_t)((time >> 32) & 0xFF),
									   (uint8_t)((time >> 24) & 0xFF),
									   (uint8_t)((time >> 16) & 0xFF),
									   (uint8_t)((time >> 8) & 0xFF),
										(uint8_t)(time & 0xFF) };

	memcpy(*buf + 1, time8, sizeof(uint64_t));

	uint64_t sid = out->id;
	uint8_t sid8[sizeof(uint64_t)] = { (uint8_t)((sid >> 56) & 0xFF),
									  (uint8_t)((sid >> 48) & 0xFF),
									  (uint8_t)((sid >> 40) & 0xFF),
									  (uint8_t)((sid >> 32) & 0xFF),
									  (uint8_t)((sid >> 24) & 0xFF),
									  (uint8_t)((sid >> 16) & 0xFF),
									  (uint8_t)((sid >> 8) & 0xFF),
									  (uint8_t)(sid & 0xFF) };

	memcpy(*buf + sizeof(uint64_t) + 1, sid8, sizeof(uint64_t));

	States states = out->states;
	uint8_t states8 =
		(states.jumping ? 0b10000000 : 0) |
		(states.shooting ? 0b01000000 : 0) |
		(states.hit ? 0b00100000 : 0) |
		(states.powerup ? 0b00010000 : 0) |
		(states.dead ? 0b00001000 : 0) |
		(states.still ? 0b00000100 : 0) |
		(states.jumpshot ? 0b00000010 : 0) |
		(states.running ? 0b00000001 : 0);

	memcpy(*buf + sizeof(uint64_t) * 2 + 1, &states8, sizeof(uint8_t));

	uint32_t vec[3];
	memcpy(vec, &out->direction, sizeof(Vector3f)); // Pour d�naturer les floats.

	uint8_t vec8[3 * sizeof(uint32_t)] = {
							(uint8_t)((vec[0] >> 24) & 0xFF),
							(uint8_t)((vec[0] >> 16) & 0xFF),
							(uint8_t)((vec[0] >> 8) & 0xFF),
							 (uint8_t)(vec[0] & 0xFF),
						(uint8_t)((vec[1] >> 24) & 0xFF),
						(uint8_t)((vec[1] >> 16) & 0xFF),
						(uint8_t)((vec[1] >> 8) & 0xFF),
						 (uint8_t)(vec[1] & 0xFF),
					 (uint8_t)((vec[2] >> 24) & 0xFF),
					 (uint8_t)((vec[2] >> 16) & 0xFF),
					(uint8_t)((vec[2] >> 8) & 0xFF),
					 (uint8_t)(vec[2] & 0xFF) };

	memcpy(*buf + sizeof(uint64_t) * 2 + 2, vec8, sizeof(uint32_t) * 3);

	memcpy(vec, &out->position, sizeof(Vector3f)); // Pour d�naturer les floats.

	uint8_t vec82[3 * sizeof(uint32_t)] = {
							(uint8_t)((vec[0] >> 24) & 0xFF),
							(uint8_t)((vec[0] >> 16) & 0xFF),
							(uint8_t)((vec[0] >> 8) & 0xFF),
							 (uint8_t)(vec[0] & 0xFF),
						(uint8_t)((vec[1] >> 24) & 0xFF),
						(uint8_t)((vec[1] >> 16) & 0xFF),
						(uint8_t)((vec[1] >> 8) & 0xFF),
						 (uint8_t)(vec[1] & 0xFF),
					 (uint8_t)((vec[2] >> 24) & 0xFF),
					 (uint8_t)((vec[2] >> 16) & 0xFF),
					(uint8_t)((vec[2] >> 8) & 0xFF),
					 (uint8_t)(vec[2] & 0xFF) };

	memcpy(*buf + sizeof(uint64_t) * 2 + 2 + sizeof(uint32_t) * 3, vec82, sizeof(uint32_t) * 3);

	*buflen = sizeof(uint64_t) * 2 + 2 + sizeof(uint32_t) * 6;
}

void netprot::Serialize(Sync* sync, char* buf[], uint32_t* buflen) {
	*buf[0] = (char)netprot::PACKET_TYPE::SYNC;
	uint64_t time = sync->timestamp;
	uint8_t stamp8[sizeof(uint64_t)] = { (uint8_t)((time >> 56) & 0xFF),
									   (uint8_t)((time >> 48) & 0xFF),
									   (uint8_t)((time >> 40) & 0xFF),
									   (uint8_t)((time >> 32) & 0xFF),
									   (uint8_t)((time >> 24) & 0xFF),
									   (uint8_t)((time >> 16) & 0xFF),
									   (uint8_t)((time >> 8) & 0xFF),
										(uint8_t)(time & 0xFF) };

	memcpy(*buf + 1, stamp8, sizeof(uint64_t));

	uint64_t sid = sync->sid;
	uint8_t sid8[sizeof(uint64_t)] = { (uint8_t)((sid >> 56) & 0xFF),
									  (uint8_t)((sid >> 48) & 0xFF),
									  (uint8_t)((sid >> 40) & 0xFF),
									  (uint8_t)((sid >> 32) & 0xFF),
									  (uint8_t)((sid >> 24) & 0xFF),
									  (uint8_t)((sid >> 16) & 0xFF),
									  (uint8_t)((sid >> 8) & 0xFF),
									  (uint8_t)(sid & 0xFF) };

	memcpy(*buf + sizeof(uint64_t) + 1, sid8, sizeof(uint64_t));

	uint32_t timer = sync->timer;
	uint8_t time8[sizeof(uint32_t)] = { (uint8_t)((timer >> 24) & 0xFF),
									   (uint8_t)((timer >> 16) & 0xFF),
									   (uint8_t)((timer >> 8) & 0xFF),
										(uint8_t)(timer & 0xFF) };

	memcpy(*buf + sizeof(uint64_t) * 2 + 1, time8, sizeof(uint32_t));

	uint16_t ammo = sync->ammo;
	uint8_t ammo8[sizeof(uint16_t)] = { (uint8_t)((ammo >> 8) & 0xFF),
										(uint8_t)(ammo & 0xFF) };

	memcpy(*buf + sizeof(uint64_t) * 2 + sizeof(uint32_t) + 1, ammo8, sizeof(uint16_t));

	uint32_t vec[3];
	memcpy(vec, &sync->position, sizeof(Vector3f)); // Pour d�naturer les floats.

	uint8_t vec8[3 * sizeof(uint32_t)] = {
							(uint8_t)((vec[0] >> 24) & 0xFF),
							(uint8_t)((vec[0] >> 16) & 0xFF),
							(uint8_t)((vec[0] >> 8) & 0xFF),
							 (uint8_t)(vec[0] & 0xFF),
						(uint8_t)((vec[1] >> 24) & 0xFF),
						(uint8_t)((vec[1] >> 16) & 0xFF),
						(uint8_t)((vec[1] >> 8) & 0xFF),
						 (uint8_t)(vec[1] & 0xFF),
					 (uint8_t)((vec[2] >> 24) & 0xFF),
					 (uint8_t)((vec[2] >> 16) & 0xFF),
					(uint8_t)((vec[2] >> 8) & 0xFF),
					 (uint8_t)(vec[2] & 0xFF) };

	memcpy(*buf + sizeof(uint64_t) * 2 + sizeof(uint32_t) + sizeof(uint16_t) + 1, vec8, sizeof(uint32_t) * 3);

	uint32_t hp;

	memcpy(&hp, &sync->hp, sizeof(float));

	uint8_t hp8[4] = {
					(uint8_t)((hp >> 24) & 0xFF),
					(uint8_t)((hp >> 16) & 0xFF),
					(uint8_t)((hp >> 8) & 0xFF),
					(uint8_t)(hp & 0xFF) };

	memcpy(*buf + sizeof(uint64_t) * 2 + sizeof(uint32_t) * 4 + sizeof(uint16_t) + 1, hp8, sizeof(float));

	Boosts boost = sync->boost;
	uint8_t boost8 = // Reste 6 bits.
		(boost.invincible ? 0b10000000 : 0) |
		(boost.damage ? 0b01000000 : 0);

	memcpy(*buf + sizeof(uint64_t) * 2 + sizeof(uint32_t) * 5 + sizeof(uint16_t) + 1, &boost8, sizeof(uint8_t));

	*buflen = sizeof(uint64_t) * 2 + sizeof(uint32_t) * 5 + sizeof(uint16_t) + sizeof(float) + 2;
}

void netprot::Serialize(TeamInfo* tinfo, char* buf[], uint32_t* buflen) {
	*buf[0] = (char)netprot::PACKET_TYPE::TEAMINF;

	size_t namesize = std::strlen(tinfo->name) + 1;

	strcpy(*buf + 1, namesize, tinfo->name);

	uint64_t tid = tinfo->id;
	uint8_t tid8[sizeof(uint64_t)] = {
									   (uint8_t)((tid >> 56) & 0xFF),
									   (uint8_t)((tid >> 48) & 0xFF),
									   (uint8_t)((tid >> 40) & 0xFF),
									   (uint8_t)((tid >> 32) & 0xFF),
									   (uint8_t)((tid >> 24) & 0xFF),
									   (uint8_t)((tid >> 16) & 0xFF),
									   (uint8_t)((tid >> 8) & 0xFF),
										(uint8_t)(tid & 0xFF)
	};

	memcpy(*buf + namesize + 2, tid8, sizeof(uint64_t));

	*buflen = namesize + sizeof(uint64_t) + 2;
}

void netprot::Serialize(LoginInfo* linfo, char* buf[], uint32_t* buflen) {
	*buf[0] = (char)netprot::PACKET_TYPE::LOGINF;

	size_t namesize = std::strlen(linfo->name) + 1;

	strcpy(*buf + 1, namesize, linfo->name);

	uint64_t sid = linfo->sid;
	uint8_t sid8[sizeof(uint64_t)] = {
									   (uint8_t)((sid >> 56) & 0xFF),
									   (uint8_t)((sid >> 48) & 0xFF),
									   (uint8_t)((sid >> 40) & 0xFF),
									   (uint8_t)((sid >> 32) & 0xFF),
									   (uint8_t)((sid >> 24) & 0xFF),
									   (uint8_t)((sid >> 16) & 0xFF),
									   (uint8_t)((sid >> 8) & 0xFF),
										(uint8_t)(sid & 0xFF)
	};

	memcpy(*buf + namesize + 2, sid8, sizeof(uint64_t));

	uint64_t tid = linfo->tid;
	uint8_t tid8[sizeof(uint64_t)] = {
									   (uint8_t)((tid >> 56) & 0xFF),
									   (uint8_t)((tid >> 48) & 0xFF),
									   (uint8_t)((tid >> 40) & 0xFF),
									   (uint8_t)((tid >> 32) & 0xFF),
									   (uint8_t)((tid >> 24) & 0xFF),
									   (uint8_t)((tid >> 16) & 0xFF),
									   (uint8_t)((tid >> 8) & 0xFF),
									   (uint8_t)(tid & 0xFF)
	};

	memcpy(*buf + namesize + 2 + sizeof(uint64_t), tid8, sizeof(uint64_t));

	*buflen = namesize + sizeof(uint64_t) * 2 + 2;
}

void netprot::Serialize(PlayerInfo* pinfo, char* buf[], uint32_t* buflen) {
	*buf[0] = (char)netprot::PACKET_TYPE::PLAYINF;

	size_t namesize = std::strlen(pinfo->name) + 1;

	strcpy(*buf + 1, namesize, pinfo->name);

	uint64_t id = pinfo->id;
	uint8_t id8[sizeof(uint64_t)] = {
									   (uint8_t)((id >> 56) & 0xFF),
									   (uint8_t)((id >> 48) & 0xFF),
									   (uint8_t)((id >> 40) & 0xFF),
									   (uint8_t)((id >> 32) & 0xFF),
									   (uint8_t)((id >> 24) & 0xFF),
									   (uint8_t)((id >> 16) & 0xFF),
									   (uint8_t)((id >> 8) & 0xFF),
										(uint8_t)(id & 0xFF)
	};

	memcpy(*buf + namesize + 2, id8, sizeof(uint64_t));

	uint64_t tid = pinfo->tid;
	uint8_t tid8[sizeof(uint64_t)] = {
									   (uint8_t)((tid >> 56) & 0xFF),
									   (uint8_t)((tid >> 48) & 0xFF),
									   (uint8_t)((tid >> 40) & 0xFF),
									   (uint8_t)((tid >> 32) & 0xFF),
									   (uint8_t)((tid >> 24) & 0xFF),
									   (uint8_t)((tid >> 16) & 0xFF),
									   (uint8_t)((tid >> 8) & 0xFF),
										(uint8_t)(tid & 0xFF)
	};

	memcpy(*buf + namesize + 2 + sizeof(uint64_t), tid8, sizeof(uint64_t));

	*buflen = namesize + sizeof(uint64_t) * 2 + 2;
}

void netprot::Serialize(GameInfo* ginfo, char* buf[], uint32_t* buflen) {
	*buf[0] = (char)netprot::PACKET_TYPE::GAMEINFO;

	uint64_t game = ginfo->seed;
	uint8_t seed8[sizeof(uint64_t)] = {
									   (uint8_t)((game >> 56) & 0xFF),
									   (uint8_t)((game >> 48) & 0xFF),
									   (uint8_t)((game >> 40) & 0xFF),
									   (uint8_t)((game >> 32) & 0xFF),
									   (uint8_t)((game >> 24) & 0xFF),
									   (uint8_t)((game >> 16) & 0xFF),
									   (uint8_t)((game >> 8) & 0xFF),
										(uint8_t)(game & 0xFF)
	};

	memcpy(*buf + 1, seed8, sizeof(uint64_t));

	game = ginfo->countdown;
	uint8_t count8[sizeof(uint64_t)] = {
										(uint8_t)((game >> 56) & 0xFF),
										(uint8_t)((game >> 48) & 0xFF),
										(uint8_t)((game >> 40) & 0xFF),
										(uint8_t)((game >> 32) & 0xFF),
										(uint8_t)((game >> 24) & 0xFF),
										(uint8_t)((game >> 16) & 0xFF),
										(uint8_t)((game >> 8) & 0xFF),
										 (uint8_t)(game & 0xFF)
	};

	memcpy(*buf + sizeof(uint64_t) + 1, count8, sizeof(uint64_t));

	game = ginfo->countdown;
	uint8_t gtype8[sizeof(uint64_t)] = {
										 (uint8_t)((game >> 56) & 0xFF),
									   (uint8_t)((game >> 48) & 0xFF),
									   (uint8_t)((game >> 40) & 0xFF),
									   (uint8_t)((game >> 32) & 0xFF),
									   (uint8_t)((game >> 24) & 0xFF),
									   (uint8_t)((game >> 16) & 0xFF),
									   (uint8_t)((game >> 8) & 0xFF),
										(uint8_t)(game & 0xFF)
	};

	memcpy(*buf + sizeof(uint64_t) + 1, gtype8, sizeof(uint64_t));

	*buflen = sizeof(uint64_t) * 3 + 1;
}

void netprot::Serialize(Chat* chat, char* buf[], uint32_t* buflen) {
	*buf[0] = (char)netprot::PACKET_TYPE::CHAT;

	uint64_t src = chat->src_id;
	uint8_t src8[sizeof(uint64_t)] = {
									   (uint8_t)((src >> 56) & 0xFF),
									   (uint8_t)((src >> 48) & 0xFF),
									   (uint8_t)((src >> 40) & 0xFF),
									   (uint8_t)((src >> 32) & 0xFF),
									   (uint8_t)((src >> 24) & 0xFF),
									   (uint8_t)((src >> 16) & 0xFF),
									   (uint8_t)((src >> 8) & 0xFF),
										(uint8_t)(src & 0xFF)
	};

	memcpy(*buf + 1, src8, sizeof(uint64_t));

	uint64_t dst = chat->dest_id;
	uint8_t dst8[sizeof(uint64_t)] = {
									   (uint8_t)((dst >> 56) & 0xFF),
									   (uint8_t)((dst >> 48) & 0xFF),
									   (uint8_t)((dst >> 40) & 0xFF),
									   (uint8_t)((dst >> 32) & 0xFF),
									   (uint8_t)((dst >> 24) & 0xFF),
									   (uint8_t)((dst >> 16) & 0xFF),
									   (uint8_t)((dst >> 8) & 0xFF),
										(uint8_t)(dst & 0xFF)
	};

	memcpy(*buf + 1 + sizeof(uint64_t), dst8, sizeof(uint64_t));

	uint64_t dstteam = chat->dest_id;
	uint8_t dstt8[sizeof(uint64_t)] = {
									   (uint8_t)((dstteam >> 56) & 0xFF),
									   (uint8_t)((dstteam >> 48) & 0xFF),
									   (uint8_t)((dstteam >> 40) & 0xFF),
									   (uint8_t)((dstteam >> 32) & 0xFF),
									   (uint8_t)((dstteam >> 24) & 0xFF),
									   (uint8_t)((dstteam >> 16) & 0xFF),
									   (uint8_t)((dstteam >> 8) & 0xFF),
										(uint8_t)(dstteam & 0xFF)
	};

	memcpy(*buf + 1 + sizeof(uint64_t) * 2, dstt8, sizeof(uint64_t));

	size_t messize = std::strlen(chat->mess) + 1;

	strcpy(*buf + 1 + sizeof(uint64_t) * 3, messize, chat->mess);

	*buflen = messize + sizeof(uint64_t) * 3 + 2;
}

void netprot::Serialize(ChunkMod* chmod, char* buf[], uint32_t* buflen) {
	*buf[0] = (char)netprot::PACKET_TYPE::CHUNKMOD;

	uint32_t vec[3];
	memcpy(vec, &chmod->pos, sizeof(Vector3f)); // Pour d�naturer les floats.

	uint8_t vec8[3 * sizeof(uint32_t)] = {
							(uint8_t)((vec[0] >> 24) & 0xFF),
							(uint8_t)((vec[0] >> 16) & 0xFF),
							(uint8_t)((vec[0] >> 8) & 0xFF),
							 (uint8_t)(vec[0] & 0xFF),
						(uint8_t)((vec[1] >> 24) & 0xFF),
						(uint8_t)((vec[1] >> 16) & 0xFF),
						(uint8_t)((vec[1] >> 8) & 0xFF),
						 (uint8_t)(vec[1] & 0xFF),
					 (uint8_t)((vec[2] >> 24) & 0xFF),
					 (uint8_t)((vec[2] >> 16) & 0xFF),
					(uint8_t)((vec[2] >> 8) & 0xFF),
					 (uint8_t)(vec[2] & 0xFF) };

	memcpy(*buf + 1, vec8, sizeof(uint32_t) * 3);

	memcpy(*buf + sizeof(uint32_t) * 3 + 1, &chmod->b_type, sizeof(BlockType));
	memcpy(*buf + sizeof(uint32_t) * 3 + 2, &chmod->old_b_type, sizeof(BlockType));

	*buflen = sizeof(uint32_t) * 3 + 3;
}

void netprot::Serialize(BulletAdd* bull, char* buf[], uint32_t* buflen) {
	*buf[0] = (char)netprot::PACKET_TYPE::BULLET;

	uint64_t tstamp = bull->tstamp;
	uint8_t ts8[sizeof(uint64_t)] = {
									   (uint8_t)((tstamp >> 56) & 0xFF),
									   (uint8_t)((tstamp >> 48) & 0xFF),
									   (uint8_t)((tstamp >> 40) & 0xFF),
									   (uint8_t)((tstamp >> 32) & 0xFF),
									   (uint8_t)((tstamp >> 24) & 0xFF),
									   (uint8_t)((tstamp >> 16) & 0xFF),
									   (uint8_t)((tstamp >> 8) & 0xFF),
										(uint8_t)(tstamp & 0xFF)
	};

	memcpy(*buf + 1, ts8, sizeof(uint64_t));

	uint64_t tid = bull->id;
	uint8_t tid8[sizeof(uint64_t)] = {
									   (uint8_t)((tid >> 56) & 0xFF),
									   (uint8_t)((tid >> 48) & 0xFF),
									   (uint8_t)((tid >> 40) & 0xFF),
									   (uint8_t)((tid >> 32) & 0xFF),
									   (uint8_t)((tid >> 24) & 0xFF),
									   (uint8_t)((tid >> 16) & 0xFF),
									   (uint8_t)((tid >> 8) & 0xFF),
										(uint8_t)(tid & 0xFF)
	};

	memcpy(*buf + 1 + sizeof(uint64_t), tid8, sizeof(uint64_t));

	uint32_t vec[3];
	memcpy(vec, &bull->pos, sizeof(Vector3f)); // Pour d�naturer les floats.

	uint8_t vec8[3 * sizeof(uint32_t)] = {
							(uint8_t)((vec[0] >> 24) & 0xFF),
							(uint8_t)((vec[0] >> 16) & 0xFF),
							(uint8_t)((vec[0] >> 8) & 0xFF),
							 (uint8_t)(vec[0] & 0xFF),
						(uint8_t)((vec[1] >> 24) & 0xFF),
						(uint8_t)((vec[1] >> 16) & 0xFF),
						(uint8_t)((vec[1] >> 8) & 0xFF),
						 (uint8_t)(vec[1] & 0xFF),
					 (uint8_t)((vec[2] >> 24) & 0xFF),
					 (uint8_t)((vec[2] >> 16) & 0xFF),
					(uint8_t)((vec[2] >> 8) & 0xFF),
					 (uint8_t)(vec[2] & 0xFF) };

	memcpy(*buf + 1 + sizeof(uint64_t) * 2, vec8, sizeof(uint32_t) * 3);

	memcpy(vec, &bull->dir, sizeof(Vector3f)); // Pour d�naturer les floats.

	uint8_t dir8[3 * sizeof(uint32_t)] = {
							(uint8_t)((vec[0] >> 24) & 0xFF),
							(uint8_t)((vec[0] >> 16) & 0xFF),
							(uint8_t)((vec[0] >> 8) & 0xFF),
							 (uint8_t)(vec[0] & 0xFF),
						(uint8_t)((vec[1] >> 24) & 0xFF),
						(uint8_t)((vec[1] >> 16) & 0xFF),
						(uint8_t)((vec[1] >> 8) & 0xFF),
						 (uint8_t)(vec[1] & 0xFF),
					 (uint8_t)((vec[2] >> 24) & 0xFF),
					 (uint8_t)((vec[2] >> 16) & 0xFF),
					(uint8_t)((vec[2] >> 8) & 0xFF),
					 (uint8_t)(vec[2] & 0xFF) };

	memcpy(*buf + 1 + sizeof(uint64_t) * 2 + sizeof(uint32_t) * 3, dir8, sizeof(uint32_t) * 3);

	*buflen = 1 + sizeof(uint64_t) * 2 + sizeof(uint32_t) * 6;
}

void netprot::Serialize(PickupMod* pmod, char* buf[], uint32_t* buflen) {
	*buf[0] = (char)netprot::PACKET_TYPE::PICKUPMOD;

	uint64_t id = pmod->id;
	uint8_t id8[sizeof(uint64_t)] = { (uint8_t)((id >> 56) & 0xFF),
									   (uint8_t)((id >> 48) & 0xFF),
									   (uint8_t)((id >> 40) & 0xFF),
									   (uint8_t)((id >> 32) & 0xFF),
									   (uint8_t)((id >> 24) & 0xFF),
									   (uint8_t)((id >> 16) & 0xFF),
									   (uint8_t)((id >> 8) & 0xFF),
										(uint8_t)(id & 0xFF) };

	memcpy(*buf + 1, id8, sizeof(uint64_t));

	uint32_t vec[3];
	memcpy(vec, &pmod->pos, sizeof(Vector3f)); // Pour d�naturer les floats.

	uint8_t vec8[3 * sizeof(uint32_t)] = {
							(uint8_t)((vec[0] >> 24) & 0xFF),
							(uint8_t)((vec[0] >> 16) & 0xFF),
							(uint8_t)((vec[0] >> 8) & 0xFF),
							 (uint8_t)(vec[0] & 0xFF),
						(uint8_t)((vec[1] >> 24) & 0xFF),
						(uint8_t)((vec[1] >> 16) & 0xFF),
						(uint8_t)((vec[1] >> 8) & 0xFF),
						 (uint8_t)(vec[1] & 0xFF),
					 (uint8_t)((vec[2] >> 24) & 0xFF),
					 (uint8_t)((vec[2] >> 16) & 0xFF),
					(uint8_t)((vec[2] >> 8) & 0xFF),
					 (uint8_t)(vec[2] & 0xFF) };

	memcpy(*buf + sizeof(uint64_t) + 2, vec8, sizeof(uint32_t) * 3);

	Boosts boost = pmod->boost;
	uint8_t boost8 = // Reste 5 bits.
		(boost.invincible ? 0b10000000 : 0) |
		(boost.damage ? 0b01000000 : 0) |
		(boost.hp ? 0b00100000 : 0);

	memcpy(*buf + sizeof(uint64_t) + sizeof(uint32_t) * 3 + 1, &boost8, sizeof(uint8_t));

	memcpy(*buf + 2 + sizeof(uint64_t) + sizeof(uint32_t) * 3, &pmod->available, sizeof(bool));

	*buflen = 2 + sizeof(uint64_t) + sizeof(uint32_t) * 3 + 3;
}

void netprot::Serialize(ErrorLog* errlog, char* buf[], uint32_t* buflen) {
	*buf[0] = (char)netprot::PACKET_TYPE::ERRLOG;

	size_t messize = std::strlen(errlog->mess) + 1;

	memcpy(*buf + 1, &errlog->mess, messize);

	memcpy(*buf + 1 + messize, &errlog->is_fatal, sizeof(bool));

	*buflen = messize + sizeof(bool) + 1;
}




bool netprot::Deserialize(Input* in, char* buf, uint32_t* buflen) {
	if (*buflen <= sizeof(Input))
		return false;

	uint8_t diff[sizeof(uint64_t)] = { 0,0,0,0,0,0,0,0 };
	memcpy(diff, &buf[1], sizeof(uint64_t));
	in->timestamp =
		(uint64_t)diff[0] << 56 |
		(uint64_t)diff[1] << 48 |
		(uint64_t)diff[2] << 40 |
		(uint64_t)diff[3] << 32 |
		(uint64_t)diff[4] << 24 |
		(uint64_t)diff[5] << 16 |
		(uint64_t)diff[6] << 8 |
		(uint64_t)diff[7];

	memcpy(diff, &buf[1 + sizeof(uint64_t)], sizeof(uint64_t));
	in->sid =
		(uint64_t)diff[0] << 56 |
		(uint64_t)diff[1] << 48 |
		(uint64_t)diff[2] << 40 |
		(uint64_t)diff[3] << 32 |
		(uint64_t)diff[4] << 24 |
		(uint64_t)diff[5] << 16 |
		(uint64_t)diff[6] << 8 |
		(uint64_t)diff[7];

	uint8_t keys = 0;
	memcpy(&keys, &buf[1 + sizeof(uint64_t) * 2], sizeof(uint8_t));
	in->keys.forward = keys & 0b10000000;
	in->keys.backward = keys & 0b01000000;
	in->keys.left = keys & 0b00100000;
	in->keys.right = keys & 0b00010000;
	in->keys.jump = keys & 0b00001000;
	in->keys.shoot = keys & 0b00000100;
	in->keys.block = keys & 0b00000010;

	uint8_t subvec[3 * sizeof(uint32_t)] = { 0,0,0,0,0,0,0,0,0,0,0,0 };
	memcpy(subvec, &buf[2 + sizeof(uint64_t) * 2], sizeof(uint8_t) * 12);
	uint32_t vec[3] = {
		(uint32_t)subvec[0] << 24 |
		(uint32_t)subvec[1] << 16 |
		(uint32_t)subvec[2] << 8 |
		(uint32_t)subvec[3],
			(uint32_t)subvec[4] << 24 |
			(uint32_t)subvec[5] << 16 |
			(uint32_t)subvec[6] << 8 |
			(uint32_t)subvec[7],
				(uint32_t)subvec[8] << 24 |
				(uint32_t)subvec[9] << 16 |
				(uint32_t)subvec[10] << 8 |
				(uint32_t)subvec[11] };

	memcpy(&in->direction, vec, sizeof(uint32_t) * 3);

	*buflen = sizeof(uint64_t) * 2 + 2 + sizeof(uint32_t) * 3;

	return true;
}

bool netprot::Deserialize(Output* out, char* buf, uint32_t* buflen) {
	if (*buflen <= sizeof(Output))
		return false;

	uint8_t diff[sizeof(uint64_t)] = { 0,0,0,0,0,0,0,0 };
	memcpy(diff, &buf[1], sizeof(uint64_t));
	out->timestamp =
		(uint64_t)diff[0] << 56 |
		(uint64_t)diff[1] << 48 |
		(uint64_t)diff[2] << 40 |
		(uint64_t)diff[3] << 32 |
		(uint64_t)diff[4] << 24 |
		(uint64_t)diff[5] << 16 |
		(uint64_t)diff[6] << 8 |
		(uint64_t)diff[7];

	memcpy(diff, &buf[1 + sizeof(uint64_t)], sizeof(uint64_t));
	out->id =
		(uint64_t)diff[0] << 56 |
		(uint64_t)diff[1] << 48 |
		(uint64_t)diff[2] << 40 |
		(uint64_t)diff[3] << 32 |
		(uint64_t)diff[4] << 24 |
		(uint64_t)diff[5] << 16 |
		(uint64_t)diff[6] << 8 |
		(uint64_t)diff[7];

	uint8_t states = 0;
	memcpy(&states, &buf[1 + sizeof(uint64_t) * 2], sizeof(uint8_t));
	out->states.jumping = states & 0b10000000;
	out->states.shooting = states & 0b01000000;
	out->states.hit = states & 0b00100000;
	out->states.powerup = states & 0b00010000;
	out->states.dead = states & 0b00001000;
	out->states.still = states & 0b00000100;
	out->states.jumpshot = states & 0b00000010;
	out->states.running = states & 0b00000001;


	uint8_t subvec[3 * sizeof(uint32_t)] = { 0,0,0,0,0,0,0,0,0,0,0,0 };
	memcpy(subvec, &buf[2 + sizeof(uint64_t) * 2], sizeof(uint8_t) * 12);
	uint32_t vec[3] = {
		(uint32_t)subvec[0] << 24 |
		(uint32_t)subvec[1] << 16 |
		(uint32_t)subvec[2] << 8 |
		(uint32_t)subvec[3],
			(uint32_t)subvec[4] << 24 |
			(uint32_t)subvec[5] << 16 |
			(uint32_t)subvec[6] << 8 |
			(uint32_t)subvec[7],
				(uint32_t)subvec[8] << 24 |
				(uint32_t)subvec[9] << 16 |
				(uint32_t)subvec[10] << 8 |
				(uint32_t)subvec[11] };

	memcpy(&out->direction, vec, sizeof(uint32_t) * 3);

	memcpy(subvec, &buf[2 + sizeof(uint64_t) * 2 + sizeof(uint32_t) * 3], sizeof(uint8_t) * 12);
	uint32_t vec2[3] = {
		(uint32_t)subvec[0] << 24 |
		(uint32_t)subvec[1] << 16 |
		(uint32_t)subvec[2] << 8 |
		(uint32_t)subvec[3],
			(uint32_t)subvec[4] << 24 |
			(uint32_t)subvec[5] << 16 |
			(uint32_t)subvec[6] << 8 |
			(uint32_t)subvec[7],
				(uint32_t)subvec[8] << 24 |
				(uint32_t)subvec[9] << 16 |
				(uint32_t)subvec[10] << 8 |
				(uint32_t)subvec[11] };

	memcpy(&out->position, vec2, sizeof(uint32_t) * 3);

	*buflen = sizeof(uint64_t) * 2 + 2 + sizeof(uint32_t) * 6;

	return true;
}

bool netprot::Deserialize(Sync* sync, char* buf, uint32_t* buflen) {
	if (*buflen <= sizeof(Sync))
		return false;

	uint8_t diff[sizeof(uint64_t)] = { 0,0,0,0,0,0,0,0 };
	memcpy(diff, &buf[1], sizeof(uint64_t));
	sync->timestamp =
		(uint64_t)diff[0] << 56 |
		(uint64_t)diff[1] << 48 |
		(uint64_t)diff[2] << 40 |
		(uint64_t)diff[3] << 32 |
		(uint64_t)diff[4] << 24 |
		(uint64_t)diff[5] << 16 |
		(uint64_t)diff[6] << 8 |
		(uint64_t)diff[7];

	memcpy(diff, &buf[1 + sizeof(uint64_t)], sizeof(uint64_t));
	sync->sid =
		(uint64_t)diff[0] << 56 |
		(uint64_t)diff[1] << 48 |
		(uint64_t)diff[2] << 40 |
		(uint64_t)diff[3] << 32 |
		(uint64_t)diff[4] << 24 |
		(uint64_t)diff[5] << 16 |
		(uint64_t)diff[6] << 8 |
		(uint64_t)diff[7];

	memcpy(diff, &buf[1 + sizeof(uint64_t) * 2], sizeof(uint32_t));
	sync->timer =
		(uint32_t)diff[0] << 24 |
		(uint32_t)diff[1] << 16 |
		(uint32_t)diff[2] << 8 |
		(uint32_t)diff[3];

	memcpy(diff, &buf[1 + sizeof(uint64_t) * 2 + sizeof(uint32_t)], sizeof(uint16_t));
	sync->ammo =
		(uint16_t)diff[0] << 8 |
		(uint16_t)diff[1];


	uint8_t subvec[3 * sizeof(uint32_t)] = { 0,0,0,0,0,0,0,0,0,0,0,0 };
	memcpy(subvec, &buf[1 + sizeof(uint64_t) * 2 + sizeof(uint32_t) + sizeof(uint16_t)], sizeof(uint8_t) * 12);
	uint32_t vec[3] = {
		(uint32_t)subvec[0] << 24 |
		(uint32_t)subvec[1] << 16 |
		(uint32_t)subvec[2] << 8 |
		(uint32_t)subvec[3],
			(uint32_t)subvec[4] << 24 |
			(uint32_t)subvec[5] << 16 |
			(uint32_t)subvec[6] << 8 |
			(uint32_t)subvec[7],
				(uint32_t)subvec[8] << 24 |
				(uint32_t)subvec[9] << 16 |
				(uint32_t)subvec[10] << 8 |
				(uint32_t)subvec[11] };

	memcpy(&sync->position, vec, sizeof(uint32_t) * 3);

	uint8_t hp8[4];

	memcpy(&hp8, &buf[1 + sizeof(uint64_t) * 2 + sizeof(uint32_t) * 4 + sizeof(uint16_t)], sizeof(uint32_t));

	uint32_t hp = (uint32_t)hp8[0] << 24 |
		(uint32_t)hp8[1] << 16 |
		(uint32_t)hp8[2] << 8 |
		(uint32_t)hp8[3];

	memcpy(&sync->hp, &hp, sizeof(float));

	uint8_t boost = 0;
	memcpy(&boost, &buf[2 + sizeof(uint64_t) * 2 + sizeof(uint32_t) * 5 + sizeof(uint16_t)], sizeof(uint8_t));
	sync->boost.invincible = boost & 0b10000000;
	sync->boost.damage = boost & 0b01000000;

	*buflen = 2 + sizeof(uint64_t) * 2 + sizeof(uint32_t) * 5 + sizeof(uint16_t) + sizeof(float);

	return true;
}

bool netprot::Deserialize(TeamInfo* tinfo, char* buf, uint32_t* buflen) {
	if (*buflen <= sizeof(LoginInfo))
		return false;

	size_t namesize = std::strlen(buf) + 1;

	if (namesize > 32)
		return false;

	strcpy(tinfo->name, namesize, &buf[1]);

	uint8_t diff[sizeof(uint64_t)] = { 0,0,0,0,0,0,0,0 };
	memcpy(diff, &buf[namesize + 1], sizeof(uint64_t));
	tinfo->id =
		(uint64_t)diff[0] << 56 |
		(uint64_t)diff[1] << 48 |
		(uint64_t)diff[2] << 40 |
		(uint64_t)diff[3] << 32 |
		(uint64_t)diff[4] << 24 |
		(uint64_t)diff[5] << 16 |
		(uint64_t)diff[6] << 8 |
		(uint64_t)diff[7];

	*buflen = namesize + sizeof(uint64_t) + 2;

	return true;
}

bool netprot::Deserialize(LoginInfo* linfo, char* buf, uint32_t* buflen) {
	if (*buflen <= sizeof(LoginInfo))
		return false;

	size_t namesize = std::strlen(buf) + 1;

	if (namesize > 32)
		return false;

	strcpy(linfo->name, namesize, &buf[1]);

	uint8_t diff[sizeof(uint64_t)] = { 0,0,0,0,0,0,0,0 };
	memcpy(diff, &buf[namesize + 1], sizeof(uint64_t));
	linfo->sid =
		(uint64_t)diff[0] << 56 |
		(uint64_t)diff[1] << 48 |
		(uint64_t)diff[2] << 40 |
		(uint64_t)diff[3] << 32 |
		(uint64_t)diff[4] << 24 |
		(uint64_t)diff[5] << 16 |
		(uint64_t)diff[6] << 8 |
		(uint64_t)diff[7];

	memcpy(diff, &buf[namesize + sizeof(uint64_t) + 1], sizeof(uint64_t));
	linfo->tid =
		(uint64_t)diff[0] << 56 |
		(uint64_t)diff[1] << 48 |
		(uint64_t)diff[2] << 40 |
		(uint64_t)diff[3] << 32 |
		(uint64_t)diff[4] << 24 |
		(uint64_t)diff[5] << 16 |
		(uint64_t)diff[6] << 8 |
		(uint64_t)diff[7];

	*buflen = namesize + sizeof(uint64_t) * 2 + 2;

	return true;
}

bool netprot::Deserialize(PlayerInfo* pinfo, char* buf, uint32_t* buflen) {
	if (*buflen <= sizeof(PlayerInfo))
		return false;

	size_t namesize = std::strlen(buf) + 1;

	if (namesize > 32)
		return false;

	strcpy(pinfo->name, namesize, &buf[1]);

	uint8_t diff[sizeof(uint64_t)] = { 0,0,0,0,0,0,0,0 };
	memcpy(diff, &buf[namesize + 1], sizeof(uint64_t));
	pinfo->id =
		(uint64_t)diff[0] << 56 |
		(uint64_t)diff[1] << 48 |
		(uint64_t)diff[2] << 40 |
		(uint64_t)diff[3] << 32 |
		(uint64_t)diff[4] << 24 |
		(uint64_t)diff[5] << 16 |
		(uint64_t)diff[6] << 8 |
		(uint64_t)diff[7];

	memcpy(diff, &buf[namesize + sizeof(uint64_t) + 1], sizeof(uint64_t));
	pinfo->tid =
		(uint64_t)diff[0] << 56 |
		(uint64_t)diff[1] << 48 |
		(uint64_t)diff[2] << 40 |
		(uint64_t)diff[3] << 32 |
		(uint64_t)diff[4] << 24 |
		(uint64_t)diff[5] << 16 |
		(uint64_t)diff[6] << 8 |
		(uint64_t)diff[7];

	*buflen = namesize + sizeof(uint64_t) * 2 + 2;

	return true;
}

bool netprot::Deserialize(GameInfo* ginfo, char* buf, uint32_t* buflen) {
	if (*buflen <= sizeof(GameInfo))
		return false;

	uint8_t diff[sizeof(uint64_t)] = { 0,0,0,0,0,0,0,0 };
	memcpy(diff, &buf[1], sizeof(uint64_t));
	ginfo->seed =
		(uint64_t)diff[0] << 56 |
		(uint64_t)diff[1] << 48 |
		(uint64_t)diff[2] << 40 |
		(uint64_t)diff[3] << 32 |
		(uint64_t)diff[4] << 24 |
		(uint64_t)diff[5] << 16 |
		(uint64_t)diff[6] << 8 |
		(uint64_t)diff[7];

	memcpy(diff, &buf[sizeof(uint64_t) + 1], sizeof(uint64_t));
	ginfo->countdown =
		(uint64_t)diff[0] << 56 |
		(uint64_t)diff[1] << 48 |
		(uint64_t)diff[2] << 40 |
		(uint64_t)diff[3] << 32 |
		(uint64_t)diff[4] << 24 |
		(uint64_t)diff[5] << 16 |
		(uint64_t)diff[6] << 8 |
		(uint64_t)diff[7];

	memcpy(diff, &buf[sizeof(uint64_t) * 2 + 1], sizeof(uint64_t));
	ginfo->gameType =
		(uint64_t)diff[0] << 56 |
		(uint64_t)diff[1] << 48 |
		(uint64_t)diff[2] << 40 |
		(uint64_t)diff[3] << 32 |
		(uint64_t)diff[4] << 24 |
		(uint64_t)diff[5] << 16 |
		(uint64_t)diff[6] << 8 |
		(uint64_t)diff[7];

	*buflen = sizeof(uint64_t) * 3 + 1;

	return true;
}

bool netprot::Deserialize(Chat* chat, char* buf, uint32_t* buflen) {
	if (*buflen <= sizeof(Chat))
		return false;

	uint8_t src[sizeof(uint64_t)] = { 0,0,0,0,0,0,0,0 };
	memcpy(src, &buf[1], sizeof(uint64_t));
	chat->src_id =
		(uint64_t)src[0] << 56 |
		(uint64_t)src[1] << 48 |
		(uint64_t)src[2] << 40 |
		(uint64_t)src[3] << 32 |
		(uint64_t)src[4] << 24 |
		(uint64_t)src[5] << 16 |
		(uint64_t)src[6] << 8 |
		(uint64_t)src[7];

	uint8_t dst[sizeof(uint64_t)] = { 0,0,0,0,0,0,0,0 };
	memcpy(dst, &buf[1 + sizeof(uint64_t)], sizeof(uint64_t));
	chat->dest_id =
		(uint64_t)dst[0] << 56 |
		(uint64_t)dst[1] << 48 |
		(uint64_t)dst[2] << 40 |
		(uint64_t)dst[3] << 32 |
		(uint64_t)dst[4] << 24 |
		(uint64_t)dst[5] << 16 |
		(uint64_t)dst[6] << 8 |
		(uint64_t)dst[7];

	uint8_t dstt[sizeof(uint64_t)] = { 0,0,0,0,0,0,0,0 };
	memcpy(dstt, &buf[1 + sizeof(uint64_t) * 2], sizeof(uint64_t));
	chat->dest_team_id =
		(uint64_t)dstt[0] << 56 |
		(uint64_t)dstt[1] << 48 |
		(uint64_t)dstt[2] << 40 |
		(uint64_t)dstt[3] << 32 |
		(uint64_t)dstt[4] << 24 |
		(uint64_t)dstt[5] << 16 |
		(uint64_t)dstt[6] << 8 |
		(uint64_t)dstt[7];

	size_t messsize = std::strlen(&buf[sizeof(uint64_t) * 3]) + 1;

	if (messsize > 140)
		return false;

	char* ciboire = &buf[1 + sizeof(uint64_t) * 3];

	strcpy(chat->mess, 140, ciboire);

	//*buflen = messsize + sizeof(uint64_t) * 3 + 1;

	return true;
}

bool netprot::Deserialize(ChunkMod* chmod, char* buf, uint32_t* buflen) {
	if (*buflen <= sizeof(ChunkMod))
		return false;

	uint8_t subvec[3 * sizeof(uint32_t)] = { 0,0,0,0,0,0,0,0,0,0,0,0 };
	memcpy(subvec, &buf[1], sizeof(uint8_t) * 12);
	uint32_t vec[3] = {
		(uint32_t)subvec[0] << 24 |
		(uint32_t)subvec[1] << 16 |
		(uint32_t)subvec[2] << 8 |
		(uint32_t)subvec[3],
			(uint32_t)subvec[4] << 24 |
			(uint32_t)subvec[5] << 16 |
			(uint32_t)subvec[6] << 8 |
			(uint32_t)subvec[7],
				(uint32_t)subvec[8] << 24 |
				(uint32_t)subvec[9] << 16 |
				(uint32_t)subvec[10] << 8 |
				(uint32_t)subvec[11] };

	memcpy(&chmod->pos, vec, sizeof(uint32_t) * 3);

	memcpy(&chmod->b_type, &buf[1 + sizeof(uint8_t) * 12], sizeof(BlockType));
	memcpy(&chmod->old_b_type, &buf[2 + sizeof(uint8_t) * 12], sizeof(BlockType));

	*buflen = sizeof(uint32_t) * 3 + 3;

	return true;
}

bool netprot::Deserialize(BulletAdd* bull, char* buf, uint32_t* buflen) {
	if (*buflen <= sizeof(BulletAdd))
		return false;

	uint8_t tst[sizeof(uint64_t)] = { 0,0,0,0,0,0,0,0 };
	memcpy(tst, &buf[1], sizeof(uint64_t));
	bull->tstamp =
		(uint64_t)tst[0] << 56 |
		(uint64_t)tst[1] << 48 |
		(uint64_t)tst[2] << 40 |
		(uint64_t)tst[3] << 32 |
		(uint64_t)tst[4] << 24 |
		(uint64_t)tst[5] << 16 |
		(uint64_t)tst[6] << 8 |
		(uint64_t)tst[7];

	memcpy(tst, &buf[1 + sizeof(uint64_t)], sizeof(uint64_t));
	bull->id =
		(uint64_t)tst[0] << 56 |
		(uint64_t)tst[1] << 48 |
		(uint64_t)tst[2] << 40 |
		(uint64_t)tst[3] << 32 |
		(uint64_t)tst[4] << 24 |
		(uint64_t)tst[5] << 16 |
		(uint64_t)tst[6] << 8 |
		(uint64_t)tst[7];

	uint8_t subvec[3 * sizeof(uint32_t)] = { 0,0,0,0,0,0,0,0,0,0,0,0 };
	memcpy(subvec, &buf[1 + sizeof(uint64_t) * 2], sizeof(uint8_t) * 12);
	uint32_t vec[3] = {
		(uint32_t)subvec[0] << 24 |
		(uint32_t)subvec[1] << 16 |
		(uint32_t)subvec[2] << 8 |
		(uint32_t)subvec[3],
			(uint32_t)subvec[4] << 24 |
			(uint32_t)subvec[5] << 16 |
			(uint32_t)subvec[6] << 8 |
			(uint32_t)subvec[7],
				(uint32_t)subvec[8] << 24 |
				(uint32_t)subvec[9] << 16 |
				(uint32_t)subvec[10] << 8 |
				(uint32_t)subvec[11] };

	memcpy(&bull->pos, vec, sizeof(uint32_t) * 3);

	memcpy(subvec, &buf[1 + sizeof(uint64_t) * 2 + sizeof(uint8_t) * 12], sizeof(uint8_t) * 12);

	uint32_t dir[3] = {
		(uint32_t)subvec[0] << 24 |
		(uint32_t)subvec[1] << 16 |
		(uint32_t)subvec[2] << 8 |
		(uint32_t)subvec[3],
			(uint32_t)subvec[4] << 24 |
			(uint32_t)subvec[5] << 16 |
			(uint32_t)subvec[6] << 8 |
			(uint32_t)subvec[7],
				(uint32_t)subvec[8] << 24 |
				(uint32_t)subvec[9] << 16 |
				(uint32_t)subvec[10] << 8 |
				(uint32_t)subvec[11] };

	memcpy(&bull->dir, dir, sizeof(uint32_t) * 3);

	*buflen = 1 + sizeof(uint64_t) * 2 + sizeof(uint8_t) * 24;

	return true;
}

bool netprot::Deserialize(ErrorLog* errlog, char* buf, uint32_t* buflen) {
	if (*buflen <= sizeof(ErrorLog))
		return false;

	size_t messsize = std::strlen(buf) + 1;

	if (messsize > 140)
		return false;

	memcpy(&errlog->mess, &buf[1], messsize);
	memcpy(&errlog->is_fatal, &buf[1 + messsize], sizeof(bool));

	*buflen = messsize + sizeof(uint64_t) * 3 + 2;

	return true;
}

bool netprot::Deserialize(PickupMod* pmod, char* buf, uint32_t* buflen) {
	if (*buflen <= sizeof(PickupMod))
		return false;

	uint8_t diff[sizeof(uint64_t)] = { 0,0,0,0,0,0,0,0 };
	memcpy(diff, &buf[1], sizeof(uint64_t));
	pmod->id =
		(uint64_t)diff[0] << 56 |
		(uint64_t)diff[1] << 48 |
		(uint64_t)diff[2] << 40 |
		(uint64_t)diff[3] << 32 |
		(uint64_t)diff[4] << 24 |
		(uint64_t)diff[5] << 16 |
		(uint64_t)diff[6] << 8 |
		(uint64_t)diff[7];

	uint8_t subvec[3 * sizeof(uint32_t)] = { 0,0,0,0,0,0,0,0,0,0,0,0 };
	memcpy(subvec, &buf[2 + sizeof(uint64_t)], sizeof(uint8_t) * 12);
	uint32_t vec[3] = {
		(uint32_t)subvec[0] << 24 |
		(uint32_t)subvec[1] << 16 |
		(uint32_t)subvec[2] << 8 |
		(uint32_t)subvec[3],
			(uint32_t)subvec[4] << 24 |
			(uint32_t)subvec[5] << 16 |
			(uint32_t)subvec[6] << 8 |
			(uint32_t)subvec[7],
				(uint32_t)subvec[8] << 24 |
				(uint32_t)subvec[9] << 16 |
				(uint32_t)subvec[10] << 8 |
				(uint32_t)subvec[11] };

	memcpy(&pmod->pos, vec, sizeof(uint32_t) * 3);

	uint8_t boosts = 0;
	memcpy(&boosts, &buf[1 + sizeof(uint64_t) + sizeof(uint32_t) * 3], sizeof(uint8_t));
	pmod->boost.invincible = boosts & 0b10000000;
	pmod->boost.damage = boosts & 0b01000000;
	pmod->boost.hp = boosts & 0b00100000;

	memcpy(&pmod->available, &buf[2 + sizeof(uint64_t) + sizeof(uint32_t) * 3], sizeof(bool));

	*buflen = 3 + sizeof(uint64_t) + sizeof(uint32_t) * 3;

	return true;
}



netprot::PACKET_TYPE netprot::getType(char* buf, const uint32_t buflen) {
	if (buflen < 1 ||
		buf[0] >= (char)netprot::PACKET_TYPE::LAST_PACK ||
		buf[0] <= (char)netprot::PACKET_TYPE::ERR)
		return netprot::PACKET_TYPE::ERR;
	return (netprot::PACKET_TYPE)buf[0];
}

netprot::Packet netprot::getPack(char* buf, uint32_t* buflen) {
	Packet pck = { nullptr, PACKET_TYPE::ERR };
	Input* in = nullptr;
	Output* out = nullptr;
	Sync* sync = nullptr;
	Chat* chat = nullptr;
	GameInfo* ginfo = nullptr;
	ErrorLog* errlog = nullptr;
	LoginInfo* loginf = nullptr;

	switch (getType(buf, *buflen)) {
	case PACKET_TYPE::INPUT:
		in = new Input();
		if (netprot::Deserialize(in, buf, buflen)) {
			pck.type = PACKET_TYPE::INPUT;
			pck.ptr = (void*)in;
		}
		break;
	case PACKET_TYPE::OUTPUT:
		out = new Output();
		if (netprot::Deserialize(out, buf, buflen)) {
			pck.type = PACKET_TYPE::OUTPUT;
			pck.ptr = (void*)out;
		}
		break;
	case PACKET_TYPE::SYNC:
		sync = new Sync();
		if (netprot::Deserialize(sync, buf, buflen)) {
			pck.type = PACKET_TYPE::SYNC;
			pck.ptr = (void*)sync;
		}
		break;
	case PACKET_TYPE::CHAT:
		chat = new Chat();
		if (netprot::Deserialize(chat, buf, buflen)) {
			pck.type = PACKET_TYPE::CHAT;
			pck.ptr = (void*)chat;
		}
		break;
	case PACKET_TYPE::GAMEINFO:
		ginfo = new GameInfo();
		if (netprot::Deserialize(ginfo, buf, buflen)) {
			pck.type = PACKET_TYPE::GAMEINFO;
			pck.ptr = (void*)ginfo;
		}
		break;
	case PACKET_TYPE::ERRLOG:
		errlog = new ErrorLog();
		if (netprot::Deserialize(errlog, buf, buflen)) {
			pck.type = PACKET_TYPE::ERRLOG;
			pck.ptr = (void*)errlog;
		}
		break;
	case PACKET_TYPE::LOGINF:
		loginf = new LoginInfo();
		if (netprot::Deserialize(loginf, buf, buflen)) {
			pck.type = PACKET_TYPE::LOGINF;
			pck.ptr = (void*)loginf;
		}
		break;
	default:
		break;
	}
	return pck;
}

netprot::Packet netprot::getPack(netprot::Buffer* buf) { return netprot::getPack(buf->ptr, &buf->len); }

bool netprot::emptyPack(netprot::Packet pck) {
	switch (pck.type) {
	case PACKET_TYPE::INPUT:
		delete (Input*)pck.ptr;
		return true;
	case PACKET_TYPE::OUTPUT:
		delete (Output*)pck.ptr;
		return true;
	case PACKET_TYPE::SYNC:
		delete (Sync*)pck.ptr;
		return true;
	case PACKET_TYPE::CHAT:
		delete (Chat*)pck.ptr;
		return true;
	case PACKET_TYPE::GAMEINFO:
		delete (GameInfo*)pck.ptr;
		return true;
	case PACKET_TYPE::ERRLOG:
		delete (ErrorLog*)pck.ptr;
		return true;
	case PACKET_TYPE::LOGINF:
		delete (LoginInfo*)pck.ptr;
		return true;
	default:
		return false;
	}
}

netprot::Packet netprot::makePack(void* ptr, PACKET_TYPE type) {
	Packet pck;
	pck.ptr = ptr;
	pck.type = type;
	return pck;
}

void netprot::recvPacks(SOCKET sock, Buffer* buf, std::vector<char*>* lsPck) {
	int len = buf->tmp ? buf->tmp - buf->ptr : 0,
		end = 0;
	char* cursor = buf->tmp ? buf->tmp : nullptr,
		* next = buf->tmp ? buf->tmp + 1 : buf->ptr,
		* last = buf->tmp ? buf->tmp : buf->ptr;
	bool ended = true;
	struct pollfd fds[1];

	fds[0].fd = sock;
	fds[0].events = POLLIN;

	while (true) {
		if (!poll(fds, 1, 0)) {
			if (ended)
				buf->tmp = nullptr;
			return;
		}

		int bytes = recv(sock, &buf->ptr[len], buf->len - len, 0);
		if (bytes <= 0) { // si recv() retourne -1 ou 0; ça veut dire qu'il y a plus rien a lire qui n'a pas déjà été traité.
			if (ended)
				buf->tmp = nullptr;
			return;
		}
		len += bytes;
		end = len;

		ended = false;

		while (true) {
			int cmp = 0;

			if (cursor)
				end -= (cursor - buf->ptr);

			if (end < 0)
				break;

			cursor = (char*)memchr(next, '\r', end);

			if (cursor) {
				next = cursor;
				cursor--;

				cmp = memcmp(cursor, Footer, sizeof(uint32_t));
				if (cmp == 0) {
					lsPck->push_back(last);
					cursor += sizeof(uint32_t);
					last = cursor;
					next = cursor + 1;
				}
			}
			else {
				buf->tmp = last;
				cursor = &buf->ptr[len];
				next = cursor + 1;
				break;
			};
		}
	}
}

void netprot::recvPacksFrom(SOCKET sock, Buffer* buf, sockaddr_in from, std::vector<char*>* lsPck) {
	int len = buf->tmp ? buf->tmp - buf->ptr : 0,
		end = 0;
	char* cursor = buf->tmp ? buf->tmp : nullptr,
		* next = buf->tmp ? buf->tmp + 1 : buf->ptr,
		* last = buf->tmp ? buf->tmp : buf->ptr;
	bool ended = true;
	struct pollfd fds[1];
	sockaddr_in sockad = from;
	addrlen_t socklen = sizeof(sockad);

	fds[0].fd = sock;
	fds[0].events = POLLIN;

	while (true) {
		if (!poll(fds, 1, 0)) {
			if (ended)
				buf->tmp = nullptr;
			return;
		}

		int bytes = recvfrom(sock, &buf->ptr[len], buf->len - len, 0, (sockaddr*)&sockad, &socklen);
		if (bytes <= 0) { // si recv() retourne -1 ou 0; ça veut dire qu'il y a plus rien a lire qui n'a pas déjà été traité.
			if (ended)
				buf->tmp = nullptr;
			return;
		}
		len += bytes;
		end = len;

		ended = false;

		while (true) {
			int cmp = 0;

			if (cursor)
				end -= (cursor - buf->ptr);

			if (end < 0)
				break;

			cursor = (char*)memchr(next, '\r', end);

			if (cursor) {
				next = cursor;
				cursor--;

				cmp = memcmp(cursor, Footer, sizeof(uint32_t));
				if (cmp == 0) {
					lsPck->push_back(last);
					cursor += sizeof(uint32_t);
					last = cursor;
					next = cursor + 1;
				}
			}
			else {
				buf->tmp = last;
				cursor = &buf->ptr[len];
				next = cursor + 1;
				break;
			};
		}
	}
}

template <>
void netprot::sendPack<netprot::Packet>(SOCKET sock, Packet* pack, Buffer* buf) {
	switch (pack->type) {
	case PACKET_TYPE::INPUT:
		sendPack<Input>(sock, (Input*)pack->ptr, buf);
		return;
	case PACKET_TYPE::OUTPUT:
		sendPack<Output>(sock, (Output*)pack->ptr, buf);
		return;
	case PACKET_TYPE::SYNC:
		sendPack<Sync>(sock, (Sync*)pack->ptr, buf);
		return;
	case PACKET_TYPE::TEAMINF:
		sendPack<TeamInfo>(sock, (TeamInfo*)pack->ptr, buf);
		return;
	case PACKET_TYPE::PLAYINF:
		sendPack<PlayerInfo>(sock, (PlayerInfo*)pack->ptr, buf);
		return;
	case PACKET_TYPE::LOGINF:
		sendPack<LoginInfo>(sock, (LoginInfo*)pack->ptr, buf);
		return;
	case PACKET_TYPE::CHUNKMOD:
		//sendPack<ChunkMod>(sock, (ChunkMod*)pack->ptr, buf);
		return;
	case PACKET_TYPE::PLAYERMOD:
		//sendPack<PlayerMod>(sock, (PlayerMod*)pack->ptr, buf);
		return;
	case PACKET_TYPE::PICKUPMOD:
		//sendPack<PickupMod>(sock, (PickupMod*)pack->ptr, buf);
		return;
	case PACKET_TYPE::GAMEINFO:
		sendPack<GameInfo>(sock, (GameInfo*)pack->ptr, buf);
		return;
	case PACKET_TYPE::ENDINFO:
		//sendPack<EndInfo>(sock, (EndInfo*)pack->ptr, buf);
		return;
	case PACKET_TYPE::CHAT:
		sendPack<Chat>(sock, (Chat*)pack->ptr, buf);
		return;
	case PACKET_TYPE::ERRLOG:
		sendPack<ErrorLog>(sock, (ErrorLog*)pack->ptr, buf);
		return;
	case PACKET_TYPE::LAST_PACK: [[fallthrough]];
	case PACKET_TYPE::ERR: [[fallthrough]];
	default:
		return;
	}
}

template <>
void netprot::sendPackTo<netprot::Packet>(SOCKET sock, Packet* pack, Buffer* buf, sockaddr_in* sockad) {
	switch (pack->type) {
	case PACKET_TYPE::INPUT:
		sendPackTo<Input>(sock, (Input*)pack->ptr, buf, sockad);
		return;
	case PACKET_TYPE::OUTPUT:
		sendPackTo<Output>(sock, (Output*)pack->ptr, buf, sockad);
		return;
	case PACKET_TYPE::SYNC:
		sendPackTo<Sync>(sock, (Sync*)pack->ptr, buf, sockad);
		return;
	case PACKET_TYPE::TEAMINF:
		sendPackTo<TeamInfo>(sock, (TeamInfo*)pack->ptr, buf, sockad);
		return;
	case PACKET_TYPE::PLAYINF:
		sendPackTo<PlayerInfo>(sock, (PlayerInfo*)pack->ptr, buf, sockad);
		return;
	case PACKET_TYPE::LOGINF:
		sendPackTo<LoginInfo>(sock, (LoginInfo*)pack->ptr, buf, sockad);
		return;
	case PACKET_TYPE::CHUNKMOD:
		//sendPackTo<ChunkMod>(sock, (ChunkMod*)pack->ptr, buf, sockad);
		return;
	case PACKET_TYPE::PLAYERMOD:
		//sendPackTo<PlayerMod>(sock, (PlayerMod*)pack->ptr, buf, sockad);
		return;
	case PACKET_TYPE::PICKUPMOD:
		//sendPackTo<PickupMod>(sock, (PickupMod*)pack->ptr, buf, sockad);
		return;
	case PACKET_TYPE::GAMEINFO:
		sendPackTo<GameInfo>(sock, (GameInfo*)pack->ptr, buf, sockad);
		return;
	case PACKET_TYPE::ENDINFO:
		//sendPackTo<EndInfo>(sock, (EndInfo*)pack->ptr, buf, sockad);
		return;
	case PACKET_TYPE::CHAT:
		sendPackTo<Chat>(sock, (Chat*)pack->ptr, buf, sockad);
		return;
	case PACKET_TYPE::ERRLOG:
		sendPackTo<ErrorLog>(sock, (ErrorLog*)pack->ptr, buf, sockad);
		return;
	case PACKET_TYPE::LAST_PACK: [[fallthrough]];
	case PACKET_TYPE::ERR: [[fallthrough]];
	default:
		return;
	}
}
