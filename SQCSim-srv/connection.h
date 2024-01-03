#ifndef CONNECTION_H__
#define CONNECTION_H__

#include <deque>
#include <unordered_map>
#include "../SQCSim-common/player.h"
#include "../SQCSim-common/vector3.h"
#include "../SQCSim-common/netprotocol.h"
#include "../SQCSim-common/world.h"
#include "define.h"

using namespace netprot;

class Connection {
public:
	Connection(
		SOCKET sock,
		sockaddr_in sockaddr,
		LoginInfo *log,
		PlayerInfo *play);
	~Connection();

	Player* player = nullptr;

	uint64_t GetHash(bool self = true) const;
	uint64_t GetTeamHash() const;
	std::string GetName() const;

	void AddInput(Input in);
	Output* getOutput(Timestamp time);
	Sync getSync(Timestamp time);
	SOCKET getSock() const;
	PlayerInfo* getInfo() const;
	sockaddr_in* getAddr() const;

	void getPacks(SOCKET sock);
	void sendPacks(SOCKET sock, std::unordered_map<uint64_t, Connection*> conns, const uint32_t timer);

	Timestamp Run(World* world, std::unordered_map<uint64_t, Booster*> boosters);

	void CleanInputManifest(Timestamp time);
	
	bool m_nsync = true;

	std::vector<Bullet*> Bullets;
	std::vector<ChunkMod*> ChunkDiffs;

	Timestamp GetTStamp() const;

private:
	std::unordered_map<Timestamp, Input> m_input_manifest;
	std::vector<Input> m_input_vector;
	std::unordered_map<Timestamp, Output> m_output_manifest;
	std::deque<Output> m_output_vector;
	std::unordered_map<Timestamp, Chat> m_chatlog;

	float m_shoot_acc = 0;
	Timestamp m_tstamp = 0;

	SOCKET m_sock;
	sockaddr_in m_addr;
	LoginInfo m_loginfo;
	PlayerInfo m_playinfo;

	Timestamp m_startsync;

	uint64_t m_last_in = 0, 
			 m_last_out = 0;
	Buffer m_buf,
		   m_bufout;
};
#endif
