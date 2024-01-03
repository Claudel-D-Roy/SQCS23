#include "connector.h"

Connector::Connector() {}

Connector::~Connector() {}

int Connector::Init() {
#ifdef _WIN32
	if (WSAStartup(MAKEWORD(2, 2), &m_wsaData) != 0) { /* Initialisation de l'environnement reseau (Windows only) */
		std::cout << "Initialisation WinSock." << std::endl;
		return 1;
	}
#endif

	m_sock_udp = socket(AF_INET, SOCK_DGRAM, 0);
	if (m_sock_udp == INVALID_SOCKET) { /* Creation du socket UDP */
		std::cout << "Creation Socket UDP." << std::endl;
		return 2;
	}

	m_sock_tcp = socket(AF_INET, SOCK_STREAM, 0);
	if (m_sock_tcp == INVALID_SOCKET) { /* Creation du socket TCP */
		std::cout << "Creation Socket TCP." << std::endl;
		return 3;
	}

	/* Creation structure donnes descripteur du socket serveur */
	sockaddr_in addr;
	addr.sin_family = AF_INET;
	addr.sin_port = htons(CLI_PORT);
	addr.sin_addr.s_addr = htonl(INADDR_ANY);

	if (bind(m_sock_udp, (sockaddr*)&addr, sizeof(addr)) != 0) { /* Associer le socket UDP au port */
		std::cout << "Association Socket UDP." << std::endl;
		return 4;
	}

	if (bind(m_sock_tcp, (sockaddr*)&addr, sizeof(addr)) != 0) { /* Associer le socket TCP au port */
		std::cout << "Association Socket TCP." << std::endl;
		return 5;
	}
	return 0;
}

int Connector::Connect(const char* srv_addr, std::string name) {
	sockaddr_in add;
	m_srvsockaddr.sin_family = AF_INET;
	m_srvsockaddr.sin_port = htons(SRV_PORT);

	if (inet_pton(AF_INET, srv_addr, &m_srvsockaddr.sin_addr) <= 0) {
		std::cout << "Addresse serveur invalide." << std::endl;
		return 1;
	}

	if (connect(m_sock_tcp, (sockaddr*)&m_srvsockaddr, sizeof(m_srvsockaddr)) < 0) {
		std::cout << "Échec de la connexion." << std::endl;
		return 2;
	}

	flag_t flag = 1;
	if (ioctl(m_sock_tcp, SOCK_NONBLOCK, &flag) < 0) {
		std::cout << "Impossible de mettre le socket en mode non-bloquant." << std::endl;
		return 3;
	}

	netprot::Buffer bf;
	netprot::LoginInfo log;
	strcpy(log.name, 32, name.c_str());

	netprot::sendPack(m_sock_tcp, &log, &bf);

	bool ready = false;
	int errors = 0;
	std::vector<char*> lsPck;
	while (!ready) {
		netprot::recvPacks(m_sock_udp, &bf, &lsPck);

		for (auto& pck : lsPck) {
			uint32_t bsize = bf.len - (pck - bf.ptr);
			netprot::PlayerInfo* pl = nullptr;
			switch (netprot::getType(pck, 1)) {
				using enum netprot::PACKET_TYPE;
			case LOGINF:
				if (!netprot::Deserialize(&m_loginfo, pck, &bsize))
					++errors;
				break;
			case GAMEINFO:
				if (!netprot::Deserialize(&m_gameinfo, pck, &bsize))
					++errors;
				break;
			case PLAYINF:
				pl = new netprot::PlayerInfo();
				if (!netprot::Deserialize(pl, pck, &bsize))
					++errors;
				else {
					m_players[pl->id] = pl;
					std::cout << "A challenger appears! " << pl->name << std::endl;
				}
				break;
			case TEAMINF:
				// TODO: Faire dequoi avec TeamInfo si on fini par avoir des teams.
				break;
			case SYNC:
				if (!netprot::Deserialize(&m_origin, pck, &bsize))
					++errors;
				ready = true;
				break;
			default:
				errors++;
				break;
			}
		}
		lsPck.clear();
		if (errors > 1000)
			return 4;
	}
	return 0;
}

uint64_t Connector::getId() const { return m_loginfo.sid; }

unsigned int Connector::getSeed() const { return m_gameinfo.seed; }

netprot::Sync Connector::getOrigin() const { return m_origin; }
