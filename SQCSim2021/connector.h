#ifndef CONNECTOR_H__
#define CONNECTOR_H__

#include <stdlib.h>
#include <unordered_map>
#include "../SQCSim-common/netprotocol.h"
#include "define.h"

class Connector {
public:
	Connector();
	~Connector();

	int Init();
	int Connect(const char* srv_addr, std::string name);
	uint64_t getId() const;
	unsigned int getSeed() const;
	netprot::Sync getOrigin() const;


	//void SendInput();
	//int Sync();

	SOCKET m_sock_udp = 0,
		m_sock_tcp = 0;
	sockaddr_in m_srvsockaddr;

	// void updateRemotePlayers(std::map<RemotePlayers> rplayers);

	std::unordered_map<uint64_t, netprot::PlayerInfo> m_players;
private:
#ifdef _WIN32
	WSADATA m_wsaData;
#endif
	std::unordered_map<Timestamp, netprot::Input> m_inputmanifest;
	std::unordered_map<uint64_t, netprot::TeamInfo> m_teams;

	netprot::LoginInfo m_loginfo;
	netprot::GameInfo m_gameinfo;
	netprot::Sync m_origin;

};
#endif
