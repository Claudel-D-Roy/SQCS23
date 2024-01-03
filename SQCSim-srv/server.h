#ifndef SERVER_H__
#define SERVER_H__

#include <cstdlib>
#include <fstream>
#include <vector>
#include <set>
#include <string>
#include <unordered_map>
#include "../SQCSim-common/world.h"
#include "../SQCSim-common/netprotocol.h"
#include "define.h"
#include "connection.h"

using namespace netprot;

class Server {
public:
	enum LOG_DEST: unsigned char { CONSOLE, LOGFILE, LOG_LAST };

	Server(LOG_DEST log = LOG_DEST::CONSOLE);
	~Server();

	int Init();
	int Ready();
	void Run();
	void Cleanup();
	void DeInit();
	bool NewGameRequested() const;

private:

#ifdef _WIN32
	WSADATA m_wsaData;
#endif
	SOCKET m_sock_udp = 0,
		   m_sock_tcp = 0;
	LOG_DEST m_log;
	std::ofstream m_logfile;

	Buffer m_buf;

	std::unordered_map<uint64_t, Player*> m_players;
	std::unordered_map<uint64_t, Connection*> m_conns;
	std::unordered_map<Timestamp, Chat> m_chatlog;
	std::unordered_map<uint64_t, Booster*> m_boosters;
	std::vector<uint64_t> m_ids;
	GameInfo m_game;

	World* m_world = nullptr;
	bool m_exit = true;

	int64_t m_boostcount = -1;

	std::string LogTimestamp();
	void Log(std::string str, bool is_error, bool is_fatal);
	void buildIdList(size_t size);

	uint64_t getUniqueId(); 	
	std::string getDeathMessage(std::string username, std::string killer) const;

};

#endif
